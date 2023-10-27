#include "unity.h"
#include "app_serial.h"
#include "bsp.h"

#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_fdcan.h"
#include "mock_task.h"
#include "mock_queue.h"

STATIC uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size );
STATIC void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
STATIC uint8_t BCD_conver( uint8_t data );

STATIC MACHINE_SERIAL Serial_StMachine( NEW_MsgTypeDef *pdata );
STATIC MACHINE_SERIAL SerialTimeState( NEW_MsgTypeDef *pmsg );
STATIC MACHINE_SERIAL SerialDateState( NEW_MsgTypeDef *pmsg );
STATIC MACHINE_SERIAL SerialAlarmState( NEW_MsgTypeDef *pmsg );
STATIC MACHINE_SERIAL SerialOkState( NEW_MsgTypeDef *pmsg );
STATIC MACHINE_SERIAL SerialErrorState( NEW_MsgTypeDef *pmsg );

STATIC uint8_t Validate_Date( uint8_t days, uint8_t month, uint16_t year );
STATIC uint8_t WeekDay( uint8_t days, uint8_t month, uint16_t year );
STATIC uint8_t Validate_Time( uint8_t hour, uint8_t minutes, uint8_t seconds );
STATIC uint8_t Validate_Alarm( uint8_t hour, uint8_t minutes );

/*Define queue for serial*/
QueueHandle_t serialQueue;

/* Define queue for clock */
QueueHandle_t clockQueue;

void setUp( void )
{
}

void tearDown( void )
{
}

/*Testing calling Serial_Init.
Invoke the function to configure with the CAN bus.
*/
void test__Serial_Init__testInit( void )
{
    HAL_FDCAN_Init_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_ConfigFilter_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_ConfigGlobalFilter_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_Start_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_ActivateNotification_IgnoreAndReturn( HAL_OK );

    Serial_Init( );
}

/*Testing calling HAL_FDCAN_RxFifo0Callback.
Invoke the function when the Message by CAN was received.
*/
void test__HAL_FDCAN_RxFifo0Callback__messageReceived( void )
{
    FDCAN_HandleTypeDef hfdcan = { 0 };

    HAL_FDCAN_GetRxMessage_IgnoreAndReturn( HAL_OK );
    xQueueGenericSendFromISR_IgnoreAndReturn( pdTRUE );

    HAL_FDCAN_RxFifo0Callback( &hfdcan, 1 );
}

/*Testing calling HAL_FDCAN_RxFifo0Callback.
Invoke the function when the Message by CAN was not received.
*/
void test__HAL_FDCAN_RxFifo0Callback__messageNOTReceived( void )
{
    FDCAN_HandleTypeDef hfdcan = { 0 };

    HAL_FDCAN_RxFifo0Callback( &hfdcan, 0 );
}

/*Testing calling Serial_Tak.
Invoke the function when the Message by CAN was received.
*/
/*void test__Serial_Task__queueEmpty( void )
{
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );

    Serial_Task( );
}*/
//////////////////////////////////////////

void test_Serial_Task_WithValidData( void )
{

    // FDCAN_HandleTypeDef hfdcan = { 0 };
    //  Crea un mensaje válido y colócalo en la cola para simular su recepción
    NEW_MsgTypeDef validMsg = { 0 };

    validMsg.Data[ 0 ] = 05;
    validMsg.Data[ 1 ] = 01;
    validMsg.Data[ 2 ] = 20;
    validMsg.Data[ 3 ] = 17;
    validMsg.Data[ 4 ] = 10;
    validMsg.Data[ 5 ] = 00;
    validMsg.Data[ 6 ] = 00;
    validMsg.Data[ 7 ] = 00;

    /*HAL_FDCAN_GetRxMessage_IgnoreAndReturn( HAL_OK );
    xQueueGenericSendFromISR_ExpectAndReturn( serialQueue, &validMsg.Data[ SINGLE_FRAME_ELEMENT ], NULL, pdFALSE,  pdTRUE );
    HAL_FDCAN_RxFifo0Callback( &hfdcan, 1 );*/

    // xQueueGenericSend_IgnoreAndReturn( pdPASS );

    // TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(serialQueue, &validMsg, 0));
    // xQueueCRSend_ExpectAndReturn(serialQueue, &validMsg, 0, pdPASS);
    // xQueueReceive_ExpectAndReturn(serialQueue, &validMsg, 0, pdPASS);
    // TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(serialQueue, &validMsg, 0));
    // xQueueReceive_IgnoreAndReturn( pdPASS );

    xQueueReceive_ExpectAndReturn( serialQueue, &validMsg, 0, pdPASS );
    // TEST_ASSERT_EQUAL_MESSAGE( pdPASS, res, "Correct time" );

    Serial_Task( );
}

/////////////////////////////////////
/*Testing calling Serial_StMachine.
Invoke the function when is necessary to validate the kind of message.
*/
/*void test__Serial_StMachine__machineTest( void )
{
    NEW_MsgTypeDef serialMSG = {
    .Data[ 0 ] = 1,
    .Data[ 1 ] = 12,
    .Data[ 2 ] = 54,
    .Data[ 3 ] = 10,
    };

    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    Serial_StMachine( &serialMSG );
}*/

/*void test__SerialTimeState__TimeStateTest( void )
{
    NEW_MsgTypeDef serialMSG = {
    .Data[ 0 ] = 1,
    .Data[ 1 ] = 12,
    .Data[ 2 ] = 54,
    .Data[ 3 ] = 10,
    };

    APP_MsgTypeDef messageStruct = { 0 };

    messageStruct.tm.tm_hour = BCD_conver( serialMSG.Data[ 1 ] );
    messageStruct.tm.tm_min  = BCD_conver( serialMSG.Data[ 2 ] );
    messageStruct.tm.tm_sec  = BCD_conver( serialMSG.Data[ 3 ] );*/

/*xQueueGenericSend_IgnoreAndReturn( pdPASS );

uint8_t hour = 12, minutes = 54, seconds = 10;

uint8_t ret_val = Validate_Time( hour, minutes , seconds );
TEST_ASSERT_EQUAL_MESSAGE( 1, ret_val, "Correct time" );

SerialTimeState( &serialMSG );
}*/

/*void test__SerialTimeState__TimeStateError( void )
{
    NEW_MsgTypeDef serialMSG = {
    .Data[ 0 ] = SERIAL_MSG_TIME,
    .Data[ 1 ] = 25,
    .Data[ 2 ] = 54,
    .Data[ 3 ] = 10,
    };

    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    uint8_t ret_val = Validate_Time(serialMSG.Data[1], serialMSG.Data[2], serialMSG.Data[3]);
    TEST_ASSERT_EQUAL_MESSAGE( 0, ret_val, "Incorrect time" );

    SerialTimeState( &serialMSG );
}*/

/*void test__SerialDateState__dateStateTest( void )
{
    NEW_MsgTypeDef serialMSG = {
    .Data[ 0 ] = 2,
    .Data[ 1 ] = 24,
    .Data[ 2 ] = 10,
    .Data[ 3 ] = 20,
    .Data[ 4 ] = 23,
    };

    APP_MsgTypeDef messageStruct = { 0 };

    messageStruct.tm.tm_mday = BCD_conver( serialMSG.Data[ 1 ] );
    messageStruct.tm.tm_mon   = BCD_conver( serialMSG.Data[ 2 ] );
    messageStruct.tm.tm_year   = ( ( BCD_conver( serialMSG.Data[ 3 ] ) * NUM_100 ) + BCD_conver( serialMSG.Data[ 4] ) );*/

/*xQueueGenericSend_IgnoreAndReturn( pdPASS );

uint8_t days = 29, month = 2, res = 0;
uint16_t year = 2020;

res = Validate_Date( days, month, year );
TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );

SerialDateState( &serialMSG );
}*/

/*Testing calling Serial_StMachine.
Invoke the function to verify the Alarm event when the queue has correct elements.
Returns the next event that will be executed.
*/
void test__SerialAlarmStatee__alarmEvent(void)
{
    NEW_MsgTypeDef msgRecieved;
    msgRecieved.Data[NUM_0] = 3;
    msgRecieved.Data[ALARM_HOUR_ELEMENT] = 0X14;
    msgRecieved.Data[ALARM_MIN_ELEMENT] = 0X37;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    MACHINE_SERIAL nextEvent = Serial_StMachine(&msgRecieved);
    TEST_ASSERT_EQUAL_MESSAGE(  SERIAL_OK, nextEvent, "Event state Alarm elements are corrected" );

}

/*Testing calling Serial_StMachine.
Invoke the function to verify the Alarm event when the queue has bad elements.
Returns the next event that will be executed.
*/
void test__SerialAlarmStatee__alarmEventError(void)
{
    NEW_MsgTypeDef msgRecieved;
    msgRecieved.Data[NUM_0] = 3;
    msgRecieved.Data[ALARM_HOUR_ELEMENT] = 0X23;
    msgRecieved.Data[ALARM_MIN_ELEMENT] = 0X62;

    xQueueGenericSend_IgnoreAndReturn( pdPASS );

    MACHINE_SERIAL nextEvent = Serial_StMachine(&msgRecieved);
    TEST_ASSERT_EQUAL_MESSAGE(  SERIAL_ERROR, nextEvent, "Event state Alarm elemnts are bad" );

}

/*Testing calling Serial_StMachine.
Invoke the function to verify the ERROR event.
Returns the next event that will be executed.
*/
void test__SerialErrorState__errorEvent(void)
{
    NEW_MsgTypeDef msgRecieved;
    msgRecieved.Data[NUM_0] = ERROR_STATE;

    HAL_FDCAN_AddMessageToTxFifoQ_IgnoreAndReturn( HAL_OK );

    MACHINE_SERIAL nextEvent = Serial_StMachine(&msgRecieved);
    TEST_ASSERT_EQUAL_MESSAGE(  SERIAL_IDLE, nextEvent, "Event state ERROR complete" );

}

/*Testing calling Serial_StMachine.
Invoke the function to verify the OK event.
Returns the next event that will be executed.
*/
void test__SerialOkState__OkEvent(void)
{
    NEW_MsgTypeDef msgRecieved;
    msgRecieved.Data[NUM_0] = OK_STATE;

    HAL_FDCAN_AddMessageToTxFifoQ_IgnoreAndReturn( HAL_OK );

    MACHINE_SERIAL nextEvent = Serial_StMachine(&msgRecieved);
    TEST_ASSERT_EQUAL_MESSAGE(  SERIAL_IDLE, nextEvent, "Event state OK complete" );

}

/*Testing calling BCD_conver.
Invoke the function when is necessary to convert data to Decimal.
Returns the expected data.
*/
void test__BCD_conver__BCDTest( void )
{
    uint8_t data = 35, BCDdata = 0;

    BCDdata = BCD_conver( data );
    TEST_ASSERT_EQUAL_MESSAGE( 23, BCDdata, "Correct convertion" );
}

/*Testing calling Validate_Time.
Invoke the function when is necessary to verify if the data are correct.
Returns 1 in case that the data were correct.
*/
void test__Validate_Time__TimeMessage( void )
{
    uint8_t hour = 19, minutes = 43, seconds = 20, ret_val = 0;

    ret_val = Validate_Time( hour, minutes, seconds );
    TEST_ASSERT_EQUAL_MESSAGE( 1, ret_val, "Correct time" );
}

/*Testing calling Validate_Time.
Invoke the function when is necessary to verify if the data are correct.
Returns 0 in case that the data of hours were incorrect.
*/
void test__Validate_Time__TimeErrorHours( void )
{
    uint8_t hour = 25, minutes = 43, seconds = 10, ret_val = 0;

    ret_val = Validate_Time( hour, minutes, seconds );
    TEST_ASSERT_EQUAL_MESSAGE( 0, ret_val, "Incorrect time" );
}

/*Testing calling Validate_Time.
Invoke the function when is necessary to verify if the data are correct.
Returns 0 in case that the data of minutes were incorrect.
*/
void test__Validate_Time__TimeErrorMinutes( void )
{
    uint8_t hour = 19, minutes = 62, seconds = 20, ret_val = 0;

    ret_val = Validate_Time( hour, minutes, seconds );
    TEST_ASSERT_EQUAL_MESSAGE( 0, ret_val, "Incorrect time" );
}

/*Testing calling Validate_Time.
Invoke the function when is necessary to verify if the data are correct.
Returns 0 in case that the data of seconds were incorrect.
*/
void test__Validate_Time__TimeErrorSeconds( void )
{
    uint8_t hour = 19, minutes = 43, seconds = 65, ret_val = 0;

    ret_val = Validate_Time( hour, minutes, seconds );
    TEST_ASSERT_EQUAL_MESSAGE( 0, ret_val, "Incorrect time" );
}

/*Testing calling Validate_Alarm.
Invoke the function when is necessary to verify if the data are correct.
Returns 1 in case that the data elements were correct.
*/
void test__Validate_Alarm__alarmMessage( void )
{
    uint8_t hour = 23, minutes = 11, ret_val = 0;

    ret_val = Validate_Alarm( hour, minutes );
    TEST_ASSERT_EQUAL_MESSAGE( 1, ret_val, "Correct time" );
}

/*Testing calling Validate_Alarm.
Invoke the function when is necessary to verify if the data are correct.
Returns 0 in case that the data elements were incorrect.
*/
void test__Validate_Alarm__alarmErrorHour( void )
{
    uint8_t hour = 25, minutes = 11, ret_val = 0;

    ret_val = Validate_Alarm( hour, minutes );
    TEST_ASSERT_EQUAL_MESSAGE( 0, ret_val, "Incorrect time" );
}

/*Testing calling Validate_Alarm.
Invoke the function when is necessary to verify if the data are correct.
Returns 0 in case that the data elements were incorrect.
*/
void test__Validate_Alarm__alarmErrorMinutes( void )
{
    uint8_t hour = 23, minutes = 61, ret_val = 0;

    ret_val = Validate_Alarm( hour, minutes );
    TEST_ASSERT_EQUAL_MESSAGE( 0, ret_val, "Incorrect time" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a leap year with month and days correct.
Returns 1 in case that the data elements of leap year were correct.
*/
void test__Validate_Date__leapYear( void )
{
    uint8_t days = 29, month = 2, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Not leap year, days and month corrects).
*/
void test__Validate_Date__februaryNormal( void )
{
    uint8_t days = 15, month = 2, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year, days and month corrects).
*/
void test__Validate_Date__monthWith30Days( void )
{
    uint8_t days = 30, month = 4, res = 0;
    uint16_t year = 1999;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year, month and days corrects).
*/
void test__Validate_Date__monthWith31Days( void )
{
    uint8_t days = 26, month = 12, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year, month and days corrects).
*/
void test__Validate_Date__monthWith31DaysOK( void )
{
    uint8_t days = 26, month = 10, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year, month and days corrects).
*/
void test__Validate_Date__monthWith31DaysMessage( void )
{
    uint8_t days = 26, month = 8, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year, month and days corrects).
*/
void test__Validate_Date__monthWith31DaysMessOK( void )
{
    uint8_t days = 26, month = 7, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

void test__Validate_Date__monthWith31DaysOki( void )
{
    uint8_t days = 26, month = 3, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}
/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year, month and days corrects).
*/
void test__Validate_Date__monthWith31DaysOk( void )
{
    uint8_t days = 26, month = 5, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year, month and days corrects).
*/
void test__Validate_Date__monthWith31DaysOkMessage( void )
{
    uint8_t days = 26, month = 1, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (days greater than 31).
*/
void test__Validate_Date__daysError( void )
{
    uint8_t days = 32, month = 2, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (days less than 1).
*/
void test__Validate_Date__daysErrorZero( void )
{
    uint8_t days = 0, month = 2, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (month greater than 12).
*/
void test__Validate_Date__monthError( void )
{
    uint8_t days = 30, month = 13, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (month less than 1).
*/
void test__Validate_Date__monthErrorZero( void )
{
    uint8_t days = 30, month = 0, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (year greater than 2099).
*/
void test__Validate_Date__yearError( void )
{
    uint8_t days = 30, month = 12, res = 0;
    uint16_t year = 2100;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (year less than 1901).
*/
void test__Validate_Date__yearErrorZero( void )
{
    uint8_t days = 30, month = 12, res = 0;
    uint16_t year = 1900;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a leap year.
Returns 0 in case that the data elements of date are incorrect (Leap year, days greater than 29 in February).
*/
void test__Validate_Date__leapYearError( void )
{
    uint8_t days = 30, month = 2, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (leap year and days corrects but month incorrect).
*/
void test__Validate_Date__leapYearErrorMoreMonths( void )
{
    uint8_t days = 29, month = 1, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Not leap year, days greater than 28 in February ).
*/
void test__Validate_Date__leapYearErrorFebruaryMoreThan28( void )
{
    uint8_t days = 29, month = 2, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year and month corrects, days incorrect for the month).
*/
void test__Validate_Date__monthDayError( void )
{
    uint8_t days = 31, month = 4, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year and month corrects, days incorrect for the month).
*/
void test__Validate_Date__monthDayErrorMoreThan30( void )
{
    uint8_t days = 31, month = 6, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year and month corrects, days incorrect for the month).
*/
void test__Validate_Date__septemberDayErrorMoreThan30( void )
{
    uint8_t days = 31, month = 9, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling Validate_Date.
Invoke the function when is necessary to verify the elemts of a date.
Returns 0 in case that the data elements of date are incorrect (Year and month corrects, days incorrect for the month).
*/
void test__Validate_Date__novemberDayErrorMoreThan30( void )
{
    uint8_t days = 31, month = 11, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Incorrect Date" );
}

/*Testing calling WeekDay.
Invoke the function to describe what is the weekday of the date.
Return from 0 to 6 (Sunday-Saturday) to corresponding of date.
*/
void test__WeekDay__weekdayCorrect( void )
{
    uint8_t days = 23, month = 10, res = 0;
    uint16_t year = 2023;

    res = WeekDay( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct weekday" );
}

/*Testing calling CanTp_SingleFrameTx.
Invoke the function when is necessary to responce if the CAN data elements were correct or incorrect.
Error the single frame is bigger than 7.
*/
void test__CanTp_SingleFrameTx__FrameTXError( void )
{
    uint8_t msg_error[ 8 ] = { 0 };
    msg_error[ NUM_0 ]     = DATA_ERROR;

    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 9;

    CanTp_SingleFrameTx( msg_error, RecieveMsg.Data[ 0 ] );
}

/*Testing calling CanTp_SingleFrameTx.
Invoke the function when is necessary to responce if the CAN data elements were correct or incorrect.
Error the single frame is smaller than 1.
*/
void test__CanTp_SingleFrameTx__FrameTXErrorEmpty( void )
{
    uint8_t msg_error[ 8 ] = { 0 };
    msg_error[ NUM_0 ]     = DATA_ERROR;

    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 0;

    CanTp_SingleFrameTx( msg_error, RecieveMsg.Data[ 0 ] );
}

/*Testing calling CanTp_SingleFrameTx.
Invoke the function when is necessary to responce if the CAN data elements were correct or incorrect.
Single frame element is correct.
*/
void test__CanTp_SingleFrameTx__FrameTXMessage( void )
{
    uint8_t msg_ok[ 8 ] = { 0 };
    msg_ok[ NUM_0 ]     = DATA_OK;

    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 5;

    HAL_FDCAN_AddMessageToTxFifoQ_IgnoreAndReturn( HAL_OK );
    CanTp_SingleFrameTx( msg_ok, RecieveMsg.Data[ 0 ] );
}

/*Testing calling CanTp_SingleFrameRx.
Invoke the function to validate the payload.
Return 1 if the single frame elemnt was correct.
*/
void test__CanTp_SingleFrameRx__SingleFrameMessage( void )
{
    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 5;

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "CANtpRX ok" );
}

/*Testing calling CanTp_SingleFrameRx.
Invoke the function to validate the payload.
Return 0 if the single frame elemnt was correct.
*/
void test__CanTp_SingleFrameRx__SingleFrameEmpty( void )
{
    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 133;

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "CANtpRX fail" );
}

/*Testing calling CanTp_SingleFrameRx.
Invoke the function to validate the payload.
Return 0 if the single frame elemnt was correct.
*/
void test__CanTp_SingleFrameRx__SingleFrameError( void )
{
    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 9;

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "CANtpRX fail" );
}