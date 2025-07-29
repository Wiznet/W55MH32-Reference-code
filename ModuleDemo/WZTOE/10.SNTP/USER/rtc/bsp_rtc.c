#include <stdio.h>
#include "bsp_rtc.h"
#include "w55mh32.h"
#include "delay.h"


_calendar_obj calendar; //时钟结构体

uint32_t timecount = 0;

/**
 * @brief 配置RTC中断的NVIC参数
 *
 * 此函数用于配置RTC中断的NVIC参数，包括中断通道、优先级和使能标志。
 */
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = RTC_IRQn; //RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;        //先占优先级1位,从优先级3位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;        //先占优先级0位,从优先级4位
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;   //使能该通道中断
    NVIC_Init(&NVIC_InitStructure);                                  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}


/**
 * @brief 初始化RTC时钟
 *
 * 该函数用于初始化RTC时钟，包括配置外部低速晶振(LSE)、使能RTC时钟、设置RTC预分频值、设置初始时间等。
 *
 * @return uint8_t 如果初始化成功返回0，如果初始化失败返回1。
 */
uint8_t RTC_Init(void)
{
    uint8_t temp = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);

    RCC_LSEConfig(RCC_LSE_ON);                                        //设置外部低速晶振(LSE),使用外设低速晶振
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 250) //检查指定的RCC标志位设置与否,等待低速晶振就绪
    {
        temp++;
        delay_ms(10);
    }
    if (temp >= 250) return 1;              //初始化时钟失败,晶振有问题
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
    RCC_RTCCLKCmd(ENABLE);                  //使能RTC时钟
    RTC_WaitForLastTask();                  //等待最近一次对RTC寄存器的写操作完成
    RTC_WaitForSynchro();                   //等待RTC寄存器同步
    RTC_ITConfig(RTC_IT_SEC, ENABLE);       //使能RTC秒中断
    RTC_WaitForLastTask();                  //等待最近一次对RTC寄存器的写操作完成
    RTC_EnterConfigMode();                  // 允许配置
    RTC_SetPrescaler(32767);                //设置RTC预分频的值，计算方式32768/(32767+1) = 1Hz 周期刚好是1秒。
    RTC_WaitForLastTask();                  //等待最近一次对RTC寄存器的写操作完成
    RTC_Set(1900, 1, 1, 0, 0, 1);           //设置时间
    RTC_ExitConfigMode();                   //退出配置模式

    RTC_WaitForSynchro();                   //等待最近一次对RTC寄存器的写操作完成
    RTC_ITConfig(RTC_IT_SEC, ENABLE);       //使能RTC秒中断
    RTC_WaitForLastTask();                  //等待最近一次对RTC寄存器的写操作完成
    RTC_NVIC_Config();                      //RCT中断分组设置
    RTC_Get();                              //更新时间
    return 0;                               //ok
}

/**
 * @brief RTC中断处理函数
 *
 * 当RTC（实时时钟）产生中断时，该函数被调用。处理秒钟中断和闹钟中断。
 *
 * @return 无返回值
 */
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)                                                                                                    //秒钟中断
    {
        RTC_Get();                                                                                                                               //更新时间
    }
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)                                                                                                    //闹钟中断
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);                                                                                                       //清闹钟中断
        RTC_Get();                                                                                                                               //更新时间
        printf("Alarm Time:%d-%d-%d %d:%d:%d\n", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec); //输出闹铃时间
    }
    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);                                                                                               //清闹钟中断
    RTC_WaitForLastTask();
}

/**
 * @brief 判断某一年是否为闰年
 *
 * 根据公历年份判断该年份是否为闰年。
 *
 * @param year 要判断的年份
 * @return 如果该年份为闰年，则返回1；否则返回0
 */
uint8_t Is_Leap_Year(uint16_t year)
{
    if (year % 4 == 0) //必须能被4整除
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0)
                return 1; //如果以00结尾,还要能被400整除
            else
                return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}

uint8_t const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};             //月修正数据表
const uint8_t mon_table[12]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //平年的月份日期表
/**
 * @brief 设置RTC时间
 *
 * 设置RTC的时间，包括年、月、日、时、分、秒。
 *
 * @param syear  年份，范围在1970到2099之间
 * @param smon   月份，范围在1到12之间
 * @param sday   日期，范围在1到31之间
 * @param hour   小时，范围在0到23之间
 * @param min    分钟，范围在0到59之间
 * @param sec    秒，范围在0到59之间
 *
 * @return 返回值
 *         - 0: 设置成功
 *         - 1: 年份超出范围（1970-2099）
 */
uint8_t RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    uint32_t seccount = 0;
    if (syear < 1970 || syear > 2099) return 1;
    for (t = 1970; t < syear; t++) //把所有年份的秒钟相加
    {
        if (Is_Leap_Year(t))
            seccount += 31622400; //闰年的秒钟数
        else
            seccount += 31536000; //平年的秒钟数
    }
    smon -= 1;
    for (t = 0; t < smon; t++)                                               //把前面月份的秒钟数相加
    {
        seccount += (uint32_t)mon_table[t] * 86400;                          //月份秒钟数相加
        if (Is_Leap_Year(syear) && t == 1) seccount += 86400;                //闰年2月份增加一天的秒钟数
    }
    seccount += (uint32_t)(sday - 1) * 86400;                                //把前面日期的秒钟数相加
    seccount += (uint32_t)hour * 3600;                                       //小时秒钟数
    seccount += (uint32_t)min * 60;                                          //分钟秒钟数
    seccount += sec;                                                         //最后的秒钟加上去

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);                                             //使能RTC和后备寄存器访问
    RTC_SetCounter(seccount);                                                //设置RTC计数器的值

    RTC_WaitForLastTask();                                                   //等待最近一次对RTC寄存器的写操作完成
    return 0;
}


/**
 * @brief 设置RTC闹钟时间
 *
 * 根据给定的年、月、日、时、分、秒设置RTC的闹钟时间。
 *
 * @param syear 年份，取值范围1970-2099
 * @param smon  月份，取值范围1-12
 * @param sday  日期，取值范围1-31
 * @param hour  小时，取值范围0-23
 * @param min   分钟，取值范围0-59
 * @param sec   秒，取值范围0-59
 *
 * @return 返回值0表示设置成功，返回值1表示年份参数无效
 */
uint8_t RTC_Alarm_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    uint32_t seccount = 0;
    if (syear < 1970 || syear > 2099) return 1;
    for (t = 1970; t < syear; t++) //把所有年份的秒钟相加
    {
        if (Is_Leap_Year(t))
            seccount += 31622400; //闰年的秒钟数
        else
            seccount += 31536000; //平年的秒钟数
    }
    smon -= 1;
    for (t = 0; t < smon; t++)                                //把前面月份的秒钟数相加
    {
        seccount += (uint32_t)mon_table[t] * 86400;           //月份秒钟数相加
        if (Is_Leap_Year(syear) && t == 1) seccount += 86400; //闰年2月份增加一天的秒钟数
    }
    seccount += (uint32_t)(sday - 1) * 86400;                 //把前面日期的秒钟数相加
    seccount += (uint32_t)hour * 3600;                        //小时秒钟数
    seccount += (uint32_t)min * 60;                           //分钟秒钟数
    seccount += sec;                                          //最后的秒钟加上去
    //设置时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);                                             //使能后备寄存器访问
    //上面三步是必须的!

    RTC_SetAlarm(seccount);

    RTC_WaitForLastTask(); //等待最近一次对RTC寄存器的写操作完成

    return 0;
}


/**
 * @brief 获取当前日期和时间
 *
 * 从RTC（实时时钟）中获取当前的时间戳，并计算出对应的日期和时间，存储在calendar结构体中。
 *
 * @return 0 表示成功
 */
uint8_t RTC_Get(void)
{
    static uint16_t daycnt = 0;

    uint32_t temp  = 0;
    uint16_t temp1 = 0;
    timecount      = RTC_GetCounter();
    temp           = timecount / 86400; //得到天数(秒钟数对应的)
    if (daycnt != temp)                 //超过一天了
    {
        daycnt = temp;
        temp1  = 1970;               //从1970年开始
        while (temp >= 365)          //计算出来的天数大于等于一年的天数
        {
            if (Is_Leap_Year(temp1)) //是闰年
            {
                if (temp >= 366)
                {
                    temp -= 366; //闰年的秒钟数
                }
                else
                {
                    temp1++;
                    break;
                }
            }
            else
                temp -= 365; //平年
            temp1++;
        }
        calendar.w_year = temp1;                             //得到年份
        temp1           = 0;
        while (temp >= 28)                                   //超过了一个月
        {
            if (Is_Leap_Year(calendar.w_year) && temp1 == 1) //当年是不是闰年/2月份
            {
                if (temp >= 29)                              //如果天数超过29天
                {
                    temp -= 29;                              //闰年的秒钟数
                }
                else
                    break; //小于29天，不到闰年3月，直接跳出。
            }
            else
            {
                if (temp >= mon_table[temp1])
                {
                    temp -= mon_table[temp1]; //平年
                }
                else
                    break;
            }
            temp1++;
        }
        calendar.w_month = temp1 + 1;                                                 //得到月份
        calendar.w_date  = temp + 1;                                                  //得到日期
    }
    temp          = timecount % 86400;                                                //得到秒钟数
    calendar.hour = temp / 3600;                                                      //小时
    calendar.min  = (temp % 3600) / 60;                                               //分钟
    calendar.sec  = (temp % 3600) % 60;                                               //秒钟
    calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date); //获取星期
    return 0;
}

/**
 * @brief 获取指定日期是星期几
 *
 * 根据给定的年、月、日，计算并返回该日期是星期几（0-6，0代表星期日）
 *
 * @param year 年份（1900-9999）
 * @param month 月份（1-12）
 * @param day 日期（1-31）
 *
 * @return 返回星期几（0-6，0代表星期日）
 */
uint8_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t  yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    // 如果为21世纪,年份数加100
    if (yearH > 19) yearL += 100;
    // 所过闰年数只算1900年之后的
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3) temp2--;
    return (temp2 % 7);
}
