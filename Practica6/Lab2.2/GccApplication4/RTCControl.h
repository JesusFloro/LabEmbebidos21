#ifndef RTCCONTROL_H_
#define RTCCONTROL_H_

#include"stdint.h"

#define SIZE 0x7;

#define YEAR_ADDR 0x6;
#define MONTH_ADDR 0x5;
#define WEEKDAY_ADDR 0x4;
#define DAY_ADDR 0x3;
#define HOURS_ADDR 0x2;
#define MINUTES_ADDR 0x1;
#define SECONDS_ADDR 0x00;

typedef union { // Year
    struct  {
   	 uint8_t un_year: 4,
   	 dec_year: 4;
    } field;
    uint8_t reg;
} year_type;

typedef union { // Month
    struct  {
   	 uint8_t un_month: 4,
   	 dec_month: 1,
   	 RESERVED: 3;
    } field;
    uint8_t reg;
} month_type;

typedef union { // Date
    struct  {
   	 uint8_t un_date: 4,
   	 dec_date: 2,
   	 RESERVED: 2;
    } field;
    uint8_t reg;
} date_type;

typedef union { // Day
    struct  {
   	 uint8_t un_day: 3,
   	 RESERVED: 5;
    } field;
    uint8_t reg;
} day_type;

typedef union { // Hours
    struct  {
   	 uint8_t un_hr : 4,
   	 dec_hr : 1,
   	 dec_mode: 1,
   	 dec_ampm: 1,
   	 RESERVED;
    } field;
    uint8_t reg;
} hours_type;

typedef union { // Minutes
    struct  {
   	 uint8_t un_min: 4,
   	 dec_min: 3,
   	 RESERVED: 1;
    } field;
    uint8_t reg;
} minutes_type;

typedef union { // Seconds
    struct  {
   	 uint8_t un_seg: 4,
   	 dec_seg: 3,
   	 RESERVED: 1;
    } field;
    uint8_t reg;
} seconds_type;

typedef union { // CTRL Reg
    struct  {
   	 uint8_t RS0: 1,
   	 RS1: 1,
   	 RES1: 2,
   	 SQWE: 1,
   	 RES2: 2,
   	 OUT: 1;
    } field;
    uint8_t reg;
} control_type;

typedef struct{
    seconds_type seconds;
    minutes_type minutes;
    hours_type hours;
    day_type days;
    date_type date;
    month_type month;
    year_type year;
    control_type control;
}RTControl;

void initRTC(RTControl* pRTCi);
void initI2C();
void sendI2CDataArray(uint8_t slaveAdd,RTControl *data);
void ReciveI2CDataArray(uint8_t slaveAdd,RTControl *data);
void printRTC(RTControl*p);
void StopCond();
void SendI2CData(RTControl *data);

#endif /* RTCCONTROL_H_ */
