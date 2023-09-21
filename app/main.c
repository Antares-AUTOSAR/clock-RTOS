/**
 * @file    main.c
 * @brief Enabling of queues and tasks with static memory.
 * Develop the heart beat task.
 */

#include "bsp.h"
#include "app_clock.h"

/**
 * @brief Save the buffer dimentions.
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
 * @defgroup Periodicity and Priority
 @{*/
#define hertbeat_timer       300u                /*!< Periodicity of heart beat task */
#define TIMER_HEART_ID       (void *)(uint32_t)2 /*!< ID for timer used to HEART BEAT */
#define SERIAL_PERIODICITY   (void *)(int)10     /*!< Serial's task periodicity  */
#define CLOCK_PERIODICITY    (void *)(int)50     /*!< Clock's task periodicity   */
#define DISPLAY_PERIODICITY  (void *)(int)100    /*!< Display's task periodicity */
#define SERIAL_PRIORITY      3u                  /*!< Serial's task priority     */
#define CLOCK_PRIORITY       2u                  /*!< Clock's task priority      */
#define DISPLAY_PRIORITY     1u                  /*!< Display's task priority    */
/**@} */

/**
 * @brief Serial Queue identifier to use in the serial machine.
 */
QueueHandle_t serialQueue; /*cppcheck-suppress misra-c2012-8.7 ; Take the value of function parameter*/
/**
 * @brief Serial Queue identifier to use in the clock machine.
 */
QueueHandle_t clockQueue;
/**
 * @brief Serial Queue identifier to use in the display machine.
 */
QueueHandle_t displayQueue;

/*Task to developed*/
static void Task_10ms( void *parameters );
static void Task_50ms( void *parameters );
static void Task_100ms( void *parameters );
static void heart_Init( void );
static void heart_beat( void );
static void vTimerCallback( TimerHandle_t pxTimer ); /*Funcion Timmer Callback*/

/**
 * @brief The initial function, code development.
 * Initializing the task of each event machine (serial, clock and display) and initializing
 * the queues with static memory.
 *
 * @retval Returns an integer
 */
int main( void )
{
    /*Enable RTT and system view*/
    HAL_Init( );
    SEGGER_SYSVIEW_Conf( );
    SEGGER_SYSVIEW_Start( );
    heart_Init( );

    /*Declaration of task control structures for the static memory*/
    static StaticTask_t xTaskBuffer_10ms;
    static StaticTask_t xTaskBuffer_50ms;
    static StaticTask_t xTaskBuffer_100ms;

    static StackType_t xTaskStack_10ms[ TASK_STACK_SIZE ];
    static StackType_t xTaskStack_50ms[ TASK_STACK_SIZE ];
    static StackType_t xTaskStack_100ms[ TASK_STACK_SIZE ];

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

    /*Timmer create*/
    TimerHandle_t xTimer;

    /*Creation of tasks with different periodicities using static memory*/
    xTaskCreateStatic( Task_10ms, "Task10ms", TASK_STACK_SIZE, SERIAL_PERIODICITY, SERIAL_PRIORITY, xTaskStack_10ms, &xTaskBuffer_10ms );
    xTaskCreateStatic( Task_50ms, "Task50ms", TASK_STACK_SIZE, CLOCK_PERIODICITY, CLOCK_PRIORITY, xTaskStack_50ms, &xTaskBuffer_50ms );
    xTaskCreateStatic( Task_100ms, "Task100ms", TASK_STACK_SIZE, DISPLAY_PERIODICITY, DISPLAY_PRIORITY, xTaskStack_100ms, &xTaskBuffer_100ms );

    /*Create a timer with a time of 300 ticks, self-recharging, Heart beat*/
    xTimer = xTimerCreate( "Timer Heart", pdMS_TO_TICKS( hertbeat_timer ), pdTRUE, TIMER_HEART_ID, vTimerCallback );
    xTimerStart( xTimer, 0 ); /*Timer starts*/

    vTaskStartScheduler( );
    return 0;
}

/**
 * @brief Task 10ms
 * Developing serial event machine tasks every 10 ms.
 *
 * @param[in] parameters Periodicity value.
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
 * Developing clock event machine tasks every 50 ms.
 *
 * @param[in] parameters Periodicity value.
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
 * Developing display event machine tasks every 100 ms.
 *
 * @param[in] parameters Periodicity value.
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

/**
 * @brief Call back timer.
 *
 * @param[in] pxTimer
 */
void vTimerCallback( TimerHandle_t pxTimer )
{
    (void)pxTimer;
    heart_beat( );
}

/**
 * @brief Init the port C(LEDS).
 * Inicialization of leds, Led0.
 */
static void heart_Init( void )
{

    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE( ); /*Enable clock on port C*/

    GPIO_InitStruct.Pin   = GPIO_PIN_0;          /*Pins to set as output*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP; /*Output on mode push-pull*/
    GPIO_InitStruct.Pull  = GPIO_NOPULL;         /*No pull-up niether pull-down*/
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; /*Pin speed*/
    /*Use the previous parameters to set configuration on pin C0*/
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );

    HAL_GPIO_WritePin( GPIOC, GPIO_PIN_0, RESET );
}

/**
 * @brief Heart beat
 * GPIO_PIN_0 ON, when the time is equal or greater that 300ms.
 */
static void heart_beat( void )
{
    HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_0 );
}

/**
 * @brief configSUPPORT_STATIC_ALLOCATION
 * Function to add necessary quantity of static memory for each task.
 *
 * @param[in] ppxIdleTaskTCBBuffer   Pass out a pointer to the StaticTask_t structure.
 * @param[in] ppxIdleTaskStackBuffer Pass out the array that will be used.
 * @param[in] pulIdleTaskStackSize   Pass out the size of the array.
 */
/*cppcheck-suppress misra-c2012-8.4 ; According of freeRTOS is necessary*/
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

/**
 * @brief vApplicationGetTimerTaskMemory
 * Provide the memory that is used by the Timer service task.
 *
 * @param[in] ppxTimerTaskTCBBuffer   Pass out a pointer to the StaticTask_t structure.
 * @param[in] ppxTimerTaskStackBuffer Pass out the array that will be used.
 * @param[in] pulTimerTaskStackSize   Pass out the size of the array.
 */
/*cppcheck-suppress misra-c2012-8.4 ; According of freeRTOS is necessary*/
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