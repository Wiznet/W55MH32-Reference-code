/*
 * file: mbedtls_sha256_alt.c
 * description: mbedtls callback functions
 * author: Mason
 * company: WIZnet
 * data: 2025.05.19
 */
#include "mbedtls/sha256.h"
#include "sha256_alt.h"
#include <string.h>
#include <stdio.h>
#include "wiz_sha.h"

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL)
        return;
    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL)
        return;
    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst, const mbedtls_sha256_context *src)
{
    if (dst == NULL || src == NULL)
        return;
    memcpy(dst, src, sizeof(mbedtls_sha256_context));
}

int mbedtls_sha256_starts_ret(mbedtls_sha256_context *ctx, int is224)
{
    if (ctx == NULL)
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;

    if (is224) {
        WIZSHA224_Starts(&ctx->hw_ctx);
    } else {
        WIZSHA256_Starts(&ctx->hw_ctx);
    }
    return 0;
}

int mbedtls_sha256_update_ret(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    WIZSHA256_Update(&ctx->hw_ctx, input, ilen);
    return 0;
}

int mbedtls_sha256_finish_ret(mbedtls_sha256_context *ctx, unsigned char *output)
{
    WIZSHA256_Finish(&ctx->hw_ctx, output);
    return 0;
}

int mbedtls_sha256_ret(const unsigned char *input, size_t ilen, unsigned char *output, int is224)
{
    if (input == NULL || output == NULL)
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;

    if (is224) {
        WIZSHA_Sha(WIZSHA224, input, ilen, output, 28);
    } else {
        WIZSHA_Sha(WIZSHA256, input, ilen, output, 32);
    }
    return 0;
}

int mbedtls_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
    return mbedtls_sha256_update_ret(ctx, data, 64);
}

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
    return mbedtls_sha256_starts_ret(ctx, is224);
}
int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    return mbedtls_sha256_update_ret(ctx, input, ilen);
}
int mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char *output)
{
    return mbedtls_sha256_finish_ret(ctx, output);
}

void WIZSHA224_Starts(WIZSHA2_Context *ctx) {
    WIZSHA256_Starts(ctx);
}
void WIZSHA224_Finish(WIZSHA2_Context *ctx, uint8_t *output) {
    WIZSHA256_Finish(ctx, output);
}

