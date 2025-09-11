#include "debug.h"
#include "w55mh32.h"
#include "wiz_sm3.h"
#include <stdio.h>
#include <string.h>


#define c_sm3_len 16
#define c_sm3_msg "0102030405060708090a0b0c0d0e0f10"
#define c_sm3_hash                                                             \
  "25C9D66ED547714E981BC825B969CC71ACD338F90A4E0F75E789EF71C0921666"

void SM3_Up_Test() {
  uint16_t i;
  WIZSM3_Context Ctx;

  uint8_t buf0[64];
  uint8_t digest[32] = {0};
  printf("Test SM3 function...\r\n");
  memset(buf0, 'a', sizeof(buf0));
  memset(digest, 0, 32);
  WIZSM3_Starts(&Ctx);
  WIZSM3_Update(&Ctx, buf0, sizeof(buf0));
  WIZSM3_Update(&Ctx, buf0, sizeof(buf0));
  WIZSM3_Finish(&Ctx, digest);
  ouputRes("SM3 result: ", digest, 32);
}

void SM3_Func_Test(void) {
  uint8_t Message[16] = {0};
  uint8_t Hash[32] = {0};
  uint8_t _Hash[32] = {0};

  bn_read_string_to_bytes(Message, sizeof(Message), c_sm3_msg);
  bn_read_string_to_bytes(_Hash, sizeof(_Hash), c_sm3_hash);

  WIZSM3_Cal(Hash, Message, c_sm3_len);

  r_printf(!memcmp(Hash, _Hash, sizeof(_Hash)), "SM3 Test\n");
}

void SM3_Test(void) {
  SM3_Up_Test();
  SM3_Func_Test();
}
