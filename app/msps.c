/* Archivo con la funciones de las incilaizaciones auxiliares de la libreria */
#include "bsp.h"

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
