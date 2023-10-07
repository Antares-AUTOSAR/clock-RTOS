#include "unity.h"

#include "stm32g0xx.h"
#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_gpio.h"
#include "mock_stm32g0xx_hal_spi.h"
#include "mock_task.h"
#include "bsp.h"

#include "hel_lcd.h"

void setUp( void )
{
}

void tearDown( void )
{
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init_callFunction( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    
    


    HEL_LCD_Init( &lcdTestHandler );
}
