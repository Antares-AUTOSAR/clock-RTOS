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


/* Test calling for HEL_LCD_Init function without any errors
    Ignore GPIO functions
    Ignore RTOS delays
    Ignore SPI functions assuming that all return HAL_OK value
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


/* Test calling for HEL_LCD_Init function with transmission error in first command (wakeup command)
    Ignore GPIO functions
    Ignore RTOS delays
    Expect the first time a SPI command is trasmitted and return HAL_ERROR
    Ignore the rest of SPI functions assuming that all return HAL_OK value
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


/* Test calling for HEL_LCD_Init function with transmission error in second command (wakeup command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the second time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in third command (wakeup command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the third time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 4th command (function set command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 4th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 5th command (oscillator frequency command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 5th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 6th command (power control command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 6th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 7th command (follower control command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 7th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 8th command (contrast command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 8th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 9th command (display on command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 9th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 10th command (entry mode command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 10th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_Init function with transmission error in 11th command (clear screen command)
    Ignore GPIO functions
    Ignore RTOS delays
    Only expect the 11th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_MspInit function
    Just invoke the function
*/
void test__HEL_LCD_MspInit__InvokeFunction( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HEL_LCD_MspInit( &lcdTestHandler );
}


/* Test calling for HEL_LCD_Command function without any transmission errors
    Ignore GPIO functions
    Expect the SPI command trasmission returns HAL_OK
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


/* Test calling for HEL_LCD_Command function with transmission error
    Ignore GPIO functions
    Expect the SPI command trasmission returns HAL_ERROR
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


/* Test calling for HEL_LCD_Data function without any transmission error
    Ignore GPIO functions
    Expect the SPI command trasmission returns HAL_OK
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


/* Test calling for HEL_LCD_Data function with transmission error
    Ignore GPIO functions
    Expect the SPI command trasmission returns HAL_ERROR
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


/* Test calling for HEL_LCD_String function without any transmission error
    Ignore GPIO functions
    Assume that all the SPI commands trasmission returns HAL_OK
*/
void test__HEL_LCD_String__SuccesfulTransmit( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;
    char stringTest[ 11 ]            = "stringTest\0";

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_String( &lcdTestHandler, stringTest );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A successful command transmission returns 0 value (HAL_OK)" );
}


/* Test calling for HEL_LCD_String function with transmission error in first string character
    Ignore GPIO functions
    Only expect the first time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_String function with transmission error in first string character
    Ignore GPIO functions
    Only expect the 6th time a SPI command is trasmitted returns HAL_ERROR
    The rest of SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_SetCursor function without any transmission error
    Ignore GPIO functions
    All the SPI transmissions return HAL_OK
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


/* Test calling for HEL_LCD_SetCursor function with invalid row
    Ignore GPIO functions
    Expect the SPI command transmission returns HAL_OK
    Using an invalid row value (2 and must be 0 or 1)
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


/* Test calling for HEL_LCD_SetCursor function with invalid column
    Ignore GPIO functions
    Expect the SPI command transmission returns HAL_OK
    Using an invalid col value (16 and must be from 0 to 15)
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


/* Test calling for HEL_LCD_Backlight function
    Ignore GPIO functions
    Just invoke the function
*/
void test__HEL_LCD_BackLight__SetOFF( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HAL_GPIO_WritePin_Ignore( );

    HEL_LCD_Backlight( &lcdTestHandler, LCD_OFF );
}


/* Test calling for HEL_LCD_Backlight function
    Ignore GPIO functions
    Just invoke the function
*/
void test__HEL_LCD_BackLight__SetON( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HAL_GPIO_WritePin_Ignore( );

    HEL_LCD_Backlight( &lcdTestHandler, LCD_ON );
}


/* Test calling for HEL_LCD_Backlight function
    Ignore GPIO functions
    Just invoke the function
*/
void test__HEL_LCD_BackLight__SetTOGGLE( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };

    HAL_GPIO_TogglePin_Ignore( );

    HEL_LCD_Backlight( &lcdTestHandler, LCD_TOGGLE );
}


/* Test calling for HEL_LCD_Contrast function without errors
    Ignore GPIO functions
    Expect the SPI command transmission returns HAL_OK
    Using an valid contrast value (32 and must be from 0 to 64)
*/
void test__HEL_LCD_Contrast__SuccessfulOperation( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Contrast( &lcdTestHandler, 32u );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_OK, returnedValue, "A successful set contrast operation returns 0 value (HAL_OK)" );
}


/* Test calling for HEL_LCD_Contrast function with invaid value
    Ignore GPIO functions
    Expect the SPI command transmission returns HAL_OK
    Using an valid contrast value (65 and must be from 0 to 64)
*/
void test__HEL_LCD_Contrast__InvalidValue( void )
{
    LCD_HandleTypeDef lcdTestHandler = { 0 };
    HAL_StatusTypeDef returnedValue  = HAL_ERROR;

    HAL_GPIO_WritePin_Ignore( );
    HAL_SPI_Transmit_IgnoreAndReturn( HAL_OK );

    returnedValue = HEL_LCD_Contrast( &lcdTestHandler, 65u );

    TEST_ASSERT_EQUAL_MESSAGE( HAL_ERROR, returnedValue, "A failed set contrast operation returns 1 value (HAL_ERROR)" );
}