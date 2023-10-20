#include "unity.h"
#include "app_serial.h"
#include "bsp.h"

#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_fdcan.h"
#include "mock_task.h"
#include "mock_queue.h"

STATIC uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size );

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

void test__HAL_FDCAN_RxFifo0Callback__messageReceived(void)
{
    FDCAN_HandleTypeDef hfdcan= {0};
    
    HAL_FDCAN_GetRxMessage_IgnoreAndReturn( HAL_OK );
    xQueueGenericSendFromISR_IgnoreAndReturn( pdTRUE );
    
    HAL_FDCAN_RxFifo0Callback( &hfdcan, 1);
}

void test__HAL_FDCAN_RxFifo0Callback__messageNOTReceived(void)
{
    FDCAN_HandleTypeDef hfdcan= {0};
    
    HAL_FDCAN_RxFifo0Callback( &hfdcan, 0);
}

void test__Serial_Task__queueEmpty(void)
{
    xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );

    Serial_Task( );
}

void test__Serial_Task__queueMessage(void)
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

    xQueueReceive_IgnoreAndReturn( pdPASS );
    //xQueueReceive_ExpectAndReturn( serialQueue, &RecieveMsg, 0, pdTRUE );
    //xQueueReceive_IgnoreAndReturn( errQUEUE_EMPTY );
    //xQueueGenericSend_IgnoreAndReturn( pdPASS );

    //uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    //TEST_ASSERT_EQUAL_MESSAGE( 1, res, "CANtpRX ok" );

    Serial_Task( );
}
//////////////////
void test__CanTp_SingleFrameRx__SingleFrameMessage(void)
{
    NEW_MsgTypeDef RecieveMsg = { 0 };

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 1, res, "CANtpRX ok" );

}

void test__CanTp_SingleFrameRx__SingleFrameEmpty(void)
{
    NEW_MsgTypeDef RecieveMsg = { 0 };
    RecieveMsg.Data[0] = 0x18;

    uint8_t res = CanTp_SingleFrameRx( RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] );
    TEST_ASSERT_EQUAL_MESSAGE( 0, res, "CANtpRX fail" );

}