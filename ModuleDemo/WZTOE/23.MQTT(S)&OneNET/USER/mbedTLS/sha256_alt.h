/*
 * file: sha1_alt.h
 * description: mbedtls callback functions
 * author: Mason
 * company: WIZnet
 * data: 2025.05.19
 */

#ifndef MBEDTLS_SHA256_ALT_H
#define MBEDTLS_SHA256_ALT_H

#include <stdint.h>
#include "wiz_sha.h"

typedef struct mbedtls_sha256_context {
    WIZSHA2_Context hw_ctx;
}
mbedtls_sha256_context;

void WIZSHA224_Starts(WIZSHA2_Context *ctx);
void WIZSHA224_Finish(WIZSHA2_Context *ctx, uint8_t *output);

#endif /* MBEDTLS_SHA256_ALT_H */
