#include "unity.h"
#include "app_clock.h"

#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_rtc.h"
#include "mock_stm32g0xx_hal_rtc_ex.h"
#include "mock_timers.h"
#include "mock_task.h"
#include "mock_queue.h"
#include "mock_stm32g0xx_hal_cortex.h"

#include "bsp.h"

STATIC MACHINE_State Clock_EventMachine( APP_MsgTypeDef *receivedMessage );
STATIC void Clock_Update_DateAndTime( TimerHandle_t pxTimer );

void setUp( void )
{
}

void tearDown( void )
{
}

extern uint8_t Alarm_Active;
extern uint8_t Stop_Alarm;

/* Define queue for display */
QueueHandle_t displayQueue;

/* Define queue for clock */
QueueHandle_t clockQueue;

/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__Clock_Init__callFunction( void )
{

    TimerHandle_t testTimer = { 0 };

    HAL_RTC_Init_IgnoreAndReturn( HAL_OK );
    HAL_RTC_SetTime_IgnoreAndReturn( HAL_OK );
    HAL_RTC_SetDate_IgnoreAndReturn( HAL_OK );
    xTimerCreate_IgnoreAndReturn( testTimer );
    xTaskGetTickCount_IgnoreAndReturn( 1u );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );
    HAL_NVIC_SetPriority_Expect( EXTI4_15_IRQn, 2, 0 );
    HAL_NVIC_EnableIRQ_Expect( EXTI4_15_IRQn );


    Clock_Init( );
}


/* Test calling for Clock_Task function when queue is empty
    Ignore reading message from queue assuming that is empty and returns errQUEUE_EMPTY value
    Ignore sending message to queue assuming that will return pdPASS value
    THIS TEST 1/2 BRANCHES OF THIS FUNCTION
*/
void test__Clock_Task__emptyQueue( void )
{
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );
    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    Clock_Task( );
}

/* Test calling for Clock_Task function when queue has one message and reads from it
    Ignore reading message from queue assuming that has a message pdPASS value
    Ignore sending message to queue assuming that will return pdPASS value
    After reading the message from queue assume that is empty and returns errQUEUE_EMPTY value
    THIS TEST 2/2 BRANCHES OF THIS FUNCTION
*/
void test__Clock_Task__oneMessageinQueue( void )
{
    Stop_Alarm = 1;
    HAL_RTC_SetAlarm_IT_IgnoreAndReturn( HAL_OK );
    xQueueReceive_IgnoreAndReturn( pdPASS );
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );
    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    TEST_ASSERT_EQUAL( 1, Stop_Alarm );
    Clock_Task( );
}

/* Test calling for Clock_EventMachine function when a SERIAL_MSG_ALARM message has been received
    Ignore sending message to queue assuming that will return pdPASS value
    Invoke the function Clock_EventMachine with a message of type SERIAL_MSG_ALARM
    The tested function will return the next state that will be executed, in this case must be CLOCK_STATE_PRINT
*/
void test__Clock_EventMachine__sendAlarmMessage( void )
{
    Alarm_Active           = 1;
    APP_MsgTypeDef message = { 0 };
    MACHINE_State state;

    message.msg = SERIAL_MSG_ALARM;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_SetAlarm_IT_IgnoreAndReturn( HAL_OK );

    TEST_ASSERT_EQUAL( 1, Alarm_Active );
    xTaskGetTickCount_IgnoreAndReturn( 1u );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );

    state = Clock_EventMachine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( CLOCK_STATE_PRINT, state, "Execution of a SERIAL_MSG_ALARM must return a CLOCK_STATE_PRINT (3)" );
}

void test__Clock_EventMachine__sendAlarmMessage2( void )
{
    Alarm_Active           = 0;
    APP_MsgTypeDef message = { 0 };
    MACHINE_State state;

    message.msg = SERIAL_MSG_ALARM;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_SetAlarm_IT_IgnoreAndReturn( HAL_OK );

    TEST_ASSERT_EQUAL( 0, Alarm_Active );

    state = Clock_EventMachine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( CLOCK_STATE_PRINT, state, "Execution of a SERIAL_MSG_ALARM must return a CLOCK_STATE_PRINT (3)" );
}

/* Test calling for Clock_EventMachine function when a SERIAL_MSG_DATE message has been received
    Ignore sending message to queue assuming that will return pdPASS value
    Invoke the function Clock_EventMachine with a message of type SERIAL_MSG_DATE
    The tested function will return the next state that will be executed, in this case must be CLOCK_STATE_PRINT
*/
void test__Clock_EventMachine__sendDateMessage( void )
{
    Alarm_Active           = 1;
    APP_MsgTypeDef message = { 0 };
    MACHINE_State state;

    message.msg = SERIAL_MSG_DATE;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_SetDate_IgnoreAndReturn( HAL_OK );
    TEST_ASSERT_EQUAL( 1, Alarm_Active );
    xTaskGetTickCount_IgnoreAndReturn( 1u );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );

    state = Clock_EventMachine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( CLOCK_STATE_PRINT, state, "Execution of a SERIAL_MSG_DATE must return a CLOCK_STATE_PRINT (3)" );
}

void test__Clock_EventMachine__sendDateMessage2( void )
{
    Alarm_Active           = 0;
    APP_MsgTypeDef message = { 0 };
    MACHINE_State state;

    message.msg = SERIAL_MSG_DATE;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_SetDate_IgnoreAndReturn( HAL_OK );
    TEST_ASSERT_EQUAL( 0, Alarm_Active );

    state = Clock_EventMachine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( CLOCK_STATE_PRINT, state, "Execution of a SERIAL_MSG_DATE must return a CLOCK_STATE_PRINT (3)" );
}
/* Test calling for Clock_EventMachine function when a SERIAL_MSG_TIME message has been received
    Ignore sending message to queue assuming that will return pdPASS value
    Invoke the function Clock_EventMachine with a message of type SERIAL_MSG_TIME
    The tested function will return the next state that will be executed, in this case must be CLOCK_STATE_PRINT
*/
void test__Clock_EventMachine__sendTimeMessage( void )
{
    Alarm_Active           = 1;
    APP_MsgTypeDef message = { 0 };
    MACHINE_State state;

    message.msg = SERIAL_MSG_TIME;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_SetTime_IgnoreAndReturn( HAL_OK );
    TEST_ASSERT_EQUAL( 1, Alarm_Active );
    xTaskGetTickCount_IgnoreAndReturn( 1u );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );

    state = Clock_EventMachine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( CLOCK_STATE_PRINT, state, "Execution of a SERIAL_MSG_TIME must return a CLOCK_STATE_PRINT (3)" );
}

void test__Clock_EventMachine__sendTimeMessage2( void )
{
    Alarm_Active           = 0;
    APP_MsgTypeDef message = { 0 };
    MACHINE_State state;

    message.msg = SERIAL_MSG_TIME;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_SetTime_IgnoreAndReturn( HAL_OK );
    TEST_ASSERT_EQUAL( 0, Alarm_Active );

    state = Clock_EventMachine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( CLOCK_STATE_PRINT, state, "Execution of a SERIAL_MSG_TIME must return a CLOCK_STATE_PRINT (3)" );
}

/* Test calling for Clock_EventMachine function when a CLOCK_MSG_PRINT message has been received
    Ignore sending message to queue assuming that will return pdPASS value
    Invoke the function Clock_EventMachine with a message of type CLOCK_MSG_PRINT
    The tested function will return the next state that will be executed, in this case must be CLOCK_STATE_IDLE
*/
void test__Clock_EventMachine__sendPrintMessage( void )
{
    APP_MsgTypeDef message = { 0 };
    MACHINE_State state;

    message.msg = CLOCK_MSG_PRINT;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_GetTime_IgnoreAndReturn( HAL_OK );
    HAL_RTC_GetDate_IgnoreAndReturn( HAL_OK );

    state = Clock_EventMachine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( CLOCK_STATE_IDLE, state, "Execution of a CLOCK_MSG_PRINT must return a CLOCK_STATE_IDLE (3)" );
}

/* Test calling for Clock_Update_DateAndTime function
    Ignore sending message to queue assuming that will return pdPASS value
    JUST TEST THAT ALL THE LINES ARE EXECUTED FOR REACH 100% COVERAGE
*/
void test__Clock_Update__DateAndTime( void )
{
    TimerHandle_t testTimer = { 0 };
    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    Clock_Update_DateAndTime( testTimer );
}

void test_RTC_CALLBACK( void )
{
    RTC_HandleTypeDef hrtc_mock;
    HAL_RTC_DeactivateAlarm_ExpectAndReturn( &hrtc_mock, RTC_ALARM_A, pdPASS );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );
    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HAL_RTC_AlarmAEventCallback( &hrtc_mock );
}