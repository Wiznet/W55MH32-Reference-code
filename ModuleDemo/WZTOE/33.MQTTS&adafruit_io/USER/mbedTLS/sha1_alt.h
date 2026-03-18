/*
 * file: sha1_alt.h
 * description: mbedtls callback functions
 * author: Mason
 * company: WIZnet
 * data: 2025.05.19
 */

#ifndef MBEDTLS_SHA1_ALT_H
#define MBEDTLS_SHA1_ALT_H

#include <stddef.h>
#include <stdint.h>
#include "wiz_sha.h"

#ifdef __cplusplus
extern "C" {
#endif

// 하드웨어 SHA1 Context
typedef struct {
    WIZSHA1_Context hw_ctx;
} mbedtls_sha1_context;

// 함수 프로토타입
void mbedtls_sha1_init(mbedtls_sha1_context *ctx);
void mbedtls_sha1_free(mbedtls_sha1_context *ctx);
void mbedtls_sha1_clone(mbedtls_sha1_context *dst, const mbedtls_sha1_context *src);
int mbedtls_sha1_starts_ret(mbedtls_sha1_context *ctx);
int mbedtls_sha1_update_ret(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen);
int mbedtls_sha1_finish_ret(mbedtls_sha1_context *ctx, unsigned char output[20]);

#ifdef __cplusplus
}
#endif

#endif // MBEDTLS_SHA1_ALT_H
