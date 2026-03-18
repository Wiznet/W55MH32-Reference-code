/*
 * file: des_alt.c
 * description: mbedtls DES/3DES hardware acceleration callbacks
 * author: Mason
 * company: WIZnet
 * date: 2025.05.19
 */
#include "mbedtls/des.h"
#include "des_alt.h"
#include <string.h>
#include "wiz_des.h"
#include "wiz_rand.h"

void mbedtls_des_init(mbedtls_des_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_des_context));
}

void mbedtls_des_free(mbedtls_des_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_des_context));
}

int mbedtls_des_setkey_enc(mbedtls_des_context *ctx, const unsigned char key[8])
{
    memcpy(ctx->key, key, 8);
    ctx->keybits = 64;
    return 0;
}

int mbedtls_des_setkey_dec(mbedtls_des_context *ctx, const unsigned char key[8])
{
    memcpy(ctx->key, key, 8);
    ctx->keybits = 64;
    return 0;
}

int mbedtls_des_crypt_ecb(mbedtls_des_context *ctx, const unsigned char input[8], unsigned char output[8])
{
    WIZ_SYM_CRYPT_CALL callDes;
    memset(&callDes, 0, sizeof(callDes));
    callDes.pu8In     = (uint8_t*)input;
    callDes.u32InLen  = 8;
    callDes.pu8Out    = output;
    callDes.u32OutLen = 8;
    callDes.pu8Key    = ctx->key;
    callDes.u16Opt    = WIZ_DES_OPT_BLK_ECB |
        ((ctx->mode == MBEDTLS_DES_ENCRYPT) ? WIZ_DES_OPT_MODE_ENCRYPT : WIZ_DES_OPT_MODE_DECRYPT);
    callDes.u32Crc    = WIZCRC_CalcBuff(0xffff, &callDes, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZDES_EncDec(&callDes);
    return 0;
}

int mbedtls_des_crypt_cbc(mbedtls_des_context *ctx, int mode, size_t length,
                          unsigned char iv[8], const unsigned char *input, unsigned char *output)
{
    if (length % 8 != 0)
        return MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH;

    WIZ_SYM_CRYPT_CALL callDes;
    memset(&callDes, 0, sizeof(callDes));
    callDes.pu8In     = (uint8_t*)input;
    callDes.u32InLen  = length;
    callDes.pu8Out    = output;
    callDes.u32OutLen = length;
    callDes.pu8IV     = iv;
    callDes.pu8Key    = ctx->key;
    callDes.u16Opt    = WIZ_DES_OPT_BLK_CBC |
        ((mode == MBEDTLS_DES_ENCRYPT) ? WIZ_DES_OPT_MODE_ENCRYPT : WIZ_DES_OPT_MODE_DECRYPT);
    callDes.u32Crc    = WIZCRC_CalcBuff(0xffff, &callDes, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZDES_EncDec(&callDes);

    // IV update: 반드시 mbedTLS 규격대로
    if (mode == MBEDTLS_DES_ENCRYPT) {
        memcpy(iv, output + length - 8, 8);
    } else {
        memcpy(iv, input + length - 8, 8);
    }
    return 0;
}

/* ---- 3DES (TDES) ---- */

void mbedtls_des3_init(mbedtls_des3_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_des3_context));
}

void mbedtls_des3_free(mbedtls_des3_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_des3_context));
}

int mbedtls_des3_set3key_enc(mbedtls_des3_context *ctx, const unsigned char key[24])
{
    memcpy(ctx->key, key, 24);
    ctx->keybits = 192;
    return 0;
}

int mbedtls_des3_set3key_dec(mbedtls_des3_context *ctx, const unsigned char key[24])
{
    memcpy(ctx->key, key, 24);
    ctx->keybits = 192;
    return 0;
}

int mbedtls_des3_crypt_ecb(mbedtls_des3_context *ctx, const unsigned char input[8], unsigned char output[8])
{
    WIZ_SYM_CRYPT_CALL callTdes;
    memset(&callTdes, 0, sizeof(callTdes));
    callTdes.pu8In     = (uint8_t*)input;
    callTdes.u32InLen  = 8;
    callTdes.pu8Out    = output;
    callTdes.u32OutLen = 8;
    callTdes.pu8Key    = ctx->key;
    callTdes.u16Opt    = WIZ_DES_OPT_BLK_ECB |
        ((ctx->mode == MBEDTLS_DES_ENCRYPT) ? WIZ_DES_OPT_MODE_ENCRYPT : WIZ_DES_OPT_MODE_DECRYPT) |
        WIZ_TDES_OPT_KEY_3;
    callTdes.u32Crc    = WIZCRC_CalcBuff(0xffff, &callTdes, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZTDES_EncDec(&callTdes);
    return 0;
}

int mbedtls_des3_crypt_cbc(mbedtls_des3_context *ctx, int mode, size_t length,
    unsigned char iv[8], const unsigned char *input, unsigned char *output)
{
    if (length % 8 != 0)
        return MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH;

    WIZ_SYM_CRYPT_CALL callTdes;
    memset(&callTdes, 0, sizeof(WIZ_SYM_CRYPT_CALL));
    callTdes.pu8In     = (uint8_t*)input;
    callTdes.u32InLen  = length;
    callTdes.pu8Out    = output;
    callTdes.u32OutLen = length;
    callTdes.pu8IV     = iv;
    callTdes.pu8Key    = ctx->key;
    callTdes.u16Opt    = WIZ_DES_OPT_BLK_CBC |
        ((mode == MBEDTLS_DES_ENCRYPT) ? WIZ_DES_OPT_MODE_ENCRYPT : WIZ_DES_OPT_MODE_DECRYPT) |
        WIZ_TDES_OPT_KEY_3;
    callTdes.u32Crc    = WIZCRC_CalcBuff(0xffff, &callTdes, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZTDES_EncDec(&callTdes);

    if (mode == MBEDTLS_DES_ENCRYPT) {
        memcpy(iv, output + length - 8, 8);
    } else {
        memcpy(iv, input + length - 8, 8);
    }
    return 0;
}

int mbedtls_des3_set2key_enc(mbedtls_des3_context *ctx, const unsigned char key[16])
{
    // 2-key: K1 | K2 | K1
    memcpy(ctx->key, key, 16);            // K1, K2
    memcpy(ctx->key + 16, key, 8);        // K1 again
    ctx->keybits = 128;
    // 실제 하드웨어 엔진이 3-key로만 동작하면, 아래 3-key set 호출해도 무방
    return mbedtls_des3_set3key_enc(ctx, ctx->key);
}

int mbedtls_des3_set2key_dec(mbedtls_des3_context *ctx, const unsigned char key[16])
{
    // 2-key: K1 | K2 | K1
    memcpy(ctx->key, key, 16);            // K1, K2
    memcpy(ctx->key + 16, key, 8);        // K1 again
    ctx->keybits = 128;
    return mbedtls_des3_set3key_dec(ctx, ctx->key);
}
