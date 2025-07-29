#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include <stdint.h>

void user_led_init(void);
void set_user_led_status(int32_t val);
void get_user_led_status(void *ptr, uint8_t *len);

#endif
