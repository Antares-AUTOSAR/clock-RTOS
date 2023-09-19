/**
 * @file    main.c
 *
 */
#include "bsp.h"
#include "app_clock.h"

/*Prototype of static memory*/
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );
/*Prototype of timer in static memory*/
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize );

/**
 * @brief Save the buffer dimentions
 */
#define TASK_STACK_SIZE      128u

/**
 * @defgroup Queue parameters
 @{*/
#define QUEUE_LENGTH_SERIAL  9                        /*!< Maximum number of items */
#define ITEM_SIZE_SERIAL     sizeof( NEW_MsgTypeDef ) /*!< The size, in bytes, required to hold each item in the queue. */
#define QUEUE_LENGTH_CLOCK   45                       /*!< Maximum number of items */
#define ITEM_SIZE_CLOCK      sizeof( APP_MsgTypeDef ) /*!< The size, in bytes, required to hold each item in the queue. */
#define QUEUE_LENGTH_DISPLAY 90                       /*!< Maximum number of items */
#define ITEM_SIZE_DISPLAY    sizeof( APP_MsgTypeDef ) /*!< The size, in bytes, required to hold each item in the queue. */
/**@} */

/**
 * @brief Serial Queue identifier to use
 */
QueueHandle_t serialQueue = { 0 };
/**
 * @brief Serial Queue identifier to use
 */
QueueHandle_t clockQueue;
/**
 * @brief Serial Queue identifier to use
 */
QueueHandle_t displayQueue = { 0 };

/*Task to developed*/
static void Task_10ms( void *parameters );
static void Task_50ms( void *parameters );
static void Task_100ms( void *parameters );

/**
 * @brief The initial function, code development.
 * @retval Returns an integer
 */
int main( void )
{
    /*Enable RTT and system view*/
    HAL_Init( );
    SEGGER_SYSVIEW_Conf( );
    SEGGER_SYSVIEW_Start( );

    /*Declaration of task control structures for the static memory*/
    static StaticTask_t xTaskBuffer_10ms;
    static StaticTask_t xTaskBuffer_50ms;
    static StaticTask_t xTaskBuffer_100ms;

    static StackType_t xTaskStack_10ms[ TASK_STACK_SIZE ];
    static StackType_t xTaskStack_50ms[ TASK_STACK_SIZE ];
    static StackType_t xTaskStack_100ms[ TASK_STACK_SIZE ];

    /*Periodicity of each task*/
    uint8_t serialPeriod  = 10;
    uint8_t clockPeriod   = 50;
    uint8_t displayPeriod = 100;

    /*QUEUE ELEMENTS*/

    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xStaticQueue;
    static StaticQueue_t xStaticQueueClock;
    static StaticQueue_t xStaticQueueDisplay;
    /* The array to use as the queue's storage area.  This must be at least
    uxQueueLength * uxItemSize bytes. */
    uint8_t serialStorageArea[ QUEUE_LENGTH_SERIAL * ITEM_SIZE_SERIAL ];    /*cppcheck-suppress misra-c2012-10.4 ; According of freeRTOS is necessary*/
    uint8_t clockStorageArea[ QUEUE_LENGTH_CLOCK * ITEM_SIZE_CLOCK ];       /*cppcheck-suppress misra-c2012-10.4 ; According of freeRTOS is necessary*/
    uint8_t displayStorageArea[ QUEUE_LENGTH_DISPLAY * ITEM_SIZE_DISPLAY ]; /*cppcheck-suppress misra-c2012-10.4 ; According of freeRTOS is necessary*/

    /*Queue create*/
    serialQueue  = xQueueCreateStatic( QUEUE_LENGTH_SERIAL, ITEM_SIZE_SERIAL, serialStorageArea, &xStaticQueue );
    clockQueue   = xQueueCreateStatic( QUEUE_LENGTH_CLOCK, ITEM_SIZE_CLOCK, clockStorageArea, &xStaticQueueClock );
    displayQueue = xQueueCreateStatic( QUEUE_LENGTH_DISPLAY, ITEM_SIZE_DISPLAY, displayStorageArea, &xStaticQueueDisplay );


    /*Creation of tasks with different periodicities using static memory*/
    xTaskCreateStatic( Task_10ms, "Task10ms", TASK_STACK_SIZE, &serialPeriod, 3u, xTaskStack_10ms, &xTaskBuffer_10ms );
    xTaskCreateStatic( Task_50ms, "Task50ms", TASK_STACK_SIZE, &clockPeriod, 2u, xTaskStack_50ms, &xTaskBuffer_50ms );
    xTaskCreateStatic( Task_100ms, "Task100ms", TASK_STACK_SIZE, &displayPeriod, 1u, xTaskStack_100ms, &xTaskBuffer_100ms );

    vTaskStartScheduler( );
    return 0;
}

/**
 * @brief Task 10ms
 * @param parameters
 */
static void Task_10ms( void *parameters )
{
    TickType_t xLastWakeTime  = xTaskGetTickCount( );
    uint8_t serialPeriodicity = *( (uint8_t *)parameters ); /*cppcheck-suppress misra-c2012-11.5 ; Take the value of function parameter*/

    for( ;; )
    {
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( serialPeriodicity ) );
    }
}

/**
 * @brief Task 50ms
 * @param parameters
 */
static void Task_50ms( void *parameters )
{
    TickType_t xLastWakeTime = xTaskGetTickCount( );
    uint8_t clockPeriodicity = *( (uint8_t *)parameters ); /*cppcheck-suppress misra-c2012-11.5 ; Take the value of function parameter*/

    Clock_Init( );

    for( ;; )
    {
        Clock_Task( );
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( clockPeriodicity ) );
    }
}

/**
 * @brief Task 100ms
 * @param parameters
 */
static void Task_100ms( void *parameters )
{
    TickType_t xLastWakeTime   = xTaskGetTickCount( );
    uint8_t displayPeriodicity = *( (uint8_t *)parameters ); /*cppcheck-suppress misra-c2012-11.5 ; Take the value of function parameter*/

    for( ;; )
    {
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( displayPeriodicity ) );
    }
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
/**
 * @brief configSUPPORT_STATIC_ALLOCATION
 * @param ppxIdleTaskTCBBuffer
 * @param ppxIdleTaskStackBuffer
 * @param pulIdleTaskStackSize
 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exist after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ]; /*cppcheck-suppress misra-c2012-18.8 ; Its necesarry to have the static memory*/


    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
/**
 * @brief vApplicationGetTimerTaskMemory
 * @param ppxTimerTaskTCBBuffer
 * @param ppxTimerTaskStackBuffer
 * @param pulTimerTaskStackSize
 */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ]; /*cppcheck-suppress misra-c2012-18.8 ; Its necesarry to have the static memory*/

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}