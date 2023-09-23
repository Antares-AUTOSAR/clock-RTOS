/*Archivo con la funciones de interrupcion del micrcontroladores, revisar archivo startup_stm32g0b1.S */
#include "bsp.h"

/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void NMI_Handler( void )
{
}

/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void SysTick_Handler( void )
{
    /* query if the os is running */
    if( xTaskGetSchedulerState( ) != taskSCHEDULER_NOT_STARTED )
    {
        /* service the os tick interrupt */
        xPortSysTickHandler( );
    }
    /* increment the internal tick for the hal drivers */
    HAL_IncTick( );
}

/* cppcheck-suppress misra-c2012-8.4 ; function cannot be modify */
void TIM16_FDCAN_IT0_IRQHandler( void )
{
    HAL_FDCAN_IRQHandler( &CANHandler );
}