#ifndef _USER_GPIO_H_
#define _USER_GPIO_H_

#include <stdint.h>
#include "w55mh32.h"

#define LED_PIN     GPIO_Pin_14
#define LED_PORT    GPIOD
#define BEEP_PIN    GPIO_Pin_15
#define BEEP_PORT   GPIOD
#define BUTTON_PIN  GPIO_Pin_5
#define BUTTON_PORT GPIOG

void user_gpio_init(void);
void led_on(void);
void led_off(void);
void beep_on(void);
void beep_off(void);
#endif
