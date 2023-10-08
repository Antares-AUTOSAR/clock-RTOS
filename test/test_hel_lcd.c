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
void test__HEL_LCD_Init__All_Commands_Successful( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK ); // All calls will retun HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A successful execution returns 0 value (HAL_OK)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_WakeUp1_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // Wakeup1
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_WakeUp2_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // Wakeup2
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_WakeUp3_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // Wakeup3
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_FunctionSet_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // FunctionSet
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_OscFreq_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FunctionSet
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // OscFreq
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_PowerControl_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FunctionSet
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // OscFreq
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // PowerControl
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_FollowerControl_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FunctionSet
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // OscFreq
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // PowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // FollowerControl
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_Contrast_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FunctionSet
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // OscFreq
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // PowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FollowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // Contrast
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_DisplayOn_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FunctionSet
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // OscFreq
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // PowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FollowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Contrast
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // DisplayOn
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_EntryMode_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FunctionSet
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // OscFreq
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // PowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FollowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Contrast
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // DisplayOn
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // EntryMode
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Init__Command_ClearScreen_Error( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );
    vTaskDelay_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup1
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup2
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Wakeup3
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FunctionSet
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // OscFreq
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // PowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // FollowerControl
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // Contrast
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // DisplayOn
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );    // EntryMode
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR ); // ClearScreen
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );                                                      // The rest of calls return HAL_OK
    returnedValue = HEL_LCD_Init( &lcdTestHandler );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any error after sending a command returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_MspInit__InvokeFunction( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HEL_LCD_MspInit( &lcdTestHandler );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Command__SuccessfulTransmit( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_OK );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Command( &lcdTestHandler, cmdTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A successful command transmission returns 0 value (HAL_OK)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Command__FailedTransmit( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t cmdTest                  = 0;

    HAL_GPIO_WritePin_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &cmdTest, sizeof( cmdTest ), 1u, HAL_ERROR );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Command( &lcdTestHandler, cmdTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "A failed command transmission returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Data__SuccessfulTransmit( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t dataTest                 = 0;

    HAL_GPIO_WritePin_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_OK );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Data( &lcdTestHandler, dataTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A successful command transmission returns 0 value (HAL_OK)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Data__FailedTransmit( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t dataTest                 = 0;

    HAL_GPIO_WritePin_Ignore( );

    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_ERROR );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Data( &lcdTestHandler, dataTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "A failed command transmission returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_String__SuccesfulTransmit( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    char stringTest[ 11 ]            = "stringTest\0";

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_String( &lcdTestHandler, stringTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A failed command transmission returns 0 value (HAL_OK)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_String__FirstCharTransmitError( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t dataTest                 = 0;
    char stringTest[ 7 ]             = "string\0";

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_ERROR );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_String( &lcdTestHandler, stringTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any failed char transmission returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_String__LastCharTransmitError( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    SPI_HandleTypeDef spiTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    uint8_t dataTest                 = 0;
    char stringTest[ 7 ]             = "string\0";

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_OK );    // s
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_OK );    // t
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_OK );    // r
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_OK );    // i
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_OK );    // n
    HAL_SPI_Transmit_ExpectAndReturn( &spiTestHandler, &dataTest, sizeof( dataTest ), 1u, HAL_ERROR ); // g
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_String( &lcdTestHandler, stringTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "Any failed char transmission returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_SetCursor__SuccessfulOperation( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_SetCursor( &lcdTestHandler, 0u, 12u );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A successful set cursor operation returns 0 value (HAL_OK)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_SetCursor__InvalidRowValue( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_SetCursor( &lcdTestHandler, 2u, 12u );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "A failed set cursor operation returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_SetCursor__InvalidColValue( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_SetCursor( &lcdTestHandler, 1u, 16u );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "A failed set cursor operation returns 1 value (HAL_ERROR)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_BackLight__SetOFF( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HAL_GPIO_WritePin_Ignore( );

    HEL_LCD_Backlight( &lcdTestHandler, LCD_OFF );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_BackLight__SetON( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HAL_GPIO_WritePin_Ignore( );

    HEL_LCD_Backlight( &lcdTestHandler, LCD_ON );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_BackLight__SetTOGGLE( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HAL_GPIO_TogglePin_Ignore( );

    HEL_LCD_Backlight( &lcdTestHandler, LCD_TOGGLE );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Contrast__SuccessfulOperation( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Contrast( &lcdTestHandler, 32u );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A successful set contrast operation returns 0 value (HAL_OK)" );
}


/* Test calling for Clock_Init function
    Ignore RTC functions assuming that will return a HAL_OK value when are invoked
    Ignore timer creation assuming that will return a valid timer handler allocated
    Ignore other two functions related with timer start which return 1u value and pdPASS
*/
void test__HEL_LCD_Contrast__InvalidValue( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Contrast( &lcdTestHandler, 65u );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "A failed set contrast operation returns 1 value (HAL_ERROR)" );
}