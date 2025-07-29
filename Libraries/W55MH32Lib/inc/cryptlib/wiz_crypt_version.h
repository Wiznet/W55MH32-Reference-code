#ifndef __WIZ_CRYPT_VERSION_H
#define __WIZ_CRYPT_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
  * @method wiz_get_version
  * @brief  Get the version number.
  * @param  void
  * @retval The constructed version number in the format:MMNNPP00 (Major, Minor, Patch).
  */
uint32_t wiz_crypt_version(void);

#ifdef __cplusplus
}
#endif

#endif 
