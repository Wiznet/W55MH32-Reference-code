#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tpad.h"
#include "delay.h"

#define TPAD_ARR_MAX_VAL 0XFFFF //The maximum ARR value
#ifdef PIN48_BOARD
#define TIM_INPUT_CHANNEL TIM_Channel_3
#define TIM_INPUT_STATE   TIM_IT_CC3
#endif

#ifdef PIN64_BOARD
#define TIM_INPUT_CHANNEL TIM_Channel_2
#define TIM_INPUT_STATE   TIM_IT_CC2
#endif

vu16 tpad_default_val = 0; //When no load (no hand press), the time required by the counter


//Initialize the touch button
//Get the value of the touch button when it is empty.
//Return value: 0, initialization successful; 1, initialization failed
u8 TPAD_Init(u8 psc)
{
    u16 buf[10];
    u16 temp;
    u8  j, i;
    TIM_Cap_Init(TPAD_ARR_MAX_VAL, psc - 1); //Counting at a frequency of 1 MHz
    for (i = 0; i < 10; i++)                 //Read 10 times in a row
    {
        buf[i] = TPAD_Get_Val();
        delay_ms(10);
    }
    for (i = 0; i < 9; i++) //sort
    {
        for (j = i + 1; j < 10; j++)
        {
            if (buf[i] > buf[j]) //ascending order
            {
                temp   = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    temp = 0;
    for (i = 2; i < 8; i++) temp += buf[i]; //Take the middle 6 data and average it
    tpad_default_val = temp / 6;
    printf("tpad_default_val:%d\r\n", tpad_default_val);
    if (tpad_default_val > TPAD_ARR_MAX_VAL / 2) return 1; //Initialization encountered more than TPAD_ARR_MAX_VAL/2 value, not normal!
    return 0;
}
//Reset once
void TPAD_Reset(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //Enable PA port clock

#ifdef PIN48_BOARD
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
#endif

#ifdef PIN64_BOARD
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
#endif

    delay_ms(5);

    TIM_SetCounter(TIM2, 0);
    TIM_ClearITPendingBit(TIM2, TIM_INPUT_STATE | TIM_IT_Update); //Clear interrupt flag

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//get the timer capture value
//If it expires, the timer's count value is returned directly.
u16 TPAD_Get_Val(void)
{
    TPAD_Reset();
    while (TIM_GetFlagStatus(TIM2, TIM_INPUT_STATE) == RESET)                           //Waiting to capture the rising edge
    {
        if (TIM_GetCounter(TIM2) > TPAD_ARR_MAX_VAL - 500) return TIM_GetCounter(TIM2); //Timed out, directly return the value of CNT
    };
#ifdef PIN48_BOARD
    return TIM_GetCapture3(TIM2);
#endif
#ifdef PIN64_BOARD
    return TIM_GetCapture2(TIM2);
#endif
}
//read n times, take the maximum value
//n: the number of consecutive fetches
//Return value: the maximum read value read in n readings
u16 TPAD_Get_MaxVal(u8 n)
{
    u16 temp = 0;
    u16 res  = 0;
    while (n--)
    {
        temp = TPAD_Get_Val(); //Get a value once
        if (temp > res) res = temp;
    };
    return res;
}
//Scan the touch button
//mode: 0, does not support continuous triggering (must be released to press once); 1, supports continuous triggering (can be pressed all the time)
//Return value: 0, not pressed; 1, pressed;
#define TPAD_GATE_VAL 100 //Touch threshold value, that is, must be greater than tpad_default_val + TPAD_GATE_VAL, to be considered effective touch.
u8 TPAD_Scan(u8 mode)
{
    static u8 keyen  = 0; //0, you can start testing; > 0, you can't start testing yet
    u8        res    = 0;
    u8        sample = 3; //The default number of samples is 3
    u16       rval;
    if (mode)
    {
        sample = 6; //When supporting continuous pressing, set the number of samples to 6 times
        keyen  = 0; //Support chain pressing
    }
    rval = TPAD_Get_MaxVal(sample);
    if (rval > (tpad_default_val + TPAD_GATE_VAL)) //Greater than tpad_default_val + TPAD_GATE_VAL, valid
    {
        if (keyen == 0) res = 1;                   //keyen==0, effective
        printf("r:%d\r\n", rval);
        keyen = 3;                                 //It will take at least 3 more times for the button to work.
    }
    if (keyen) keyen--;
    return res;
}

void TIM_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef       TIM_ICInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period        = arr;
    TIM_TimeBaseStructure.TIM_Prescaler     = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ICInitStructure.TIM_Channel     = TIM_INPUT_CHANNEL;
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter    = 0x03;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    TIM_Cmd(TIM2, ENABLE);
}


