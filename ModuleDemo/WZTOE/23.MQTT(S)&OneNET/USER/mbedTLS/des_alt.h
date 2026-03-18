/*
 * file: des_alt.h
 * description: mbedtls callback functions
 * author: Mason
 * company: WIZnet
 * data: 2025.05.19
 */
#ifndef MBEDTLS_DES_ALT_H
#define MBEDTLS_DES_ALT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// DES context
typedef struct {
    unsigned char key[8];
    unsigned int keybits;
    int mode; // MBEDTLS_DES_ENCRYPT or MBEDTLS_DES_DECRYPT
} mbedtls_des_context;

// 3DES context
typedef struct {
    unsigned char key[24];
    unsigned int keybits;
    int mode;
} mbedtls_des3_context;

// DES API
void mbedtls_des_init(mbedtls_des_context *ctx);
void mbedtls_des_free(mbedtls_des_context *ctx);
int mbedtls_des_setkey_enc(mbedtls_des_context *ctx, const unsigned char key[8]);
int mbedtls_des_setkey_dec(mbedtls_des_context *ctx, const unsigned char key[8]);
int mbedtls_des_crypt_ecb(mbedtls_des_context *ctx, const unsigned char input[8], unsigned char output[8] );
int mbedtls_des_crypt_cbc(mbedtls_des_context *ctx, int mode, size_t length, unsigned char iv[8], const unsigned char *input, unsigned char *output );


// 3DES API
void mbedtls_des3_init(mbedtls_des3_context *ctx);
void mbedtls_des3_free(mbedtls_des3_context *ctx);
int mbedtls_des3_set3key_enc(mbedtls_des3_context *ctx, const unsigned char key[24]);
int mbedtls_des3_set3key_dec(mbedtls_des3_context *ctx, const unsigned char key[24]);
int mbedtls_des3_crypt_ecb(mbedtls_des3_context *ctx, const unsigned char input[8], unsigned char output[8] );
int mbedtls_des3_crypt_cbc(mbedtls_des3_context *ctx, int mode, size_t length, unsigned char iv[8], const unsigned char *input, unsigned char *output );

int mbedtls_des3_set2key_enc(mbedtls_des3_context *ctx, const unsigned char key[16]);
int mbedtls_des3_set2key_dec(mbedtls_des3_context *ctx, const unsigned char key[16]);

#ifdef __cplusplus
}
#endif

#endif // MBEDTLS_DES_ALT_H
