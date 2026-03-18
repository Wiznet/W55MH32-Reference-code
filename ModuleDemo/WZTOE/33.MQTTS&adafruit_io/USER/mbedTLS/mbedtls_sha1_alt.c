/*
 * file: mbedtls_sha1_alt.c
 * description: mbedTLS SHA1 HW acceleration using MH2103's WIZSHA1
 * author: Mason
 * company: WIZnet
 * date: 2025.05.19
 */
#include "mbedtls/sha1.h"
#include "sha1_alt.h"
#include <string.h>
#include "wiz_sha.h" // WIZSHA1_Context, WIZSHA1_Starts, etc.

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst, const mbedtls_sha1_context *src)
{
    memcpy(dst, src, sizeof(mbedtls_sha1_context));
}

int mbedtls_sha1_starts_ret(mbedtls_sha1_context *ctx)
{
    WIZSHA1_Starts(&ctx->hw_ctx);
    return 0;
}

int mbedtls_sha1_update_ret(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    WIZSHA1_Update(&ctx->hw_ctx, input, ilen);
    return 0;
}

int mbedtls_sha1_finish_ret(mbedtls_sha1_context *ctx, unsigned char output[20])
{
    WIZSHA1_Finish(&ctx->hw_ctx, output);
    return 0;
}
