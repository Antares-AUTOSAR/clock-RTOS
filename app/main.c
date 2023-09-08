/**
 * @file    main.c
 * @brief   **Template Application entry point**
 *
 * The main file is the entry point of the application or any user code, please provide the proper
 * description of this file according to your own implementation This Demo app only blinks an LED
 * connected to PortA Pin 5
 *
 * @note    Only the files inside folder app will be take them into account when the doxygen runs
 *          by typing "make docs", index page is generated in
 *          Build/doxigen/html/index.html
 */
#include "bsp.h"

static void vTask( void *parameters );

/**
 * @brief   **Application entry point**
 *
 * Provide the proper description for function main according to your own implementation
 *
 * @retval  None
 */
int main( void )
{
    HAL_Init( );
    /*enable RTT and system view*/
    SEGGER_SYSVIEW_Conf( );
    SEGGER_SYSVIEW_Start( );

    xTaskCreate( vTask, "Task", 128u, NULL, 1u, NULL );

    vTaskStartScheduler( );

    return 0u;
}

/**
 * @brief   **Sample task**
 *
 * This is only a demo task that blinks a led and displaya message in the segger terminal
 *
 * @param[in]   parameters any data pass by reference to task function
 */
static void vTask( void *parameters )
{
    UNUSED( parameters );
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOC_CLK_ENABLE( );

    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin   = GPIO_PIN_0;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );

    for( ;; )
    {
        HAL_GPIO_TogglePin( GPIOC, GPIO_PIN_0 );
        /*this is just like semihosting but only takes microseconds*/
        SEGGER_RTT_printf( 0, "Hola mundo de SEGGER\r\n" );
        HAL_Delay( 2000u );
    }
}
