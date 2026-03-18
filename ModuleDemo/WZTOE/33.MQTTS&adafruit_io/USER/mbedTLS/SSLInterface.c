/*
 * file: SSLInterface.c
 * description: mbedtls callback functions
 * author: peter
 * company: wiznet
 * data: 2015.11.26
 */
#include <stdio.h>
#include <string.h>
#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"
#include "SSLInterface.h"
#include "SSL_Random.h"
#include "socket.h"
#include "bsp_tim.h"
#include "util.h"
#include "delay.h"

int WIZnetRecvTimeOut(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    uint32_t start_ms = millis();
    do
    {
        if (getSn_RX_RSR((uint8_t)(int)ctx))
        {
            return recv((uint8_t)(int)ctx, (uint8_t *)buf, (uint16_t)len);
        }
    } while ((millis() - start_ms) < timeout);

    return 0;
}

/*Shell for mbedtls recv function*/
int WIZnetRecv(void *ctx, unsigned char *buf, unsigned int len)
{
    return (recv((uint8_t)(int)ctx, (uint8_t *)buf, (uint16_t)len));
}

/*Shell for mbedtls recv non-block function*/
int WIZnetRecvNB(void *ctx, unsigned char *buf, unsigned int len)
{
    uint32_t recv_len = 0;

    getsockopt((uint8_t)(int)(ctx), SO_RECVBUF, &recv_len);
    if (recv_len > 0)
        return recv((uint8_t)(int)ctx, (uint8_t *)buf, (uint16_t)len);
    else
        return 0;
}


/*Shell for mbedtls send function*/
int WIZnetSend(void *ctx, const unsigned char *buf, unsigned int len)
{
    return (send((uint8_t)(int)ctx, (uint8_t *)buf, (uint16_t)len));
}

/*Shell for mbedtls debug function.
 *DEBUG_LEBEL can be changed from 0 to 3*/
#ifdef MBEDTLS_DEBUG_C
void WIZnetDebugCB(void *ctx, int level, const char *file, int line, const char *str)
{
    if (level <= MBEDTLS_DEBUG_LEVEL)
    {
        printf("%s\r\n", str);
    }
}
#endif


/* SSL context initialization
 * */
int wiz_tls_init(wiz_tls_context *tlsContext, uint32_t recv_timeout, char *domain_name, char *root_ca, char *client_cert, char *pkey)
{
    int         ret             = 1;

#if defined(MBEDTLS_ENTROPY_C)
    const char *pers            = "ssl_client1";
#endif
    //const char *alpnProtocols[] = {"x-amzn-mqtt-ca", NULL};
#if defined(MBEDTLS_ERROR_C)
    char error_buf[100];
#endif

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
#endif

    /*
    Initialize session data
  */
#if defined(MBEDTLS_ENTROPY_C)
    tlsContext->entropy = malloc(sizeof(mbedtls_entropy_context));
#endif
    tlsContext->ctr_drbg = malloc(sizeof(mbedtls_ctr_drbg_context));
    tlsContext->ssl      = malloc(sizeof(mbedtls_ssl_context));
    tlsContext->conf     = malloc(sizeof(mbedtls_ssl_config));
    tlsContext->cacert   = malloc(sizeof(mbedtls_x509_crt));
    tlsContext->clicert  = malloc(sizeof(mbedtls_x509_crt));
    tlsContext->pkey     = malloc(sizeof(mbedtls_pk_context));

#if defined(MBEDTLS_ENTROPY_C)
    mbedtls_entropy_init(tlsContext->entropy);
#endif

    mbedtls_ctr_drbg_init(tlsContext->ctr_drbg);
    mbedtls_ssl_init(tlsContext->ssl);
    mbedtls_ssl_config_init(tlsContext->conf);
    mbedtls_x509_crt_init(tlsContext->cacert);
    mbedtls_x509_crt_init(tlsContext->clicert);
    mbedtls_pk_init(tlsContext->pkey);

    const int *ciphersuite_list = mbedtls_ssl_list_ciphersuites();
    while (*ciphersuite_list != 0)
    {
        const char *name = mbedtls_ssl_get_ciphersuite_name(*ciphersuite_list);
        if (name != NULL)
            printf("%s\r\n", name);
        ciphersuite_list++;
    }
    /*
    Initialize certificates
  */
#if defined(MBEDTLS_ENTROPY_C)
    if ((ret = mbedtls_ctr_drbg_seed(tlsContext->ctr_drbg, mbedtls_entropy_func, tlsContext->entropy,
                                     (const unsigned char *)pers, strlen(pers)))
        != 0)
    {
        printf(" failed\r\n  ! mbedtls_ctr_drbg_seed returned -0x%x\r\n", -ret);
        return -1;
    }
#endif

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(tlsContext->conf, WIZnetDebugCB, stdout);
#endif

    /*
    Parse certificate
  */
    if (root_ca != NULL)
    {
        uint16_t root_ca_len = strlen(root_ca);
        printf(" Loading the CA root certificate len = %d\r\n", root_ca_len);
        ret = mbedtls_x509_crt_parse(tlsContext->cacert, (const unsigned char *)root_ca, root_ca_len + 1);
        if (ret < 0)
        {
            printf(" failed\r\n  !  mbedtls_x509_crt_parse returned -0x%x while parsing root cert\r\n", -ret);
            return -1;
        }
        printf("ok! mbedtls_x509_crt_parse returned -0x%x while parsing root cert\r\n", -ret);

        uint8_t ip_temp[4];
        if (!is_ipaddr((uint8_t *)domain_name, (uint8_t *)ip_temp))
        {
            if ((ret = mbedtls_ssl_set_hostname(tlsContext->ssl, domain_name)) != 0)
            {
                printf(" failed mbedtls_ssl_set_hostname returned %d\r\n", ret);
                return -1;
            }
        }
        else
        {
            if ((ret = mbedtls_ssl_set_hostname(tlsContext->ssl, NULL)) != 0)
            {
                printf(" failed mbedtls_ssl_set_hostname returned %d\r\n", ret);
                return -1;
            }
        }
        printf("ok! mbedtls_ssl_set_hostname returned %d\r\n", ret);
        tlsContext->root_ca_option = MBEDTLS_SSL_VERIFY_REQUIRED;
    }
    else
    {
        tlsContext->root_ca_option = MBEDTLS_SSL_VERIFY_NONE;
    }

    if (client_cert != NULL && pkey != NULL)
    {
        uint32_t client_cert_len = strlen(client_cert);
        uint32_t pkey_len        = strlen(pkey);

        ret = mbedtls_x509_crt_parse((tlsContext->clicert), (const unsigned char *)client_cert, client_cert_len + 1);
        if (ret != 0)
        {
            printf(" failed\r\n  !  mbedtls_x509_crt_parse returned -0x%x while parsing device cert\r\n", -ret);
            return -1;
        }
        printf("ok! mbedtls_x509_crt_parse returned -0x%x while parsing device cert\r\n", -ret);

        ret = mbedtls_pk_parse_key(tlsContext->pkey, (const unsigned char *)pkey, pkey_len + 1, NULL, 0, mbedtls_ctr_drbg_random, tlsContext->ctr_drbg);
        if (ret != 0)
        {
            printf(" failed\r\n  !  mbedtls_pk_parse_key returned -0x%x while parsing private key\r\n", -ret);
            return -1;
        }
        printf("ok! mbedtls_pk_parse_key returned -0x%x while parsing private key\r\n", -ret);
    }

    if ((ret = mbedtls_ssl_config_defaults(tlsContext->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT))
        != 0)
    {
        printf(" failed mbedtls_ssl_config_defaults returned %d\r\n", ret);
        return -1;
    }

    //printf("socket_fd = %d\r\n", tlsContext->socket_fd);
    mbedtls_ssl_conf_authmode(tlsContext->conf, tlsContext->root_ca_option);
    mbedtls_ssl_conf_ca_chain(tlsContext->conf, tlsContext->cacert, NULL);
    mbedtls_ssl_conf_rng(tlsContext->conf, SSLRandomCB, tlsContext->ctr_drbg);

    if (client_cert != NULL && pkey != NULL)
    {
        if ((ret = mbedtls_ssl_conf_own_cert(tlsContext->conf, tlsContext->clicert, tlsContext->pkey)) != 0)
        {
            printf("failed! mbedtls_ssl_conf_own_cert returned %d\r\n", ret);
            return -1;
        }
        printf("ok! mbedtls_ssl_conf_own_cert returned %d\r\n", ret);
    }

    mbedtls_ssl_conf_endpoint(tlsContext->conf, MBEDTLS_SSL_IS_CLIENT);
    if (recv_timeout == 0)
        recv_timeout = 2000;
    mbedtls_ssl_conf_read_timeout(tlsContext->conf, recv_timeout);

    if ((ret = mbedtls_ssl_setup(tlsContext->ssl, tlsContext->conf)) != 0)
    {
        printf(" failed mbedtls_ssl_setup returned -0x%x\r\n", -ret);
        return -1;
    }
    mbedtls_ssl_set_bio(tlsContext->ssl, (void *)tlsContext->socket_fd, SSLSendCB, SSLRecvCB, SSLRecvTimeOutCB);

    return 1;
}

/*Free the memory for ssl context*/
void wiz_tls_deinit(wiz_tls_context *tlsContext)
{
    /*  free SSL context memory  */

    printf("SSL Free\r\n");
    mbedtls_ssl_free(tlsContext->ssl);
    mbedtls_ssl_config_free(tlsContext->conf);
    mbedtls_ctr_drbg_free(tlsContext->ctr_drbg);
#if defined(MBEDTLS_ENTROPY_C)
    mbedtls_entropy_free(tlsContext->entropy);
#endif
    mbedtls_x509_crt_free(tlsContext->cacert);
    mbedtls_x509_crt_free(tlsContext->clicert);
    mbedtls_pk_free(tlsContext->pkey);

#if defined(MBEDTLS_ENTROPY_C)
    free(tlsContext->entropy);
#endif
    free(tlsContext->ctr_drbg);
    free(tlsContext->ssl);
    free(tlsContext->conf);
    free(tlsContext->cacert);
    free(tlsContext->clicert);
    free(tlsContext->pkey);
}

int wiz_tls_socket(wiz_tls_context *tlsContext, uint8_t sock, unsigned int port)
{
    /*socket open*/
    tlsContext->socket_fd = sock;
    //return socket((uint8_t)(tlsContext->socket_fd), Sn_MR_TCP, (uint16_t)port, (SF_TCP_NODELAY | SF_IO_NONBLOCK));
    return socket((uint8_t)(tlsContext->socket_fd), Sn_MR_TCP, (uint16_t)port, 0x00);
}

int wiz_tls_connect(wiz_tls_context *tlsContext, char *addr, unsigned int port)
{
    int      ret;
    uint32_t start_ms = millis(), flags;

    printf("[SSL] Performing the SSL/TLS handshake...\r\n");
    while ((ret = mbedtls_ssl_handshake(tlsContext->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            //mbedtls_strerror(ret, (char *) tempBuf, DEBUG_BUFFER_SIZE );
            //printf( " failed\n\r  ! mbedtls_ssl_handshake returned %d: %s\n\r", ret, tempBuf );
            printf(" failed\n\r  ! mbedtls_ssl_handshake returned -0x%x\n\r", -ret);
            return (-1);
        }
        delay_ms(10);
    }

    if (tlsContext->root_ca_option == MBEDTLS_SSL_VERIFY_REQUIRED)
    {
        printf("  . Verifying peer X.509 certificate...\r\n");

        /* In real life, we probably want to bail out when ret != 0 */
        if ((flags = mbedtls_ssl_get_verify_result(tlsContext->ssl)) != 0)
        {
            char vrfy_buf[512];
            printf("failed\r\n");
            mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
            printf("%s\r\n", vrfy_buf);
            return -1;
        }
        else
        {
            printf("ok\r\n");
        }
    }
    printf("[SSL] SSL/TLS handshake ok\n\r");
    printf("[ Ciphersuite is %s ]\n\r",
           mbedtls_ssl_get_ciphersuite(tlsContext->ssl));
    return (0);
}

/* SSL handshake */
int wiz_tls_socket_connect(wiz_tls_context *tlsContext, char *addr, unsigned int port)
{
    int     ret;
    uint8_t sock = (uint8_t)(tlsContext->socket_fd);

#if defined(MBEDTLS_ERROR_C)
    char error_buf[1024];
#endif
    /*socket open*/
    ret = socket(sock, Sn_MR_TCP, 0, 0x00);
    if (ret != sock)
        return ret;

    /*Connect to the target*/
    ret = connect(sock, (uint8_t *)addr, port);
    if (ret != SOCK_OK)
        return ret;

#if defined(MBEDTLS_DEBUG_C)
    printf("[TLS] Performing the SSL/TLS handshake...\r\n");
#endif

    while ((ret = mbedtls_ssl_handshake(tlsContext->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
#if defined(MBEDTLS_ERROR_C)
            memset(error_buf, 0, 1024);
            mbedtls_strerror(ret, (char *)error_buf, DEBUG_BUFFER_SIZE);
            printf(" failed\n\r  ! mbedtls_ssl_handshake returned %d: %s\n\r", ret, error_buf);
#endif
            return (-1);
        }
    }

#if defined(MBEDTLS_DEBUG_C)
    printf(" ok\n\r    [ Ciphersuite is %s ]\n\r",
           mbedtls_ssl_get_ciphersuite(tlsContext->ssl));
#endif

    return (0);
}

int wiz_tls_close(wiz_tls_context *tlsContext)
{
    uint8_t sock = (uint8_t)(tlsContext->socket_fd);

    wiz_tls_close_notify(tlsContext);
    wiz_tls_session_reset(tlsContext);
    wiz_tls_deinit(tlsContext);
    close(sock);

    return (0);
}

unsigned int wiz_tls_read(wiz_tls_context *tlsContext, unsigned char *readbuf, unsigned int len)
{
    return mbedtls_ssl_read(tlsContext->ssl, readbuf, len);
}

unsigned int wiz_tls_write(wiz_tls_context *tlsContext, unsigned char *writebuf, unsigned int len)
{
    return mbedtls_ssl_write(tlsContext->ssl, writebuf, len);
}

int wiz_tls_disconnect(wiz_tls_context *tlsContext, uint32_t timeout)
{
    int      ret       = 0;
    uint8_t  sock      = (uint8_t)(tlsContext->socket_fd);
    uint32_t tickStart = millis();

    do {
        ret = disconnect(sock);
        if ((ret == SOCK_OK) || (ret == SOCKERR_TIMEOUT)) break;
    } while ((millis() - tickStart) < timeout);

    if (ret == SOCK_OK)
        ret = sock; // socket number

    return ret;
}


/* ssl Close notify */
unsigned int wiz_tls_close_notify(wiz_tls_context *tlsContext)
{
    int rc;
    do rc = mbedtls_ssl_close_notify(tlsContext->ssl);
    while (rc == MBEDTLS_ERR_SSL_WANT_WRITE);
    return rc;
}


/* ssl session reset */
int wiz_tls_session_reset(wiz_tls_context *tlsContext)
{
    return mbedtls_ssl_session_reset(tlsContext->ssl);
}

