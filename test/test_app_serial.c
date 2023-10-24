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

STATIC uint8_t Validate_Date( uint8_t days, uint8_t month, uint16_t year );
STATIC uint8_t WeekDay( uint8_t days, uint8_t month, uint16_t year );

void setUp( void )
{
}

void tearDown( void )
{
}

/*Define queue for serial*/
QueueHandle_t serialQueue;

/* Define queue for clock */
QueueHandle_t clockQueue;

void test__Serial_Init__testInit( void )
{
    HAL_FDCAN_Init_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_ConfigFilter_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_ConfigGlobalFilter_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_Start_IgnoreAndReturn( HAL_OK );
    HAL_FDCAN_ActivateNotification_IgnoreAndReturn( HAL_OK );

    Serial_Init( );
}

void test__HAL_FDCAN_RxFifo0Callback__messageReceived( void )
{
    FDCAN_HandleTypeDef hfdcan = { 0 };

    HAL_FDCAN_GetRxMessage_IgnoreAndReturn( HAL_OK );
    xQueueGenericSendFromISR_IgnoreAndReturn( pdTRUE );

    HAL_FDCAN_RxFifo0Callback( &hfdcan, 1 );
}

void test__HAL_FDCAN_RxFifo0Callback__messageNOTReceived( void )
{
    FDCAN_HandleTypeDef hfdcan = { 0 };

    HAL_FDCAN_RxFifo0Callback( &hfdcan, 0 );
}

void test__Serial_Task__queueEmpty( void )
{
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );

    Serial_Task( );
}

void test__Serial_Task__queueMessage( void )
{
    /* NEW_MsgTypeDef RecieveMsg =
     {
         .Data[0] = 0x00,
         .Data[1] = 0x01,
         .Data[2] = 0x14,
         .Data[3] = 0x05,
         .Data[4] = 0x10,
         .Data[5] = 0x00,
         .Data[6] = 0x00,
         .Data[7] = 0x00,
     };*/

    // xQueueReceive_IgnoreAndReturn( pdPASS );
    // xQueueReceive_ExpectAndReturn( serialQueue, &RecieveMsg, 0, pdTRUE );
    // xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );
    // xQueueGenericSend_IgnoreAndReturn( pdPASS );

    // uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    // TEST_ASSERT_EQUAL_MESSAGE( 1, res, "CANtpRX ok" );

    Serial_Task( );
}


void test__BCD_conver__BCDTest( void )
{
    uint8_t data = 35, BCDdata = 0;

    BCDdata = BCD_conver( data );
    TEST_ASSERT_EQUAL_MESSAGE( 23, BCDdata, "Correct convertion" );
}

void test__Validate_Time__TimeMessage( void )
{
    uint8_t hour = 19, minutes = 43, seconds = 20, ret_val = 0;

    ret_val = Validate_Time( hour, minutes, seconds );
    TEST_ASSERT_EQUAL_MESSAGE( 1, ret_val, "Correct time" );
}
///////////////////////
//--------------------año visiesto
//--año visiesto con mes y dias correctos
void test__Validate_Date__leapYear( void )
{
    uint8_t days = 29, month = 2, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}
//--año visiesto, mes correcto, dia incorrecto
void test__Validate_Date__leapYearError( void )
{
    uint8_t days = 30, month = 2, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Fail Date" );
}
//-año no visiesto, febrero y dias correcto
void test__Validate_Date__februaryNormal( void )
{
    uint8_t days = 15, month = 2, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}
//-año visiesto pero mes equivocado
void test__Validate_Date__leapYearErrorMoreMonths( void )
{
    uint8_t days = 29, month = 1, res = 0;
    uint16_t year = 2020;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Fail Date" );
}
//-año no visiesto, mes febrero, dias mas de 28
void test__Validate_Date__leapYearErrorFebruaryMoreThan28( void )
{
    uint8_t days = 29, month = 2, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Fail Date" );
}

//-------------------meses con 30 dias
//-año correcto, mes con 30 dias
void test__Validate_Date__monthWith30Days( void )
{
    uint8_t days = 30, month = 4, res = 0;
    uint16_t year = 1999;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}
//-Mes correcto y error en dias
void test__Validate_Date__monthCorrectDayError( void )
{
    uint8_t days = 31, month = 4, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Fail Date" );
}

//-Mes correcto y error en dias
void test__Validate_Date__monthCorrectDayErrorMoreThan30( void )
{
    uint8_t days = 31, month = 6, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Fail Date" );
}

//-Mes correcto y error en dias
void test__Validate_Date__septemberDayErrorMoreThan30( void )
{
    uint8_t days = 31, month = 9, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Fail Date" );
}
//-Mes correcto y error en dias
void test__Validate_Date__novemberDayErrorMoreThan30( void )
{
    uint8_t days = 31, month = 11, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "Fail Date" );
}
//---------------------meses con 31 dias
//-año correcto, mes con 31 dias
void test__Validate_Date__monthWith31Days( void )
{
    uint8_t days = 26, month = 12, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

//-año correcto, mes con 31 dias
void test__Validate_Date__monthWith31Days1( void )
{
    uint8_t days = 26, month = 10, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}
//-año correcto, mes con 31 dias
void test__Validate_Date__monthWith31Days2( void )
{
    uint8_t days = 26, month = 8, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}
//-año correcto, mes con 31 dias
void test__Validate_Date__monthWith31Days3( void )
{
    uint8_t days = 26, month = 7, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

//-año correcto, mes con 31 dias
void test__Validate_Date__monthWith31Days4( void )
{
    uint8_t days = 26, month = 5, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

void test__Validate_Date__monthWith31Days5( void )
{
    uint8_t days = 26, month = 1, res = 0;
    uint16_t year = 2023;

    res = Validate_Date( days, month, year );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "Correct Date" );
}

////////
void test__CanTp_SingleFrameTx__FrameTXError( void )
{
    uint8_t msg_error[ 8 ] = { 0 };
    msg_error[ NUM_0 ]     = DATA_ERROR;

    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 9;

    CanTp_SingleFrameTx( msg_error, RecieveMsg.Data[ 0 ] );
}

void test__CanTp_SingleFrameTx__FrameTXErrorEmpty( void )
{
    uint8_t msg_error[ 8 ] = { 0 };
    msg_error[ NUM_0 ]     = DATA_ERROR;

    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 0;

    CanTp_SingleFrameTx( msg_error, RecieveMsg.Data[ 0 ] );
}

void test__CanTp_SingleFrameTx__FrameTXMessageOK( void )
{
    uint8_t msg_ok[ 8 ] = { 0 };
    msg_ok[ NUM_0 ]     = DATA_OK;

    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 5;

    HAL_FDCAN_AddMessageToTxFifoQ_IgnoreAndReturn( HAL_OK );
    CanTp_SingleFrameTx( msg_ok, RecieveMsg.Data[ 0 ] );
}
///////////
void test__CanTp_SingleFrameRx__SingleFrameMessage( void )
{
    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 5;

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "CANtpRX ok" );
}

void test__CanTp_SingleFrameRx__SingleFrameEmpty( void )
{
    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 133;

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "CANtpRX fail" );
}

void test__CanTp_SingleFrameRx__SingleFrameError( void )
{
    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[ 0 ]      = 9;

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "CANtpRX fail" );
}