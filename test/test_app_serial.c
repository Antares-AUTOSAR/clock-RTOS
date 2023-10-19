#include "unity.h"
#include "app_serial.h"


#include "FreeRTOS.h"
#include "mock_stm32g0xx_hal_fdcan.h"
#include "mock_task.h"
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

void test__Serial_Task__queueOK(void)
{
    //xQueueReceive_IgnoreAndReturn( pdPASS );
    //xQueueGenericSend_IgnoreAndReturn( pdPASS );
    Serial_Task();
}