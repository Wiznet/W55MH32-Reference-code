#include <string.h>
#include <stdio.h>
#include "w55mh32.h"
#include "wiz_sha.h"

void SHA_Update_Test()
{
    uint16_t        i;
    WIZSHA1_Context ctx;
    WIZSHA2_Context ctx2;
    uint8_t         buf0[0x40];
    uint8_t         buf1[0x40];
    uint8_t         t0[2]        = "12";
    unsigned char   TestSHA[100] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
    uint8_t         digest[32]   = {0};
    printf("Test the sha function...\r\n");
    WIZSHA1_Starts(&ctx);
    WIZSHA1_Update(&ctx, TestSHA, 100);
    WIZSHA1_Finish(&ctx, digest);
    printf("The sha_160 result is \n");
    for (i = 0; i < 20; i++)
    {
        printf("0x%2x\t", digest[i]);
    }

    memset(digest, 0, 32);
    memset(buf0, 'a', 0x40);
    memset(buf1, 'b', 0x40);
    memset(digest, 0, 32);
    WIZSHA256_Starts(&ctx2);
    WIZSHA256_Update(&ctx2, buf0, 0x40);
    WIZSHA256_Update(&ctx2, buf1, 0x40);
    WIZSHA256_Update(&ctx2, buf1, 0);
    WIZSHA256_Finish(&ctx2, digest);
    printf("The sha_256 result is \n");
    for (i = 0; i < 32; i++)
    {
        printf("0x%2x\t", digest[i]);
    }
}


void SHA_Func_Test()
{
    uint8_t TestSHA[100]    = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
    uint8_t Cipher_SHA256[] = {0x9c, 0xfe, 0x7f, 0xaf, 0xf7, 0x05, 0x42, 0x98, 0xca, 0x87, 0x55, 0x7e, 0x15, 0xa1, 0x02, 0x62,
                               0xde, 0x8d, 0x3e, 0xee, 0x77, 0x82, 0x74, 0x17, 0xfb, 0xdf, 0xea, 0x1c, 0x41, 0xb9, 0xec, 0x23};
    uint8_t Cipher_SHA1[]   = {0x29, 0xb0, 0xe7, 0x87, 0x82, 0x71, 0x64, 0x5f, 0xff, 0xb7, 0xee, 0xc7, 0xdb, 0x4a, 0x74, 0x73, 0xa1, 0xc0, 0x0b, 0xc1};
    uint8_t Cipher_SHA224[] = {0x2C, 0x09, 0xD5, 0x93, 0x30, 0x73, 0x6C, 0x5D, 0x53, 0x19, 0x0B, 0x36, 0x7D, 0x2D, 0x91, 0xCE,
                               0x54, 0xA5, 0x4A, 0xD9, 0x59, 0xA1, 0x6D, 0x40, 0x01, 0x83, 0xF0, 0x7F};
    uint8_t au8Cipher[32]   = {0};
    //Sha_256 test
    memset(au8Cipher, 0, 32);
    WIZSHA_Sha(WIZSHA256, TestSHA, sizeof(TestSHA), au8Cipher, 32);
    r_printf((0 == memcmp(Cipher_SHA256, au8Cipher, 32)), "wiz_sha_256 test\n");
    //Sha_160 test
    memset(au8Cipher, 0, 32);
    WIZSHA_Sha(WIZSHA1, TestSHA, sizeof(TestSHA), au8Cipher, 20);
    r_printf((0 == memcmp(Cipher_SHA1, au8Cipher, 20)), "wiz_sha_160 test\n");
    //Sha_160 test
    memset(au8Cipher, 0, 32);
    WIZSHA_Sha(WIZSHA224, TestSHA, sizeof(TestSHA), au8Cipher, 28);
    r_printf((0 == memcmp(Cipher_SHA224, au8Cipher, 28)), "wiz_sha_224 test\n");
}

void SHA_Test()
{
    SHA_Update_Test();
    SHA_Func_Test();
}
