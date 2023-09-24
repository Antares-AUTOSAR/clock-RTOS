#include "unity.h"
#include "app_clock.h"

#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_rtc.h"
#include "mock_stm32g0xx_hal_rtc_ex.h"
#include "mock_timers.h"
#include "mock_task.h"
#include "mock_queue.h"

void setUp( void )
{
    mock_stm32g0xx_hal_rtc_Init( );
    mock_stm32g0xx_hal_rtc_ex_Init( );
}

void tearDown( void )
{
}

void test__Clock_Init( void )
{
    int res;

    res = 16;

    TEST_ASSERT_EQUAL_MESSAGE( 16, res, "Ret value must be 16" );
}
