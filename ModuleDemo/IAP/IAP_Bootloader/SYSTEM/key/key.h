#ifndef __KEY_H
#define __KEY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"

#define KEY1_PIN    GPIO_Pin_13
#define KEY1_GROUP  GPIOC
#define KEY1_PERIPH RCC_APB2Periph_GPIOC

#define KEY2_PIN    GPIO_Pin_0
#define KEY2_GROUP  GPIOA
#define KEY2_PERIPH RCC_APB2Periph_GPIOA

#define KEY1 GPIO_ReadInputDataBit(KEY1_GROUP, KEY1_PIN)
#define KEY2 GPIO_ReadInputDataBit(KEY2_GROUP, KEY2_PIN)

#define KEY1_PRES 1 //KEY1 press
#define KEY2_PRES 2 //KEY2 press

void    KEY_Init(void);
uint8_t KEY_Scan(void);

#endif

