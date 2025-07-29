/**
 * @file tftp.c
 * @brief TFTP Source File.
 * @version 0.1.0
 * @author Sang-sik Kim
 */

/* Includes -----------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "tftp.h"
#include "socket.h"
#include "netutil.h"

/* define -------------------------------------------------------*/

/* typedef ------------------------------------------------------*/

/* Extern Variable ----------------------------------------------*/

/* Extern Functions ---------------------------------------------*/
#ifdef F_STORAGE
extern void save_data(uint8_t *data, uint32_t data_len, uint16_t block_number);
#endif

/* Global Variable ----------------------------------------------*/
static int g_tftp_socket = -1;

static uint8_t g_filename[FILE_NAME_SIZE];

static uint32_t g_server_ip   = 0;
static uint16_t g_server_port = 0;
static uint16_t g_local_port  = 0;

static uint32_t g_tftp_state = STATE_NONE;
static uint16_t g_block_num  = 0;

static uint32_t g_timeout      = 5;
static uint32_t g_resend_flag  = 0;
static uint32_t tftp_time_cnt  = 0;
static uint32_t tftp_retry_cnt = 0;

static uint8_t *g_tftp_rcv_buf = NULL;

static TFTP_OPTION default_tftp_opt = {
    .code  = (uint8_t *)"timeout",
    .value = (uint8_t *)"5"};

uint8_t g_progress_state = TFTP_PROGRESS;

#ifdef __TFTP_DEBUG__
int dbg_level = (INFO_DBG | ERROR_DBG | IPC_DBG);
#endif

/* static function define ---------------------------------------*/
static void set_filename(uint8_t *file, uint32_t file_size)
{
    memcpy(g_filename, file, file_size);
}

static inline void set_server_ip(uint32_t ipaddr)
{
    g_server_ip = ipaddr;
}

static inline uint32_t get_server_ip(void)
{
    return g_server_ip;
}

static inline void set_server_port(uint16_t port)
{
    g_server_port = port;
}

static inline uint16_t get_server_port(void)
{
    return g_server_port;
}

static inline void set_local_port(uint16_t port)
{
    g_local_port = port;
}

static inline uint16_t get_local_port(void)
{
    return g_local_port;
}

static inline uint16_t genernate_port(void)
{
    /* TODO */
    return 0;
}

static inline void set_tftp_state(uint32_t state)
{
    g_tftp_state = state;
}

static inline uint32_t get_tftp_state(void)
{
    return g_tftp_state;
}

static inline void set_tftp_timeout(uint32_t timeout)
{
    g_timeout = timeout;
}

static inline uint32_t get_tftp_timeout(void)
{
    return g_timeout;
}

static inline void set_block_number(uint16_t block_number)
{
    g_block_num = block_number;
}

static inline uint16_t get_block_number(void)
{
    return g_block_num;
}

/**
 * @brief Open the TFTP socket
 *
 * Create a UDP socket on the given socket and return the socket descriptor.
 *
 * @param sock Socket type
 *
 * @return Returns a socket descriptor for success and -1 for failure
 */
static int open_tftp_socket(uint8_t sock)
{
    uint8_t sd, sck_state;

    sd = socket(sock, Sn_MR_UDP, 51000, SF_IO_NONBLOCK);
    if (sd != sock)
    {
        // DBG_PRINT(ERROR_DBG, "[%s] socket error\r\n", __func__);
        return -1;
    }

    do
    {
        getsockopt(sd, SO_STATUS, &sck_state);
    } while (sck_state != SOCK_UDP);

    return sd;
}

/**
 * @brief Send UDP packets
 *
 * UDP packets are sent through the specified socket to the specified IP address and port.
 *
 * @param socket Socket descriptors
 * @param packet Packet pointer
 * @param len Packet length
 * @param ip Destination IP address
 * @param port Destination port number
 *
 * @return Returns the number of bytes sent for success and -1 for failure
 */
static int send_udp_packet(int socket, uint8_t *packet, uint32_t len, uint32_t ip, uint16_t port)
{
    int snd_len;

    ip = htonl(ip);

    snd_len = sendto(socket, packet, len, (uint8_t *)&ip, port);
    if (snd_len != len)
    {
        // DBG_PRINT(ERROR_DBG, "[%s] sendto error\r\n", __func__);
        return -1;
    }

    return snd_len;
}

/**
 * @brief Receive UDP packets
 *
 * Packets are received from the specified UDP socket and the received data is saved to the specified buffer.
 *
 * @param socket UDP socket descriptor
 * @param packet A pointer to the buffer that receives the data
 * @param len Buffer size
 * @param ip A pointer to the source IP address where the source IP address is stored
 * @param port A pointer to store the source port number
 *
 * @return The length of the received data is returned if successful, and -1 is returned if it fails
 */
static int recv_udp_packet(int socket, uint8_t *packet, uint32_t len, uint32_t *ip, uint16_t *port)
{
    int     ret;
    uint8_t sck_state;
    int     recv_len;

    /* Receive Packet Process */
    ret = getsockopt(socket, SO_STATUS, &sck_state);
    if (ret != SOCK_OK)
    {
        // DBG_PRINT(ERROR_DBG, "[%s] getsockopt SO_STATUS error\r\n", __func__);
        return -1;
    }

    if (sck_state == SOCK_UDP)
    {
        ret = getsockopt(socket, SO_RECVBUF, &recv_len);
        if (ret != SOCK_OK)
        {
            // DBG_PRINT(ERROR_DBG, "[%s] getsockopt SO_RECVBUF error\r\n", __func__);
            return -1;
        }

        if (recv_len)
        {
            recv_len = recvfrom(socket, packet, len, (uint8_t *)ip, port);
            if (recv_len < 0)
            {
                // DBG_PRINT(ERROR_DBG, "[%s] recvfrom error\r\n", __func__);
                return -1;
            }

            *ip = ntohl(*ip);

            return recv_len;
        }
    }
    return -1;
}

/**
 * @brief Close the TFTP socket
 *
 * Close the specified TFTP socket to release related resources.
 *
 * @param socket Socket descriptors
 */
static void close_tftp_socket(int socket)
{
    close(socket);
}

/**
 * @brief Initialize the TFTP module
 *
 * Initialize the parameters and status of the TFTP module.
 */
static void init_tftp(void)
{
    g_filename[0] = 0;

    set_server_ip(0);
    set_server_port(0);
    set_local_port(0);

    set_tftp_state(STATE_NONE);
    set_block_number(0);

    // timeout flag
    g_resend_flag  = 0;
    tftp_retry_cnt = tftp_time_cnt = 0;

    g_progress_state = TFTP_PROGRESS;
}

/**
 * @brief Cancel the timeout
 *
 * Cancel the timeout operation of Teftep (Triviaalfeldtrantransverprotocol).
 * If the resend flag is true, set it to false and reset the retry counter and time counter.
 */
static void tftp_cancel_timeout(void)
{
    if (g_resend_flag)
    {
        g_resend_flag  = 0;
        tftp_retry_cnt = tftp_time_cnt = 0;
    }
}

/**
 * @brief Register a timeout handler
 *
 * When a timeout occurs during a TFTP transfer, this function is called for timeout handling.
 * If the g_resend_flag flag is 0, it is set to 1 and the tftp_retry_cnt and tftp_time_cnt are reset to 0.
 */
static void tftp_reg_timeout(void)
{
    if (g_resend_flag == 0)
    {
        g_resend_flag  = 1;
        tftp_retry_cnt = tftp_time_cnt = 0;
    }
}

static void process_tftp_option(uint8_t *msg, uint32_t msg_len)
{
    /* TODO Option Process */
}

/**
 * @brief Send a TFTP RRQ request
 *
 * Send a TFTP RRQ request to the specified server IP address and port.
 *
 * @param filename filename
 * @param mode Transmission mode
 * @param opt TFTP option
 * @param opt_len TFTP option length
 */
static void send_tftp_rrq(uint8_t *filename, uint8_t *mode, TFTP_OPTION *opt, uint8_t opt_len)
{
    uint8_t  snd_buf[MAX_MTU_SIZE];
    uint8_t *pkt = snd_buf;
    uint32_t i, len;

    *((uint16_t *)pkt)  = htons(TFTP_RRQ);
    pkt                += 2;
    strcpy((char *)pkt, (const char *)filename);
    pkt += strlen((char *)filename) + 1;
    strcpy((char *)pkt, (const char *)mode);
    pkt += strlen((char *)mode) + 1;

    for (i = 0; i < opt_len; i++)
    {
        strcpy((char *)pkt, (const char *)opt[i].code);
        pkt += strlen((char *)opt[i].code) + 1;
        strcpy((char *)pkt, (const char *)opt[i].value);
        pkt += strlen((char *)opt[i].value) + 1;
    }

    len = pkt - snd_buf;

    send_udp_packet(g_tftp_socket, snd_buf, len, get_server_ip(), TFTP_SERVER_PORT);
    set_tftp_state(STATE_RRQ);
    set_filename(filename, strlen((char *)filename) + 1);
    tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
    DBG_PRINT(IPC_DBG, ">> TFTP RRQ : FileName(%s), Mode(%s)\r\n", filename, mode);
#endif
}

#if 0 // 2014.07.01 sskim
static void send_tftp_wrq(uint8_t *filename, uint8_t *mode, TFTP_OPTION *opt, uint8_t opt_len)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t i, len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_WRQ);
	pkt += 2;
	strcpy((char *)pkt, (const char *)filename);
	pkt += strlen((char *)filename) + 1;
	strcpy((char *)pkt, (const char *)mode);
	pkt += strlen((char *)mode) + 1;

	for(i = 0 ; i < opt_len ; i++) {
		strcpy((char *)pkt, (const char *)opt[i].code);
		pkt += strlen((char *)opt[i].code) + 1;
		strcpy((char *)pkt, (const char *)opt[i].value);
		pkt += strlen((char *)opt[i].value) + 1;
	}

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), TFTP_SERVER_PORT);
	set_tftp_state(STATE_WRQ);
	set_filename(filename, strlen((char *)filename) + 1);
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	DBG_PRINT(IPC_DBG, ">> TFTP WRQ : FileName(%s), Mode(%s)\r\n", filename, mode);
#endif
}
#endif

#if 0 // 2014.07.01 sskim
/**
 * @brief Send TFTP data
 *
 * The given data block is sent to the server via the TFTP protocol.
 *
 * @param block_number Block number
 * @param data Data pointers
 * @param data_len The length of the data
 */
static void send_tftp_data(uint16_t block_number, uint8_t *data, uint16_t data_len)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_DATA);
	pkt += 2;
	*((uint16_t *)pkt) = htons(block_number);
	pkt += 2;
	memcpy(pkt, data, data_len);
	pkt += data_len;

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), get_server_port());
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	DBG_PRINT(IPC_DBG, ">> TFTP DATA : Block Number(%d), Data Length(%d)\r\n", block_number, data_len);
#endif
}
#endif

/**
 * @brief Send a TFTP reply packet
 *
 * Send the specified block number as a TFTP reply packet.
 *
 * @param block_number Block number
 */
static void send_tftp_ack(uint16_t block_number)
{
    uint8_t  snd_buf[4];
    uint8_t *pkt = snd_buf;

    *((uint16_t *)pkt)  = htons((uint16_t)TFTP_ACK);
    pkt                += 2;
    *((uint16_t *)pkt)  = htons(block_number);
    pkt                += 2;

    send_udp_packet(g_tftp_socket, snd_buf, 4, get_server_ip(), get_server_port());
    tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
    DBG_PRINT(IPC_DBG, ">> TFTP ACK : Block Number(%d)\r\n", block_number);
#endif
}

#if 0 // 2014.07.01 sskim
/**
 * @brief Send a TFTP OACK packet
 *
 * Send out the given list of TFTP options as OACK packets.
 *
 * @param opt List of TFTP options
 * @param opt_len The length of the picklist
 */
static void send_tftp_oack(TFTP_OPTION *opt, uint8_t opt_len)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t i, len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_OACK);
	pkt += 2;

	for(i = 0 ; i < opt_len ; i++) {
		strcpy((char *)pkt, (const char *)opt[i].code);
		pkt += strlen((char *)opt[i].code) + 1;
		strcpy((char *)pkt, (const char *)opt[i].value);
		pkt += strlen((char *)opt[i].value) + 1;
	}

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), get_server_port());
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	DBG_PRINT(IPC_DBG, ">> TFTP OACK \r\n");
#endif
}
#endif

#if 0 // 2014.07.01 sskim
/**
 * @brief A TFTP error was sent
 *
 * Encapsulate TFTP error messages into packets and send them to the server via UDP.
 *
 * @param error_number Error number
 * @param error_message error message
 */
static void send_tftp_error(uint16_t error_number, uint8_t *error_message)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_ERROR);
	pkt += 2;
	*((uint16_t *)pkt) = htons(error_number);
	pkt += 2;
	strcpy((char *)pkt, (const char *)error_message);
	pkt += strlen((char *)error_message) + 1;

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), get_server_port());
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	DBG_PRINT(IPC_DBG, ">> TFTP ERROR : Error Number(%d)\r\n", error_number);
#endif
}
#endif

/**
 * @brief Receive a TFTP RRQ request
 *
 * In TFTP server mode, an RRQ (Read Request) request is received from the client.
 *
 * @param msg Received message buffers
 * @param msg_len Message length
 */
static void recv_tftp_rrq(uint8_t *msg, uint32_t msg_len)
{
    /* When TFTP Server Mode */
}

/**
 * @brief Receive a TFTP WRQ request
 *
 * When the system is in TFTP server mode, it receives TFTP WRQ requests and processes them accordingly.
 *
 * @param msg TFTP messages received
 * @param msg_len Message length
 */
static void recv_tftp_wrq(uint8_t *msg, uint32_t msg_len)
{
    /* When TFTP Server Mode */
}

/**
 * @brief Receive TFTP data
 *
 * The received TFTP data is processed.
 *
 * @param msg TFTP data message pointer
 * @param msg_len TFTP data message length
 */
static void recv_tftp_data(uint8_t *msg, uint32_t msg_len)
{
    TFTP_DATA_T *data = (TFTP_DATA_T *)msg;

    data->opcode    = ntohs(data->opcode);
    data->block_num = ntohs(data->block_num);
#ifdef __TFTP_DEBUG__
    DBG_PRINT(IPC_DBG, "<< TFTP_DATA : opcode(%d), block_num(%d)\r\n", data->opcode, data->block_num);
#endif
    switch (get_tftp_state())
    {
    case STATE_RRQ:
    case STATE_OACK:
        if (data->block_num == 1)
        {
            set_tftp_state(STATE_DATA);
            set_block_number(data->block_num);
#ifdef F_STORAGE
            save_data(data->data, msg_len - 4, data->block_num);
#endif
            tftp_cancel_timeout();
        }
        send_tftp_ack(data->block_num);
        printf("recv data[%d]:%s\r\n", msg_len - 4, data->data);
        if ((msg_len - 4) < TFTP_BLK_SIZE)
        {
            init_tftp();
            g_progress_state = TFTP_SUCCESS;
        }

        break;

    case STATE_DATA:
        if (data->block_num == (get_block_number() + 1))
        {
            set_block_number(data->block_num);
#ifdef F_STORAGE
            save_data(data->data, msg_len - 4, data->block_num);
#endif
            tftp_cancel_timeout();
        }
        send_tftp_ack(data->block_num);

        if ((msg_len - 4) < TFTP_BLK_SIZE)
        {
            init_tftp();
            g_progress_state = TFTP_SUCCESS;
        }

        break;

    default:
        /* invalid message */
        break;
    }
}

/**
 * @brief Receive a TFTP acknowledgment message
 *
 * Receive a TFTP acknowledgment message and process it based on the current status.
 *
 * @param msg Message pointers
 * @param msg_len Message length
 */
static void recv_tftp_ack(uint8_t *msg, uint32_t msg_len)
{
#ifdef __TFTP_DEBUG__
    DBG_PRINT(IPC_DBG, "<< TFTP_ACK : \r\n");
#endif

    switch (get_tftp_state())
    {
    case STATE_WRQ:
        break;

    case STATE_ACK:
        break;

    default:
        /* invalid message */
        break;
    }
}

/**
 * @brief Receive Teftep Oak messages
 *
 * Processed OACK messages received based on the current TFTP status.
 *
 * @param msg The content of the Teftepook message
 * @param msg_len Deftep Oak message length
 */
static void recv_tftp_oack(uint8_t *msg, uint32_t msg_len)
{
#ifdef __TFTP_DEBUG__
    DBG_PRINT(IPC_DBG, "<< TFTP_OACK : \r\n");
#endif

    switch (get_tftp_state())
    {
    case STATE_RRQ:
        process_tftp_option(msg, msg_len);
        set_tftp_state(STATE_OACK);
        tftp_cancel_timeout();
        send_tftp_ack(0);
        break;

    case STATE_WRQ:
        process_tftp_option(msg, msg_len);
        set_tftp_state(STATE_ACK);
        tftp_cancel_timeout();

        /* TODO DATA Transfer */
        // send_tftp_data(...);
        break;

    default:
        /* invalid message */
        break;
    }
}

/**
 * @brief Receive a TFTP error message
 *
 * Receive TFTP error messages and parse error codes and error messages.
 *
 * @param msg Error message pointer
 * @param msg_len The length of the error message
 */
static void recv_tftp_error(uint8_t *msg, uint32_t msg_len)
{
    TFTP_ERROR_T *data = (TFTP_ERROR_T *)msg;

    data->opcode     = ntohs(data->opcode);
    data->error_code = ntohs(data->error_code);

#ifdef __TFTP_DEBUG__
    DBG_PRINT(IPC_DBG, "<< TFTP_ERROR : %d (%s)\r\n", data->error_code, data->error_msg);
    DBG_PRINT(ERROR_DBG, "[%s] Error Code : %d (%s)\r\n", __func__, data->error_code, data->error_msg);
#endif
    init_tftp();
    g_progress_state = TFTP_FAIL;
}

/**
 * @brief Receive TFTP packets
 *
 * TFTP packets are received from the specified IP address and port, and processed accordingly based on the opcodes in the packets.
 *
 * @param packet Packet pointer
 * @param packet_len Packet length
 * @param from_ip The IP address of the source of the packet
 * @param from_port The packet source port number
 */
static void recv_tftp_packet(uint8_t *packet, uint32_t packet_len, uint32_t from_ip, uint16_t from_port)
{
    uint16_t opcode;

    /* Verify Server IP */
    if (from_ip != get_server_ip())
    {
#ifdef __TFTP_DEBUG__
        DBG_PRINT(ERROR_DBG, "[%s] Server IP faults\r\n", __func__);
        DBG_PRINT(ERROR_DBG, "from IP : %08x, Server IP : %08x\r\n", from_ip, get_server_ip());
#endif
        return;
    }

    opcode = ntohs(*((uint16_t *)packet));

    /* Set Server Port */
    if ((get_tftp_state() == STATE_WRQ) || (get_tftp_state() == STATE_RRQ))
    {
        set_server_port(from_port);
#ifdef __TFTP_DEBUG__
        DBG_PRINT(INFO_DBG, "[%s] Set Server Port : %d\r\n", __func__, from_port);
#endif
    }

    switch (opcode)
    {
    case TFTP_RRQ: /* When Server */
        recv_tftp_rrq(packet, packet_len);
        break;
    case TFTP_WRQ: /* When Server */
        recv_tftp_wrq(packet, packet_len);
        break;
    case TFTP_DATA:
        recv_tftp_data(packet, packet_len);
        break;
    case TFTP_ACK:
        recv_tftp_ack(packet, packet_len);
        break;
    case TFTP_OACK:
        recv_tftp_oack(packet, packet_len);
        break;
    case TFTP_ERROR:
        recv_tftp_error(packet, packet_len);
        break;

    default:
        // Unknown Mesage
        break;
    }
}

/* Functions ----------------------------------------------------*/
/**
 * @brief Initialize TFTP
 *
 * Initialize the TFTP module, open the TFTP socket, and set the receive buffer.
 *
 * @param socket Socket number
 * @param buf Receive buffer pointer
 */
void TFTP_init(uint8_t socket, uint8_t *buf)
{
    init_tftp();

    g_tftp_socket  = open_tftp_socket(socket);
    g_tftp_rcv_buf = buf;
}

/**
 * @brief Exit the TFTP mode
 *
 * This function is used to exit TFTP mode and perform a series of cleanup operations, including initializing TFTP,
 * closing TFTP sockets, resetting TFTP sockets to invalid values, and emptying TFTP receive buffers.
 *
 */
void TFTP_exit(void)
{
    init_tftp();

    close_tftp_socket(g_tftp_socket);
    g_tftp_socket = -1;

    g_tftp_rcv_buf = NULL;
}

/**
 * @brief Run the TFTP protocol
 *
 * Perform operations related to the TFTP protocol, including timeout processing and receiving packet processing.
 *
 * @return Returns the current state of the TFTP protocol
 */
int TFTP_run(void)
{
    int      len;
    uint16_t from_port;
    uint32_t from_ip;

    /* Timeout Process */
    if (g_resend_flag)
    {
        if (tftp_time_cnt >= g_timeout)
        {
            switch (get_tftp_state())
            {
            case STATE_WRQ:
                break;

            case STATE_RRQ:
                send_tftp_rrq(g_filename, (uint8_t *)TRANS_BINARY, &default_tftp_opt, 1);
                break;

            case STATE_OACK:
            case STATE_DATA:
                send_tftp_ack(get_block_number());
                break;

            case STATE_ACK:
                break;

            default:
                break;
            }

            tftp_time_cnt = 0;
            tftp_retry_cnt++;

            if (tftp_retry_cnt >= 5)
            {
                init_tftp();
                g_progress_state = TFTP_FAIL;
            }
        }
    }

    /* Receive Packet Process */
    len = recv_udp_packet(g_tftp_socket, g_tftp_rcv_buf, MAX_MTU_SIZE, &from_ip, &from_port);
    if (len < 0)
    {
#ifdef __TFTP_DEBUG__
        DBG_PRINT(ERROR_DBG, "[%s] recv_udp_packet error\r\n", __func__);
#endif
        return g_progress_state;
    }

    recv_tftp_packet(g_tftp_rcv_buf, len, from_ip, from_port);

    return g_progress_state;
}

/**
 * @brief Read the TFTP request
 *
 * Set the IP address of the TFTP server and send a TFTP read request.
 *
 * @param server_ip The IP address of the TFTP server
 * @param filename File name pointer
 */
void TFTP_read_request(uint32_t server_ip, uint8_t *filename)
{
    set_server_ip(server_ip);
#ifdef __TFTP_DEBUG__
    DBG_PRINT(INFO_DBG, "[%s] Set Tftp Server : %x\r\n", __func__, server_ip);
#endif

    g_progress_state = TFTP_PROGRESS;
    send_tftp_rrq(filename, (uint8_t *)TRANS_BINARY, &default_tftp_opt, 1);
}

/**
 * @brief TFTP timeout handler
 * This function is called to handle when a TFTP transfer timeout occurs.
 *
 * If the global variable g_resend_flag is true, increase the value of the TFTP time counter tftp_time_cnt.
 */
void tftp_timeout_handler(void)
{
    if (g_resend_flag)
        tftp_time_cnt++;
}
