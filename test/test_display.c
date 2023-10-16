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
STATIC void TimeString( char *string, uint8_t hours, uint8_t minutes, uint8_t seconds );
STATIC void DateString( char *string, uint8_t month, uint8_t day, uint16_t year, uint8_t weekday );
STATIC Display_M Display_Machine( APP_MsgTypeDef *DisplayMsg );
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

void test_time_string( void )
{
    char string[] = "00:00:00";

    TimeString( string, 12, 33, 22 );

    TEST_ASSERT_EQUAL_STRING( "12:33:22", string );
}

void test_date_string( void )
{
    char date_string[] = "000,00 0000 00"; /* cppcheck-suppress misra-c2012-7.4  ;Array to print date*/

    DateString( date_string, 04, 12, 2010, 01 );

    TEST_ASSERT_EQUAL_STRING( "APR,12 2010 MO", date_string );
}

void test_Display_Init( void )
{
    HAL_SPI_Init_ExpectAndReturn( &SpiHandle, HAL_OK );
    HEL_LCD_Init_ExpectAndReturn( &hlcd, HAL_OK );
    HEL_LCD_Backlight_Expect( &hlcd, LCD_ON );
    HEL_LCD_Contrast_ExpectAndReturn( &hlcd, 15, HAL_OK );

    Display_Init( );
}

void test_Display_Task_QueueEmpty( void )
{
    xQueueReceive_IgnoreAndReturn( pdFAIL );
    Display_Task( );
}

/*
void test_Display_Task_ms( void )
{
   
    //APP_MsgTypeDef DisplayMsg = {DISPLAY_MSG_TIME};

    //xQueueReceive_ExpectAndReturn(displayQueue,&DisplayMsg,0,pdFAIL);
    
    //xQueueReceive_IgnoreAndReturn( pdPASS );
    Display_Task( );
}
*/

void test_display_machine(void)
{
    
}


void test_Time( void )
{
    char string[]      = "12:33:22";
    char date_string[] = "APR,12 2010 MO";

    HEL_LCD_SetCursor_ExpectAndReturn( &hlcd, 1, 3, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &hlcd, string, HAL_OK );

    HEL_LCD_SetCursor_ExpectAndReturn( &hlcd, 0, 1, HAL_OK );
    HEL_LCD_String_ExpectAndReturn( &hlcd, date_string, HAL_OK );
    // Crea una estructura de mensaje simulada con datos de tiempo
    APP_MsgTypeDef DisplayMsg = {
    .msg = DISPLAY_MSG_TIME,
    .tm  = {
     .tm_hour = 12,
     .tm_min  = 33,
     .tm_sec  = 22,
     .tm_mon  = 4,
     .tm_mday = 12,
     .tm_year = 2010,
     .tm_wday = 1 } };


    // Ejecuta la función Time
    Display_M result = Time( &DisplayMsg );


    // Verifica el resultado y otros aspectos si es necesario
    TEST_ASSERT_EQUAL_MESSAGE( DISPLAY_IDLE_STATE, result, "Execution of a DISPLAY_MSG_TIME must return a DISPLAY_IDLE_STATE (0)" );
}
