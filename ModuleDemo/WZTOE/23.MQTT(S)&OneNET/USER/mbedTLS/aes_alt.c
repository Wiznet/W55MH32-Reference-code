/*
 * file: aes_alt.c
 * description: mbedtls callback functions
 * author: Mason
 * company: WIZnet
 * data: 2025.05.19
 */
#include "mbedtls/aes.h"
#include "aes_alt.h"
#include <string.h>
#include "wiz_aes.h"
#include "wiz_rand.h"

void mbedtls_aes_init(mbedtls_aes_context *ctx) {
    memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx) {
    memset(ctx, 0, sizeof(mbedtls_aes_context));
}

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits) {
    memcpy(ctx->key, key, keybits / 8);
    ctx->keybits = keybits;
    ctx->mode = MBEDTLS_AES_ENCRYPT;
    return 0;
}

int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits) {
    memcpy(ctx->key, key, keybits / 8);
    ctx->keybits = keybits;
    ctx->mode = MBEDTLS_AES_DECRYPT;
    return 0;
}

int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16]) {
    WIZ_SYM_CRYPT_CALL callAes;
    memset(&callAes, 0, sizeof(WIZ_SYM_CRYPT_CALL));

    callAes.pu8In     = (uint8_t*)input;
    callAes.u32InLen  = 16;
    callAes.pu8Out    = output;
    callAes.u32OutLen = 16;
    callAes.pu8Key    = ctx->key;
    callAes.u16Opt    = WIZ_AES_OPT_BLK_ECB | ((mode == MBEDTLS_AES_ENCRYPT) ? WIZ_AES_OPT_MODE_ENCRYPT : WIZ_AES_OPT_MODE_DECRYPT);

    switch (ctx->keybits) {
        case 128: callAes.u16Opt |= WIZ_AES_OPT_KEY_128; break;
        case 192: callAes.u16Opt |= WIZ_AES_OPT_KEY_192; break;
        case 256: callAes.u16Opt |= WIZ_AES_OPT_KEY_256; break;
        default: return -1;
    }
    callAes.u32Crc = WIZCRC_CalcBuff(0xffff, &callAes, sizeof(WIZ_SYM_CRYPT_CALL) - 4);

    WIZAES_EncDec(&callAes);
    return 0;
}

int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx, int mode, size_t length,
                          unsigned char iv[16], const unsigned char *input, unsigned char *output) {
    if (length % 16 != 0)
        return -1; // Invalid length

    WIZ_SYM_CRYPT_CALL callAes;
    memset(&callAes, 0, sizeof(WIZ_SYM_CRYPT_CALL));

    callAes.pu8In     = (uint8_t*)input;
    callAes.u32InLen  = length;
    callAes.pu8Out    = output;
    callAes.u32OutLen = length;
    callAes.pu8IV     = iv;
    callAes.pu8Key    = ctx->key;
    callAes.u16Opt    = WIZ_AES_OPT_BLK_CBC | ((mode == MBEDTLS_AES_ENCRYPT) ? WIZ_AES_OPT_MODE_ENCRYPT : WIZ_AES_OPT_MODE_DECRYPT);

    switch (ctx->keybits) {
        case 128: callAes.u16Opt |= WIZ_AES_OPT_KEY_128; break;
        case 192: callAes.u16Opt |= WIZ_AES_OPT_KEY_192; break;
        case 256: callAes.u16Opt |= WIZ_AES_OPT_KEY_256; break;
        default: return -1;
    }
    callAes.u32Crc = WIZCRC_CalcBuff(0xffff, &callAes, sizeof(WIZ_SYM_CRYPT_CALL) - 4);

    WIZAES_EncDec(&callAes);

    // mbedTLS expects IV to be updated
    if (mode == MBEDTLS_AES_ENCRYPT) {
        memcpy(iv, output + length - 16, 16);
    } else {
        memcpy(iv, input + length - 16, 16);
    }
    return 0;
}
