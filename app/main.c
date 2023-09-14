/**
 * @file    main.c
 *
 */
#include "bsp.h"

/*Prototype of static memory*/
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );
/*Prototype of timer in static memory*/
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize );

/*Save the buffer dimentions*/
#define TASK_STACK_SIZE 128u

/*Queue identifier to use*/
QueueHandle_t serialQueue; 
QueueHandle_t clockQueue; 
QueueHandle_t displayQueue;  

/*Task to developed*/
static void Task_10ms( void *parameters );
static void Task_50ms( void *parameters );
static void Task_100ms( void *parameters );

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

    /*Queue create*/
    /*serialQueue = xQueueCreateStatic( 9, sizeof( NEW_MsgTypeDef ) );
    clockQueue = xQueueCreate( 45, sizeof( APP_MsgTypeDef ) );
    displayQueue = xQueueCreate( 90, sizeof( APP_MsgTypeDef ) );*/


    /*Creation of tasks with different periodicities using static memory*/
    xTaskCreateStatic( Task_10ms, "Task10ms", TASK_STACK_SIZE, &serialPeriod, 3u, xTaskStack_10ms, &xTaskBuffer_10ms );
    xTaskCreateStatic( Task_50ms, "Task50ms", TASK_STACK_SIZE, &clockPeriod, 2u, xTaskStack_50ms, &xTaskBuffer_50ms );
    xTaskCreateStatic( Task_100ms, "Task100ms", TASK_STACK_SIZE, &displayPeriod, 1u, xTaskStack_100ms, &xTaskBuffer_100ms );

    vTaskStartScheduler( );
    return 0;
}

static void Task_10ms( void *parameters )
{
    TickType_t xLastWakeTime  = xTaskGetTickCount( );
    uint8_t serialPeriodicity = *( (uint8_t *)parameters ); /*cppcheck-suppress misra-c2012-11.5 ; Take the value of function parameter*/

    for( ;; )
    {
        SEGGER_RTT_printf( 0, "Tarea 10 ms\n" );
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( serialPeriodicity ) );
    }
}

static void Task_50ms( void *parameters )
{
    TickType_t xLastWakeTime = xTaskGetTickCount( );
    uint8_t clockPeriodicity = *( (uint8_t *)parameters ); /*cppcheck-suppress misra-c2012-11.5 ; Take the value of function parameter*/

    for( ;; )
    {
        SEGGER_RTT_printf( 0, "Tarea 50 ms\n" );
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( clockPeriodicity ) );
    }
}

static void Task_100ms( void *parameters )
{
    TickType_t xLastWakeTime   = xTaskGetTickCount( );
    uint8_t displayPeriodicity = *( (uint8_t *)parameters ); /*cppcheck-suppress misra-c2012-11.5 ; Take the value of function parameter*/

    for( ;; )
    {
        SEGGER_RTT_printf( 0, "Tarea 100 ms\n" );
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( displayPeriodicity ) );
    }
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
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