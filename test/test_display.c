#include "bsp.h"
#include "app_display.h"
#include "hel_lcd.h"
#include "unity.h"
#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_spi.h"
#include "mock_queue.h"
#include "mock_hel_lcd.h"

QueueHandle_t displayQueue;

extern SPI_HandleTypeDef SpiHandle;

extern LCD_HandleTypeDef hlcd;

STATIC char *get_month( uint8_t month );
STATIC Display_M Display_Machine( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Date( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Time( APP_MsgTypeDef *DisplayMsg );

void setUp( void )
{
}

void tearDown( void )
{
}

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

void test_getMonth_invalid( void )
{
    TEST_ASSERT_NULL( get_month( 0 ) );
    TEST_ASSERT_NULL( get_month( 13 ) );
}

void test_Weekday_Abbreviation( void )
{
    const char *abbreviations[] =
    {
    "SU", "MO", "TU", "WE", "TH", "FR", "SA" };

    TEST_ASSERT_EQUAL_STRING( "SU", abbreviations[ 0 ] );
    TEST_ASSERT_EQUAL_STRING( "MO", abbreviations[ 1 ] );
    TEST_ASSERT_EQUAL_STRING( "TU", abbreviations[ 2 ] );
    TEST_ASSERT_EQUAL_STRING( "WE", abbreviations[ 3 ] );
    TEST_ASSERT_EQUAL_STRING( "TH", abbreviations[ 4 ] );
    TEST_ASSERT_EQUAL_STRING( "FR", abbreviations[ 5 ] );
    TEST_ASSERT_EQUAL_STRING( "SA", abbreviations[ 6 ] );
}

void test_Display_Init( void )
{
    HAL_SPI_Init_ExpectAndReturn( &SpiHandle, HAL_OK );
    HEL_LCD_Init_ExpectAndReturn( &hlcd, HAL_OK );
    HEL_LCD_Backlight_Expect( &hlcd, LCD_ON );
    HEL_LCD_Contrast_ExpectAndReturn( &hlcd, 15, HAL_OK );

    Display_Init( );
}

void test_Display_Task_emptyQueue( void )
{
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );
    Display_Task( );
}

void test_DisplayTask_messageQueue( void )
{
    char string[] = "00:00:00";

    xQueueReceive_IgnoreAndReturn( pdPASS );
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );
    HEL_LCD_SetCursor_ExpectAndReturn( &hlcd, 1, 3, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &hlcd, string, HAL_OK );
    xQueueGenericSend_IgnoreAndReturn( pdPASS );
    Display_Task( );
}

void test_DisplayEventMachine_SendDate( void )
{
    char string[] = "00:00:00";

    HEL_LCD_SetCursor_ExpectAndReturn( &hlcd, 1, 3, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &hlcd, string, HAL_OK );
    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    APP_MsgTypeDef message = { 0 };
    Display_M state;

    message.msg = DISPLAY_MSG_DATE;

    state = Display_Machine( &message );

    TEST_ASSERT_EQUAL_MESSAGE( DISPLAY_MSG_DATE, state, "Execution of a DISPLAY_TIME must return a DISPLAY_MSG_DATE(0)" );
}

void test_DisplayEventMachine_Idle( void )
{
}