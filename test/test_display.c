#include "bsp.h"
#include "app_display.h"
#include "hel_lcd.h"
#include "unity.h"
#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_spi.h"
#include "mock_queue.h"
#include "mock_hel_lcd.h"
#include "mock_timers.h"
#include "mock_task.h"
#include "mock_stm32g0xx_hal_tim.h"

TimerHandle_t xTimerDisplay;

// stm32g0xx_hal_tim.c
QueueHandle_t displayQueue;

extern LCD_HandleTypeDef Hlcd;
extern SPI_HandleTypeDef SpiHandle;
extern uint8_t buzzer_flag;
extern uint8_t buzzer;


STATIC char *get_month( uint8_t month );
STATIC void TimeString( char *string, uint8_t hours, uint8_t minutes, uint8_t seconds );
STATIC void DateString( char *string, uint8_t month, uint8_t day, uint16_t year, uint8_t weekday );
STATIC Display_M Date( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Time( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Alarm_A( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Alarm( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Alarm_Clean( APP_MsgTypeDef *DisplayMsg );
STATIC void Display_Buzzer( TimerHandle_t pxTimer );
STATIC void Display_1Mn_Buzzer( TimerHandle_t pxTimer );

void setUp( void )
{
}

void tearDown( void )
{
}

/**
 * @brief Testing for validation get_month
 *
 * This function test the behavior of the get_month for valid month values
 * It asserts that the function returns the correct month abbreviation for each value input
 *
 */
void test_getMonth_valid( void )
{
    TEST_ASSERT_EQUAL_STRING( "JAN", get_month( 1 ) );
    TEST_ASSERT_EQUAL_STRING( "FEB", get_month( 2 ) );
    TEST_ASSERT_EQUAL_STRING( "MAR", get_month( 3 ) );
    TEST_ASSERT_EQUAL_STRING( "APR", get_month( 4 ) );
    TEST_ASSERT_EQUAL_STRING( "MAY", get_month( 5 ) );
    TEST_ASSERT_EQUAL_STRING( "JUN", get_month( 6 ) );
    TEST_ASSERT_EQUAL_STRING( "JUL", get_month( 7 ) );
    TEST_ASSERT_EQUAL_STRING( "AUG", get_month( 8 ) );
    TEST_ASSERT_EQUAL_STRING( "SEP", get_month( 9 ) );
    TEST_ASSERT_EQUAL_STRING( "OCT", get_month( 10 ) );
    TEST_ASSERT_EQUAL_STRING( "NOV", get_month( 11 ) );
    TEST_ASSERT_EQUAL_STRING( "DEC", get_month( 12 ) );
}

/**
 * @brief Testing for invalidating get_month
 *
 * This function test the behavior of the get_month for invalid month values
 * It asserts that the function returns Null for invalid input
 */
void test_getMonth_invalid( void )
{
    TEST_ASSERT_NULL( get_month( 0 ) );
    TEST_ASSERT_NULL( get_month( 13 ) );
}

/**
 * @brief Test for validating TimeString function
 *
 * This function test the behavior of the TimeString function by setting a string to "00:00:00"
 * and then using the function to ipdate the string with time values
 */
void test_time_string( void )
{
    char string[] = "00:00:00";
    TimeString( string, 12, 33, 22 );
    TEST_ASSERT_EQUAL_STRING( "12:33:22", string );
}

/**
 * @brief Test for validating DateString function
 *
 * This function test the behavior of the DataString function by setting a string to "000,00 0000 00"
 * and then using the function to update the string with date values
 *
 */
void test_date_string( void )
{
    char date_string[] = "000,00 0000 00";
    DateString( date_string, 04, 12, 2010, 01 );
    TEST_ASSERT_EQUAL_STRING( "APR,12 2010 MO", date_string );
}

/**
 * @brief Test for initialization of display
 *
 * This function test the initialization of the displays and sets expectations for varios display-related functions
 * It checks if this functions are called as expected
 *
 */
void test_Display_Init( void )
{
    TimerHandle_t testTimer = { 0 };

    HAL_SPI_Init_IgnoreAndReturn( HAL_OK );
    HEL_LCD_Init_ExpectAndReturn( &Hlcd, HAL_OK );
    HEL_LCD_Backlight_Expect( &Hlcd, LCD_ON );
    HEL_LCD_Contrast_ExpectAndReturn( &Hlcd, 15, HAL_OK );
    HAL_TIM_PWM_Init_IgnoreAndReturn( HAL_OK );
    HAL_TIM_PWM_ConfigChannel_IgnoreAndReturn( HAL_OK );
    xTimerCreate_IgnoreAndReturn( testTimer );
    xTaskGetTickCount_IgnoreAndReturn( 1u );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );
    Display_Init( );
}

/**
 * @brief Test for the display task when queue is empty
 *
 * This function test the simulation scenario where the display queue is empty
 *
 */
void test_Display_Task_QueueEmpty( void )
{
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );
    Display_Task( );
}


/**
 * @brief Test for the display task when a message is available
 *
 * This functions test the scenario where a message is available in the display
 * Taking into account that a message from the queue has to be called, to be aware that the stateMachine is working
 *
 */
void test_Display_Task_Message( void )
{
    char string[] = "00:00:00";

    APP_MsgTypeDef DisplayMsg =
    {
    .msg = SERIAL_MSG_ALARM,
    };

    xQueueReceive_ExpectAndReturn( displayQueue, &DisplayMsg, 0, pdPASS );
    HEL_LCD_SetCursor_ExpectAndReturn( &Hlcd, 1, 3, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &Hlcd, string, HAL_OK );
    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HEL_LCD_SetCursor_IgnoreAndReturn( HAL_OK );
    HEL_LCD_String_IgnoreAndReturn( HAL_OK );
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );

    Display_Task( );
}

/**
 * @brief Test for the DisplayEventMachine function (Time)
 *
 * This function test the behavior of the event machine when receives a DISPLAY_MSG_TIME message
 */
void test_DisplayEventMachine_MessageTime( void )
{
    char string[] = "12:33:22";

    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    HEL_LCD_SetCursor_ExpectAndReturn( &Hlcd, 1, 3, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &Hlcd, string, HAL_OK );

    APP_MsgTypeDef DisplayMsg = {
    .msg = DISPLAY_MSG_TIME,
    .tm  = {
     .tm_hour = 12,
     .tm_min  = 33,
     .tm_sec  = 22,
    } };

    Display_M result = Time( &DisplayMsg );
    TEST_ASSERT_EQUAL_MESSAGE( DISPLAY_MSG_TIME, result, "Execution of a DISPLAY_MSG_TIME must return a DISPLAY_MSG_TIME (1)" );
}

/**
 * @brief Test for the DisplayEventMachine function (Date)
 *
 * This function test the behavior of the event machine when receives a DISPLAY_MSG_DATE message
 */
void test_DisplayEventMachine_MessageDate( void )
{
    char date_string[] = "APR,12 2010 MO";

    HEL_LCD_SetCursor_ExpectAndReturn( &Hlcd, 0, 1, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &Hlcd, date_string, HAL_OK );

    APP_MsgTypeDef DisplayMsg = {
    .msg = SERIAL_MSG_ALARM,
    .tm  = {
     .tm_mon  = 4,
     .tm_mday = 12,
     .tm_year = 2010,
     .tm_wday = 1 } };

    Display_M result = Date( &DisplayMsg );
    TEST_ASSERT_EQUAL_MESSAGE( DISPLAY_IDLE_STATE, result, "Execution of a DISPLAY_MSG_DATE must return a DISPLAY_IDLE_STATE (0)" );
}

/**
 * @brief
 *
 *
 */
void test_DisplayEventMachine_MessageAlarmA( void )
{
    HEL_LCD_SetCursor_ExpectAndReturn( &Hlcd, 1, 0, HAL_OK );
    HEL_LCD_Data_ExpectAndReturn( &Hlcd, 'A', HAL_OK );

    APP_MsgTypeDef DisplayMsg =
    {
    .msg = SERIAL_MSG_TIME,
    };

    Display_M result = Alarm_A( &DisplayMsg );

    TEST_ASSERT_EQUAL_MESSAGE( DISPLAY_1, result, "Execution of a DISPLAY_MSG_ALARM_A must return a DISPLAY_1 (2)" );
}

/**
 * @brief
 *
 *
 */
void test_DisplayEventMachine_MessageAlarm( void )
{
    char string[] = "ALARM!!!";
    HEL_LCD_SetCursor_ExpectAndReturn( &Hlcd, 1, 0, HAL_OK );
    HEL_LCD_Data_ExpectAndReturn( &Hlcd, ' ', HAL_OK );
    HEL_LCD_SetCursor_ExpectAndReturn( &Hlcd, 1, 3, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &Hlcd, string, HAL_OK );
    HEL_LCD_Backlight_Expect( &Hlcd, LCD_ON );

    APP_MsgTypeDef DisplayMsg =
    {
    .msg = CLOCK_MSG_PRINT,
    };

    Display_M result = Alarm( &DisplayMsg );

    TEST_ASSERT_EQUAL_MESSAGE( DISPLAY_2, result, "Execution of a DISPLAY_MSG_ALARM must return a DISPLAY_2 (3)" );
}

/**
 * @brief
 *
 *
 */
void test_DisplayEventMachine_MessageAlarm_Clean( void )
{
    HAL_TIM_PWM_Stop_IgnoreAndReturn( pdPASS );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );
    HEL_LCD_Backlight_Expect( &Hlcd, LCD_ON );
    xTaskGetTickCount_IgnoreAndReturn( 1u );

    APP_MsgTypeDef DisplayMsg =
    {
    .msg = OK_STATE,
    };

    Display_M result = Alarm_Clean( &DisplayMsg );

    TEST_ASSERT_EQUAL_MESSAGE( DISPLAY_3, result, "Execution of a DISPLAY_MSG_ALARM_CLEAN must return a DISPLAY_3 (4)" );
}

/**
 * @brief
 *
 *
 */
void test_display_buzzer( void )
{
    TimerHandle_t testTimer = { 0 };
    Display_Buzzer( testTimer );
}

/**
 * @brief
 *
 *
 */
void test_display_buzzer_1( void )
{
    buzzer_flag             = 1;
    TimerHandle_t testTimer = { 0 };

    HAL_TIM_PWM_Stop_IgnoreAndReturn( pdPASS );
    HEL_LCD_Backlight_Expect( &Hlcd, LCD_ON );
    Display_Buzzer( testTimer );
    TEST_ASSERT_EQUAL( 1, buzzer_flag );
}

/**
 * @brief
 *
 *
 */
void test_display_buzzer_2( void )
{
    buzzer_flag             = 1;
    buzzer                  = 1;
    TimerHandle_t testTimer = { 0 };

    HAL_TIM_PWM_Stop_IgnoreAndReturn( pdPASS );
    HEL_LCD_Backlight_Expect( &Hlcd, LCD_OFF );
    HAL_TIM_PWM_Start_IgnoreAndReturn( pdPASS );
    Display_Buzzer( testTimer );
    TEST_ASSERT_EQUAL( 0, buzzer );
    TEST_ASSERT_EQUAL( 1, buzzer_flag );
}

/**
 * @brief
 *
 *
 */
void test_buzzer_1mn( void )
{
    TimerHandle_t testTimer = { 0 };
    HAL_TIM_PWM_Stop_IgnoreAndReturn( pdPASS );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );
    HEL_LCD_Backlight_Expect( &Hlcd, LCD_ON );
    Display_1Mn_Buzzer( testTimer );
}

/**
 * @brief
 *
 *
 */
void test_buzzer_1mn_2( void )
{
    buzzer_flag             = 0;
    TimerHandle_t testTimer = { 0 };
    HAL_TIM_PWM_Stop_IgnoreAndReturn( pdPASS );
    xTimerGenericCommand_IgnoreAndReturn( pdPASS );
    TEST_ASSERT_EQUAL( 0, buzzer_flag );
    Display_1Mn_Buzzer( testTimer );
}