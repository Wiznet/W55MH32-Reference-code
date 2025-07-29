#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_

#include <stdint.h>

//时间结构体
typedef struct
{
    volatile uint8_t hour;
    volatile uint8_t min;
    volatile uint8_t sec;
    //公历日月年周
    volatile uint16_t w_year;
    volatile uint8_t  w_month;
    volatile uint8_t  w_date;
    volatile uint8_t  week;
} _calendar_obj;
extern _calendar_obj calendar;                                                                                    //日历结构体

extern uint8_t const mon_table[12];                                                                               //月份日期数据表
void                 Disp_Time(uint8_t x, uint8_t y, uint8_t size);                                               //在制定位置开始显示时间
void                 Disp_Week(uint8_t x, uint8_t y, uint8_t size, uint8_t lang);                                 //在指定位置显示星期
uint8_t              RTC_Init(void);                                                                              //初始化RTC,返回0,失败;1,成功;
uint8_t              Is_Leap_Year(uint16_t year);                                                                 //平年,闰年判断
uint8_t              RTC_Alarm_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t              RTC_Get(void);                                                                               //更新时间
uint8_t              RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day);
uint8_t              RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec); //设置时间

#endif
