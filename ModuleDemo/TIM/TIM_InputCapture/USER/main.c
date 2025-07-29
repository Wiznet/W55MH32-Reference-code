#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"


USART_TypeDef *USART_TEST = USART1;

void              UART_Configuration(uint32_t bound);
void              TIM_Configuration(void);
void              RCC_ClkConfiguration(void);
RCC_ClocksTypeDef clocks;

uint8_t  TIM3_CAPTURE_STA = 0; //input capture state
uint16_t TIM3_CAPTURE_VAL;     //Enter capture value
int      main(void)
{
    uint32_t temp = 0;
    RCC_ClkConfiguration();
    delay_init();
    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);

    printf("TIM Input Capture Test.\n");

    TIM_Configuration();

    while (1)
    {
        if (TIM3_CAPTURE_STA & 0X80) //Successfully captured a rising edge
        {
            temp  = TIM3_CAPTURE_STA & 0X3F;
            temp *= 65536;                  //sum of overflow times
            temp += TIM3_CAPTURE_VAL;       //Get the total high time
            printf("HIGH:%d us\r\n", temp); //Print total peak time
            TIM3_CAPTURE_STA = 0;           //Initiate the next capture
        }
    }
}

TIM_ICInitTypeDef TIM_ICInitStructure;
void              TIM_Configuration(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                        //Enable Timer 3 Clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); //Enable GPIO peripherals and AFIO multiplexing function module clocks


    //Set this pin as a multiplexed output function to output the PWM pulse waveform of TIM3 CH2
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;    //TIM_CH2
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD; //Multiplexed push-pull output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);         //Initialize GPIO

    //Timer TIM3 initialization
    TIM_TimeBaseStructure.TIM_Period        = 0XFFFF;             //Sets the value of the auto-reload register cycle for the next update event load activity
    TIM_TimeBaseStructure.TIM_Prescaler     = 72 - 1;             //Sets the prescaler value used as a divisor of the TIMx clock frequency
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //Set up clock splitting:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //TIM Up Count Mode
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);               //The time cardinality unit to initialize TIMx according to the specified parameters

    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;    //Rising edge capture
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //Map to TI1
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           //Configure input divider, not divider
    TIM_ICInitStructure.TIM_ICFilter    = 0x00;                     //IC1F=0000 Configure Input Filter No filtering
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    //interrupt packet initialization
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn; //TIM3 interrupt
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;         //Priority level 2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;         //From priority level 0
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;    //The IRQ channel is enabled
    NVIC_Init(&NVIC_InitStructure);                                   //Initialize peripheral NVIC registers according to the parameters specified in the NVIC_InitStruct

    TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC2, ENABLE);           //Allow update interrupts, allow CC2IE to capture interrupts

    TIM_Cmd(TIM3, ENABLE);
}
void RCC_ClkConfiguration(void)
{
    RCC_DeInit();

    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    RCC_PLLCmd(DISABLE);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);

    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    RCC_HSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
}
void UART_Configuration(uint32_t bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = bound;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART_TEST, &USART_InitStructure);
    USART_Cmd(USART_TEST, ENABLE);
}


//Timer 3 interrupt service routine
void TIM3_IRQHandler(void)
{
    if ((TIM3_CAPTURE_STA & 0X80) == 0) //Has not been successfully captured
    {
        if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
        {
            if (TIM3_CAPTURE_STA & 0X40)               //High level has been captured
            {
                if ((TIM3_CAPTURE_STA & 0X3F) == 0X3F) //The high level is too long
                {
                    TIM3_CAPTURE_STA |= 0X80;          //The tag was successfully captured once.
                    TIM3_CAPTURE_VAL  = 0XFFFF;
                }
                else
                    TIM3_CAPTURE_STA++;
            }
        }
        if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)             //Capture 1 A capture event occurred
        {
            if (TIM3_CAPTURE_STA & 0X40)                            //Capture a falling edge
            {
                TIM3_CAPTURE_STA |= 0X80;                           //Flag successfully captured a high pulse width
                TIM3_CAPTURE_VAL  = TIM_GetCapture2(TIM3);
                TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Rising); //Set to rising edge capture
            }
            else                                                    //Not yet started, first capture of rising edge
            {
                TIM3_CAPTURE_STA = 0;                               //empty
                TIM3_CAPTURE_VAL = 0;
                TIM_SetCounter(TIM3, 0);
                TIM3_CAPTURE_STA |= 0X40;                            //Marker captures rising edge
                TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Falling); //Set to falling edge capture
            }
        }
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2 | TIM_IT_Update); //Clear interrupt flag
}

int SER_PutChar(int ch)
{
    while (!USART_GetFlagStatus(USART_TEST, USART_FLAG_TC));
    USART_SendData(USART_TEST, (uint8_t)ch);

    return ch;
}

int fputc(int c, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    if (c == '\n')
    {
        SER_PutChar('\r');
    }
    return (SER_PutChar(c));
}

