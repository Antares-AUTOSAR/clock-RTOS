/**
 * @file    app_clock.c
 * @brief   Clock driver source file
 *
 * Find here source for interfaces and private functions which allows to
 * initialize and set the clock
 */

#include "app_clock.h"
#include "bsp.h"

/**
 * @brief   Struct for store function for each state of event machine
 */
typedef struct
{
    void ( *stateFunc )( APP_MsgTypeDef *receivedMessage ); /*!< Pointer to function which perform state statements */
} StateNode;

/* Function prototypes for each state action procedure */
static void state_serialMsgAlarm( APP_MsgTypeDef *receivedMessage );
static void state_serialMsgDate( APP_MsgTypeDef *receivedMessage );
static void state_serialMsgTime( APP_MsgTypeDef *receivedMessage );
static void state_clockMsgPrint( APP_MsgTypeDef *receivedMessage );

/* Function prototype to send an update message every second*/
static void Clock_Update_DateAndTime( TimerHandle_t pxTimer );

/* Function prototype to trigger the event machine */
static void Clock_EventMachine( APP_MsgTypeDef *receivedMessage );


/**
 * @brief  Struct for handling RTC peripheral Declared in bsp
 */
RTC_HandleTypeDef RtcHandler;


/**
 * @brief  Variable for handling date correctly (stores 2 higher digits of year)
 */
static uint8_t dateYearH;


/**
 * @brief  Struct for handling Software timer
 */
TimerHandle_t xTimerDisplay;

/**
 * @brief  Flag that tell us that alarm has been activated
 */
static uint8_t Alarm_Active = 0;

/**
 * @brief  Flag that tell us that will stop the msg of alarm and clean it
 */
static uint8_t Stop_Alarm = 0;

/**
 * @brief   Use once to Initialize the clock
 *
 * Initializes RTC peripheral using 32.768kHz external clock
 * Sets time and date to 12:00:00 JAN 1 1999
 */
void Clock_Init( void )
{
    RTC_DateTypeDef sDate = { 0 };
    RTC_TimeTypeDef sTime = { 0 };

    RtcHandler.Instance            = RTC;
    RtcHandler.Init.HourFormat     = RTC_HOURFORMAT_24;
    RtcHandler.Init.AsynchPrediv   = RTC_INIT_PASYNC;
    RtcHandler.Init.SynchPrediv    = RTC_INIT_PSYNC;
    RtcHandler.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RtcHandler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandler.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init( &RtcHandler );

    sTime.Hours          = RTC_INITIAL_HOUR;
    sTime.Minutes        = RTC_INITIAL_MIN;
    sTime.Seconds        = RTC_INITIAL_SEC;
    sTime.SubSeconds     = RTC_INITIAL_SSEC; // Initial time 12:00:00.0
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime( &RtcHandler, &sTime, RTC_FORMAT_BCD );

    sDate.Month   = RTC_MONTH_JANUARY;
    sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
    sDate.Date    = RTC_INITIAL_MDAY;
    sDate.Year    = RTC_INITIAL_YEAR;
    dateYearH     = RTC_INITIAL_YEARH; // Initial date JAN1 2000
    HAL_RTC_SetDate( &RtcHandler, &sDate, RTC_FORMAT_BCD );

    HAL_NVIC_SetPriority( EXTI4_15_IRQn, 2, 0 );
    HAL_NVIC_EnableIRQ( EXTI4_15_IRQn );

    xTimerDisplay = xTimerCreate( "D_Timer", 1000, pdTRUE, TIMER_DISPLAY_ID, Clock_Update_DateAndTime );
    xTimerStart( xTimerDisplay, 0 );
}


/**
 * @brief   Run periodically. It allows to configure the clock
 *
 * Verifies if clockQueue has elements to read, if it's true passes the received
 * message(s) to eventmachine for processing data according to its type
 */
void Clock_Task( void )
{
    static APP_MsgTypeDef messageStruct = { 0 };

    while( xQueueReceive( clockQueue, &messageStruct, TICKS ) != errQUEUE_EMPTY )
    {
        Clock_EventMachine( &messageStruct );
    }
    if( Stop_Alarm == TRUE_A )
    {
        Stop_Alarm        = FALSE_A;
        messageStruct.msg = OK_STATE;
        xQueueSend( displayQueue, &messageStruct, TICKS );
    }
}


/**
 * @brief   Process message according to its type
 * @param   receivedMessage Message readed from clockQueue which allows to move between states inside machine
 *
 * The state machine is builded with pointers to functions stored inside a struct (StateNode)
 * which represent a state of the machine
 *
 * For improve the code reading, an array of StateNode (stateMachine) stores all the nodes
 * (states of the machine)
 *
 * Every time this function is executed, it  calls a process according to the received
 * message's type
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
 * @brief   (State) Function executed when Serial Alarm message has been received
 * @param   receivedMessage Message received from clockQueue
 *
 * Sets the RTC with received alarm with interrupt
 *
 * Sends message to clockQueue everytime a type alarm message has been received
 */
static void state_serialMsgAlarm( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    RTC_AlarmTypeDef sAlarm = { 0 };

    sAlarm.AlarmTime.Hours          = receivedMessage->tm.tm_hour_alarm;
    sAlarm.AlarmTime.Minutes        = receivedMessage->tm.tm_min_alarm;
    sAlarm.Alarm                    = RTC_ALARM_A;
    sAlarm.AlarmDateWeekDay         = 1;
    sAlarm.AlarmDateWeekDaySel      = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmMask                = RTC_ALARMMASK_SECONDS | RTC_ALARMMASK_DATEWEEKDAY;
    sAlarm.AlarmSubSecondMask       = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

    HAL_RTC_SetAlarm_IT( &RtcHandler, &sAlarm, RTC_FORMAT_BIN );

    if( Alarm_Active == ON )
    {
        xTimerStart( xTimerDisplay, TICKS );
        Alarm_Active = OFF;
        Stop_Alarm   = TRUE_A;
    }

    clockMessage.msg = SERIAL_MSG_TIME;
    xQueueSend( displayQueue, &clockMessage, TICKS );
}


/**
 * @brief   (State) Function executed when Serial Date message has been received
 * @param   receivedMessage Message received from clockQueue
 *
 * Sets the RTC with received date
 * Sends message to clockQueue everytime a type date message has been received
 */
static void state_serialMsgDate( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    RTC_DateTypeDef sDate = { 0 };

    sDate.Date    = receivedMessage->tm.tm_mday;
    sDate.Month   = receivedMessage->tm.tm_mon;
    sDate.Year    = receivedMessage->tm.tm_year % 100UL;
    sDate.WeekDay = receivedMessage->tm.tm_wday;
    dateYearH     = receivedMessage->tm.tm_year / 100UL;
    HAL_RTC_SetDate( &RtcHandler, &sDate, RTC_FORMAT_BIN );

    if( Alarm_Active == ON )
    {
        xTimerStart( xTimerDisplay, TICKS );
        Alarm_Active = OFF;
        Stop_Alarm   = TRUE_A;
    }

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, TICKS );
}


/**
 * @brief   (State) Function executed when Serial Time message has been received
 * @param   receivedMessage Message received from clockQueue
 *
 * Sets the RTC with received time
 * Sends message to clockQueue everytime a time date message has been received
 */
static void state_serialMsgTime( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    RTC_TimeTypeDef sTime = { 0 };

    sTime.Hours   = receivedMessage->tm.tm_hour;
    sTime.Minutes = receivedMessage->tm.tm_min;
    sTime.Seconds = receivedMessage->tm.tm_sec;
    HAL_RTC_SetTime( &RtcHandler, &sTime, RTC_FORMAT_BIN );

    if( Alarm_Active == ON )
    {
        xTimerStart( xTimerDisplay, TICKS );
        Alarm_Active = OFF;
        Stop_Alarm   = TRUE_A;
    }

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, TICKS );
}


/**
 * @brief   (State) Function executed when Clock Date message has been received
 * @param   receivedMessage Message received from clockQueue.
 *
 * Sends a message to display queue indicating data must be printed in LCD
 * Due to event machine implementation, the parameter must be passed but specified
 * as UNUSED
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
 * @param   pxTimer Timer handler
 *
 * Sends a message every second to clockQueue indicating data must be printed
 * in LCD
 */
void Clock_Update_DateAndTime( TimerHandle_t pxTimer )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    UNUSED( pxTimer );

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, 0 );
}

/**
 * @brief  Callback interruption of the ALARM
 *
 * This interruption activates it when the alarm has been setted, when it happens it will desactivated the alarm
 * and will send us to the case of displaying the ALARM. Also, the timer for displaying each second will stop
 * @param   hrtc A pointer to the RTC structure
 */
/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void HAL_RTC_AlarmAEventCallback( RTC_HandleTypeDef *hrtc )
{
    static APP_MsgTypeDef DisplayMsg = { 0 };

    HAL_RTC_DeactivateAlarm( hrtc, RTC_ALARM_A );

    Alarm_Active = ON;

    xTimerStop( xTimerDisplay, TICKS );

    DisplayMsg.msg = CLOCK_MSG_PRINT;
    xQueueSend( displayQueue, &DisplayMsg, TICKS );
}