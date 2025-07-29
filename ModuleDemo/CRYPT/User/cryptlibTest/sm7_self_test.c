#include <stdio.h>
#include "wiz_sm7.h"
#include "debug.h"
#include "wiz_rand.h"

void SM7_Func_Test(void)
{
    uint8_t au8Cipher1[8] = {0xCE, 0x3C, 0x08, 0xD4, 0x02, 0xAE, 0x24, 0x7C};
    uint8_t au8Key1[16]   = {0x1F, 0xD3, 0x84, 0xD8, 0x6B, 0x50, 0xBE, 0x01, 0x21, 0x43, 0xD6, 0x16, 0x18, 0x15, 0x19, 0x83};
    uint8_t au8Plain1[8]  = {0xE2, 0x73, 0x2F, 0xB8, 0x1D, 0x7D, 0x7E, 0x51};

    uint8_t au8Cipher2[8] = {0xbe, 0x87, 0x60, 0x5b, 0xe2, 0xc6, 0x1f, 0xfa};
    uint8_t au8Key2[16]   = {0x07, 0xad, 0xe6, 0x58, 0x45, 0x53, 0x62, 0x6a, 0xc8, 0x1b, 0x1b, 0x3d, 0x7f, 0x4f, 0xdf, 0x5d};
    uint8_t au8Plain2[8]  = {0x49, 0x7b, 0x94, 0x5f, 0x68, 0xd8, 0xa0, 0x7d};
    uint8_t au8Iv[8]      = {0xb0, 0xa0, 0x47, 0x56, 0xcb, 0x21, 0x80, 0x71};

    uint8_t            au8mCipher[8];
    uint8_t            au8mPlain[8];
    WIZ_SYM_CRYPT_CALL callSm7;

    memset(&callSm7, 0, sizeof(callSm7));
    //ECB
    callSm7.pu8In     = au8Plain1;
    callSm7.pu8Key    = au8Key1;
    callSm7.pu8Out    = au8mCipher;
    callSm7.u32InLen  = sizeof(au8Plain1);
    callSm7.u32OutLen = sizeof(au8mCipher);
    callSm7.u16Opt    = WIZ_SM7_OPT_BLK_ECB | WIZ_SM7_OPT_MODE_ENCRYPT;
    callSm7.u32Crc    = WIZCRC_CalcBuff(0xffff, &callSm7, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZSM7_EncDec(&callSm7);
    r_printf(!memcmp(au8mCipher, au8Cipher1, sizeof(au8Plain1)), "SM7 ECB ENC Test\n");

    callSm7.pu8In     = au8Cipher1;
    callSm7.pu8Key    = au8Key1;
    callSm7.pu8Out    = au8mPlain;
    callSm7.u32InLen  = sizeof(au8Cipher1);
    callSm7.u32OutLen = sizeof(au8mPlain);
    callSm7.u16Opt    = WIZ_SM7_OPT_BLK_ECB | WIZ_SM7_OPT_MODE_DECRYPT;
    callSm7.u32Crc    = WIZCRC_CalcBuff(0xffff, &callSm7, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZSM7_EncDec(&callSm7);
    r_printf(!memcmp(au8Plain1, au8mPlain, sizeof(au8Plain1)), "SM7 ECB DEC Test\n");

    //CBC
    callSm7.pu8In     = au8Plain2;
    callSm7.pu8IV     = au8Iv;
    callSm7.pu8Key    = au8Key2;
    callSm7.pu8Out    = au8mCipher;
    callSm7.u32InLen  = sizeof(au8Plain2);
    callSm7.u32OutLen = sizeof(au8mCipher);
    callSm7.u16Opt    = WIZ_SM7_OPT_BLK_CBC | WIZ_SM7_OPT_MODE_ENCRYPT;
    callSm7.u32Crc    = WIZCRC_CalcBuff(0xffff, &callSm7, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZSM7_EncDec(&callSm7);
    r_printf(!memcmp(au8mCipher, au8Cipher2, sizeof(au8Plain2)), "SM7 CBC ENC Test\n");

    callSm7.pu8In     = au8Cipher2;
    callSm7.pu8IV     = au8Iv;
    callSm7.pu8Key    = au8Key2;
    callSm7.pu8Out    = au8mPlain;
    callSm7.u32InLen  = sizeof(au8Cipher2);
    callSm7.u32OutLen = sizeof(au8mPlain);
    callSm7.u16Opt    = WIZ_SM7_OPT_BLK_CBC | WIZ_SM7_OPT_MODE_DECRYPT;
    callSm7.u32Crc    = WIZCRC_CalcBuff(0xffff, &callSm7, sizeof(WIZ_SYM_CRYPT_CALL) - 4);
    WIZSM7_EncDec(&callSm7);
    r_printf(!memcmp(au8Plain2, au8mPlain, sizeof(au8Plain2)), "SM7 CBC ENC/DEC Test\n");
}


void SM7_Test()
{
    SM7_Func_Test();
}

