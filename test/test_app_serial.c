#include "unity.h"
#include "app_serial.h"


#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_fdcan.h"
#include "mock_queue.h"

#include "bsp.h"


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

/*void test__HAL_FDCAN__RxFifo0Callback(void)
{

    //FDCAN_HandleTypeDef CANHandler;
    //FDCAN_HandleTypeDef *hfdcan;
    //hfdcan = &CANHandler;
   // uint32_t RxFifo0ITs = 1;

    //HAL_FDCAN_GetRxMessage_IgnoreAndReturn( HAL_OK);
    //xQueueGenericSend_IgnoreAndReturn( pdPASS );

    //HAL_FDCAN_RxFifo0Callback( hfdcan, RxFifo0ITs);
}*/