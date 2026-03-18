/*
 * file: aes_alt.h
 * description: mbedtls callback functions
 * author: Mason
 * company: WIZnet
 * data: 2025.05.19
 */
#ifndef MBEDTLS_AES_ALT_H
#define MBEDTLS_AES_ALT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char key[32];
    unsigned int keybits;
    int mode;
} mbedtls_aes_context;

void mbedtls_aes_init(mbedtls_aes_context *ctx);
void mbedtls_aes_free(mbedtls_aes_context *ctx);

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits);
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits);

int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16]);
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output);

#ifdef __cplusplus
}
#endif

#endif // MBEDTLS_AES_ALT_H
