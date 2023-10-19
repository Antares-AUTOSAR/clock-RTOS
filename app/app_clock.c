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
    MACHINE_State ( *stateFunc )( APP_MsgTypeDef *receivedMessage ); /*!< Pointer to function which perform state statements */
} StateNode;

/* Function prototypes for each state action procedure */
static MACHINE_State state_serialMsgAlarm( APP_MsgTypeDef *receivedMessage );
static MACHINE_State state_serialMsgDate( APP_MsgTypeDef *receivedMessage );
static MACHINE_State state_serialMsgTime( APP_MsgTypeDef *receivedMessage );
static MACHINE_State state_clockMsgPrint( APP_MsgTypeDef *receivedMessage );

STATIC MACHINE_State Clock_EventMachine( APP_MsgTypeDef *receivedMessage );
STATIC void Clock_Update_DateAndTime( TimerHandle_t pxTimer );

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

    while( xQueueReceive( clockQueue, &messageStruct, 0 ) != errQUEUE_EMPTY )
    {
        (void)Clock_EventMachine( &messageStruct );
    }
}


/**
 * @brief   Process message according to its type
 * @param   receivedMessage Message readed from clockQueue which allows to move between states inside machine
 * @retval  Returns the next state that will be executed
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
MACHINE_State Clock_EventMachine( APP_MsgTypeDef *receivedMessage )
{
    /*  Define lookup table  */
    static StateNode stateMachine[ TOTAL_CLOCK_STATES ] = {
    { state_serialMsgAlarm },
    { state_serialMsgDate },
    { state_serialMsgTime },
    { state_clockMsgPrint } };

    static MACHINE_State nextEvent;

    /*  Variable to handle the current state  */
    static APP_Messages event;

    event     = receivedMessage->msg;
    nextEvent = stateMachine[ event ].stateFunc( receivedMessage );

    return nextEvent;
}


/**
 * @brief   (State) Function executed when Serial Alarm message has been received
 * @param   receivedMessage Message received from clockQueue
 * @retval  Returns the next state that will be executed
 *
 * Due to event machine implementation, the parameter must be passed but specified
 * as UNUSED until alarm feature is integrated to project
 *
 * Sends message to clockQueue everytime a type alarm message has been received
 */
MACHINE_State state_serialMsgAlarm( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    UNUSED( receivedMessage );

    clockMessage.msg = CLOCK_MSG_PRINT; // Message to indicate to LCD update displayed data
    xQueueSend( clockQueue, &clockMessage, 0 );

    return CLOCK_STATE_PRINT; // Next state
}


/**
 * @brief   (State) Function executed when Serial Date message has been received
 * @param   receivedMessage Message received from clockQueue
 * @retval  Returns the next state that will be executed
 *
 * Sets the RTC with received date
 * Sends message to clockQueue everytime a type date message has been received
 */
MACHINE_State state_serialMsgDate( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    RTC_DateTypeDef sDate = { 0 };

    sDate.Date    = receivedMessage->tm.tm_mday;
    sDate.Month   = receivedMessage->tm.tm_mon;
    sDate.Year    = receivedMessage->tm.tm_year % 100UL;
    sDate.WeekDay = receivedMessage->tm.tm_wday;
    dateYearH     = receivedMessage->tm.tm_year / 100UL;
    HAL_RTC_SetDate( &RtcHandler, &sDate, RTC_FORMAT_BIN );

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, 0 );

    return CLOCK_STATE_PRINT; // Next state
}


/**
 * @brief   (State) Function executed when Serial Time message has been received
 * @param   receivedMessage Message received from clockQueue
 * @retval  Returns the next state that will be executed
 *
 * Sets the RTC with received time
 * Sends message to clockQueue everytime a time date message has been received
 */
MACHINE_State state_serialMsgTime( APP_MsgTypeDef *receivedMessage )
{
    static APP_MsgTypeDef clockMessage = { 0 };

    RTC_TimeTypeDef sTime = { 0 };

    sTime.Hours   = receivedMessage->tm.tm_hour;
    sTime.Minutes = receivedMessage->tm.tm_min;
    sTime.Seconds = receivedMessage->tm.tm_sec;
    HAL_RTC_SetTime( &RtcHandler, &sTime, RTC_FORMAT_BIN );

    clockMessage.msg = CLOCK_MSG_PRINT;
    xQueueSend( clockQueue, &clockMessage, 0 );

    return CLOCK_STATE_PRINT; // Next state
}


/**
 * @brief   (State) Function executed when Clock Date message has been received
 * @param   receivedMessage Message received from clockQueue
 * @retval  Returns the next state that will be executed
 *
 * Sends a message to display queue indicating data must be printed in LCD
 * Due to event machine implementation, the parameter must be passed but specified
 * as UNUSED
 */
MACHINE_State state_clockMsgPrint( APP_MsgTypeDef *receivedMessage )
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

    return CLOCK_STATE_IDLE; // Next state
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