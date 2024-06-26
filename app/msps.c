/**
 * @file    msps.c
 * @brief   Msp initialization functions
 *
 * This file contains the initialization functions for MSP
 *
 */

#include "bsp.h"
#include "hel_lcd.h"

/**
 * @brief   Initialize MSP
 *
 * This function initializes the MSP for the microcontroller.
 * It sets up the essential system configuration and clock setting requiered for the microcontroller's operation.
 * Additionally, it configures the MCU internal clock to run at 64MHz for the AHB bus and 32 MHz for the APB bus.
 */
/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void HAL_MspInit( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    __HAL_RCC_SYSCFG_CLK_ENABLE( );
    __HAL_RCC_PWR_CLK_ENABLE( );

    /** Configure the main internal regulator output voltage*/
    HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 );

    /* Initializes the RCC Oscillators according to the specified parameters in
    the RCC_OscInitTypeDef structure The frequency set is 64MHz with the internal
    16MHz HSI oscilator. According to the formulas:
    fVCO = fPLLIN x ( N / M ) = 16MHz x (8 / 1) = 128MHz
    fPLLP = fVCO / P = 128MHz / 2 = 64MHz
    fPLLQ = fVCO / Q = 128MHz / 2 = 64MHz fPLLR = fVCO / R = 128MHz / 2 = 64MHz
    */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN            = 8;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    HAL_RCC_OscConfig( &RCC_OscInitStruct );

    /** Initializes the CPU, AHB and APB buses clocks*/
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_2 );
}

/**
 * @brief   Initialize RTC (Real Time Clock)
 *
 * This function intializes the RTC for the microcontroller
 * It configures and enables the necessary clocks and system resources for the RTC operation
 * Additionally, it enable the LSE (Low Speed External) clock in low-power mode to feed the internal RTC
 *
 * @param hrtc: pointer to RTC handle structure
 */
/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void HAL_RTC_MspInit( RTC_HandleTypeDef *hrtc )
{
    (void)hrtc;

    RCC_OscInitTypeDef RCC_OscInitStruct         = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

    __HAL_RCC_SYSCFG_CLK_ENABLE( );
    __HAL_RCC_PWR_CLK_ENABLE( );

    HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 );
    HAL_PWR_EnableBkUpAccess( );
    __HAL_RCC_LSEDRIVE_CONFIG( RCC_LSEDRIVE_LOW );

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection    = RCC_RTCCLKSOURCE_NONE;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState       = RCC_LSI_OFF;
    HAL_RCC_OscConfig( &RCC_OscInitStruct );

    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct );

    __HAL_RCC_RTC_ENABLE( );
    __HAL_RCC_RTCAPB_CLK_ENABLE( );

    HAL_NVIC_SetPriority( RTC_TAMP_IRQn, 2, 0 );
    HAL_NVIC_EnableIRQ( RTC_TAMP_IRQn );
}

/**
 * @brief   Initialize FDCAN
 *
 * This function initializes the FDCAN peripheral and associated GPIO pins for operation
 * It configures the clock sources, GPIO pins, and interrupt priorities to enable FDCAN communication
 *
 * @param hfdcan: pointer to FDCAN handle structure
 */
/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void HAL_FDCAN_MspInit( FDCAN_HandleTypeDef *hfdcan )
{
    (void)hfdcan;

    GPIO_InitTypeDef GpioCanStruct;

    /* Enable the clocks of the peripherals GPIO and CAN */
    __HAL_RCC_FDCAN_CLK_ENABLE( );
    __HAL_RCC_GPIOD_CLK_ENABLE( );

    /* Set pin 0(rx) and pin 1(x) to alternate mode for FDCAN1 */
    GpioCanStruct.Mode      = GPIO_MODE_AF_PP;
    GpioCanStruct.Alternate = GPIO_AF3_FDCAN1;
    GpioCanStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
    GpioCanStruct.Pull      = GPIO_NOPULL;
    GpioCanStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( GPIOD, &GpioCanStruct );

    HAL_NVIC_SetPriority( TIM16_FDCAN_IT0_IRQn, 2, 0 );
    HAL_NVIC_EnableIRQ( TIM16_FDCAN_IT0_IRQn );
}

/**
 * @brief   Initialize SPI
 *
 * This function initializes the SPI peripheral and associated GPIO pins for operation
 * It enables the SPI clock and the GPIO ports used for SPI communication
 *
 * @param hspi: pointer to SPI handle structure
 */
/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void HAL_SPI_MspInit( SPI_HandleTypeDef *hspi )
{
    (void)hspi;

    GPIO_InitTypeDef GPIO_InitStruct;
    __GPIOD_CLK_ENABLE( );
    __GPIOB_CLK_ENABLE( );
    __GPIOC_CLK_ENABLE( );
    __SPI1_CLK_ENABLE( );

    GPIO_InitStruct.Pin       = GPIO_PIN_6 | GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_SPI1;
    HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );
}

/* cppcheck-suppress misra-c2012-8.6 ; its external linkage is declared at HAL library */
void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd )
{
    __GPIOD_CLK_ENABLE( );
    __GPIOB_CLK_ENABLE( );

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin   = hlcd->RstPin | hlcd->CsPin | hlcd->RsPin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( hlcd->RstPort, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = hlcd->BklPin;
    HAL_GPIO_Init( hlcd->BklPort, &GPIO_InitStruct );

    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    HAL_GPIO_WritePin( hlcd->BklPort, hlcd->BklPin, SET );
}

/**
 * @brief   Initialize PWM
 *
 * This function initializes the TIM peripheral and associated GPIO pins for operation
 * It enables the ports for using the pwm with the buzzer
 *
 * @param htim: pointer to htmi handle structure
 */
/* cppcheck-suppress misra-c2012-8.4 ; its external linkage is declared at HAL library */
void HAL_TIM_PWM_MspInit( TIM_HandleTypeDef *htim )
{
    (void)htim;
    GPIO_InitTypeDef GPIO_InitStruct;

    __TIM14_CLK_ENABLE( );
    __GPIOC_CLK_ENABLE( );

    GPIO_InitStruct.Pin       = GPIO_PIN_12;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM14;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
}