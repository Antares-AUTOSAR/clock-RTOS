/**
 * @file    clock.c
 * @brief   ****
 *
 * Clock driver source file
 * Includes source for functions:
 *  Work in progress
 *
 */

#include "clock.h"


/**
 * @brief  Function prototypes for each state action procedure
 */
static void state_serialMsgAlarm( APP_MsgTypeDef *receivedMessage );
static void state_serialMsgDate( APP_MsgTypeDef *receivedMessage );
static void state_serialMsgTime( APP_MsgTypeDef *receivedMessage );
static void state_clockMsgPrint( APP_MsgTypeDef *receivedMessage );


/**
 * @brief  Function to trigger the event machine
 */
static void Clock_EventMachine( APP_MsgTypeDef *receivedMessage );


/**
 * @brief  Struct for handling RTC peripheral Declared in bsp
 */
RTC_HandleTypeDef RtcHandler = { 0 };


/**
 * @brief  Struct for handling date correctly
 */
static uint8_t dateYearH = 19;


/**
 * @brief   Use once to Initialize the clock
 *
 * Initializes RTC peripheral using 32.768kHz external clock
 * Sets time and date to 12:00:00 JAN 1 1999
 *
 */
void Clock_Init( void )
{
    RTC_DateTypeDef sDate = { 0 };
    RTC_TimeTypeDef sTime = { 0 };

    RtcHandler.Instance            = RTC;
    RtcHandler.Init.HourFormat     = RTC_HOURFORMAT_24;
    RtcHandler.Init.AsynchPrediv   = 127;
    RtcHandler.Init.SynchPrediv    = 255;
    RtcHandler.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RtcHandler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandler.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init( &RtcHandler );

    sTime.Hours          = 0x12;
    sTime.Minutes        = 0x00;
    sTime.Seconds        = 0x00;
    sTime.SubSeconds     = 0x00;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime( &RtcHandler, &sTime, RTC_FORMAT_BCD );

    sDate.Month   = RTC_MONTH_JANUARY;
    sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
    sDate.Date    = 0x01;
    sDate.Year    = 0x99;
    dateYearH     = 19;
    HAL_RTC_SetDate( &RtcHandler, &sDate, RTC_FORMAT_BCD );
}


/**
 * @brief   Run continuosuly. It allows to configure the clock
 *
 * Verifies if clockQueue has elements to read, if it's true passes the received
 * message(s) to eventmachine for processing data according to its type
 *
 */
void Clock_Task( void )
{
    static APP_MsgTypeDef messageStruct = { 0 };

    while( xQueueReceive( clockQueue, &messageStruct, 0 ) != errQUEUE_EMPTY )
    {
        Clock_EventMachine( &messageStruct );
    }
}


/**
 * @brief   Process message according to its type
 */
void Clock_EventMachine( APP_MsgTypeDef *receivedMessage )
{
    /*  Define lookup table  */
    static StateNode stateMachine[ TOTAL_CLOCK_STATES ] = {
    { state_serialMsgAlarm },
    { state_serialMsgDate },
    { state_serialMsgTime },
    { state_clockMsgPrint } };


    /*  Variable to handle the current state  */
    static APP_Messages event;

    event = receivedMessage->msg;
    stateMachine[ event ].stateFunc( receivedMessage );
}


/**
 * @brief   Function executed when Serial Alarm message has been received
 */
static void state_serialMsgAlarm( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    UNUSED( receivedMessage );

    clockMessage.msg = CLOCK_MSG_PRINT; // Message to indicate to LCD update displayed data
    xQueueSend( clockQueue, &clockMessage, 0 );
}


/**
 * @brief   Function executed when Serial Date message has been received
 */
static void state_serialMsgDate( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    RTC_DateTypeDef sDate = { 0 };

    sDate.Date  = receivedMessage->tm.tm_mday;
    sDate.Month = receivedMessage->tm.tm_mon;
    sDate.Year  = receivedMessage->tm.tm_year % 100UL;
    dateYearH   = receivedMessage->tm.tm_year / 100UL;
    HAL_RTC_SetDate( &RtcHandler, &sDate, RTC_FORMAT_BIN );

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, 0 );
}


/**
 * @brief   Function executed when Serial Time message has been received
 */
static void state_serialMsgTime( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    RTC_TimeTypeDef sTime = { 0 };

    sTime.Hours   = receivedMessage->tm.tm_hour;
    sTime.Minutes = receivedMessage->tm.tm_min;
    sTime.Seconds = receivedMessage->tm.tm_sec;
    HAL_RTC_SetTime( &RtcHandler, &sTime, RTC_FORMAT_BIN );

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, 0 );
}


/**
 * @brief   Function executed when Clock Date message has been received
 */
static void state_clockMsgPrint( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    UNUSED( receivedMessage );

    RTC_DateTypeDef sDate = { 0 };
    RTC_TimeTypeDef sTime = { 0 };

    HAL_RTC_GetTime( &RtcHandler, &sTime, RTC_FORMAT_BIN );
    HAL_RTC_GetDate( &RtcHandler, &sDate, RTC_FORMAT_BIN );

    clockMessage.tm.tm_hour = sTime.Hours;
    clockMessage.tm.tm_min  = sTime.Minutes;
    clockMessage.tm.tm_sec  = sTime.Seconds;
    clockMessage.tm.tm_year = sDate.Year + ( dateYearH * 100U );
    clockMessage.tm.tm_mday = sDate.Date;
    clockMessage.tm.tm_wday = sDate.WeekDay;
    clockMessage.tm.tm_mon  = sDate.Month;

    xQueueSend( displayQueue, &clockMessage, 0 );
}


/**
 * @brief   Function executed every second to print data in LCD
 */
void Clock_Update_DateAndTime( void )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, 0 );
}