/**
 * @file    hel_lcd.c
 * @brief   Board Support LCD for microcontroller STM32.
 *
 * File provides the neccesary drivers, libraries, and configurations for the LCD.
 *
 */
#include "bsp.h"
#include "hel_lcd.h"



/**
 * @brief  Function to initialize the LCD.
 *
 * Commands to sent for SPI communication between the LCD and the microcontroller.
 *
 * @param[in] hlcd  Pointer to Lcd handler.
 * @retval value 	Returns an indication of a successful operation or not.
 */
uint8_t HEL_LCD_Init( LCD_HandleTypeDef *hlcd )
{

    uint8_t value      = HAL_OK;
    uint8_t i          = 0;
    uint8_t flag_error = 0;

    HEL_LCD_MspInit( hlcd );

    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );
    HAL_GPIO_WritePin( hlcd->RstPort, hlcd->RstPin, RESET );
    vTaskDelay( pdMS_TO_TICKS( 2 ) );

    HAL_GPIO_WritePin( hlcd->RstPort, hlcd->RstPin, SET );
    vTaskDelay( pdMS_TO_TICKS( 20 ) );

    while( i == 0u )
    {

        value = HEL_LCD_Command( hlcd, (uint8_t)0x30 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        vTaskDelay( pdMS_TO_TICKS( 2 ) );

        value = HEL_LCD_Command( hlcd, (uint8_t)0x30 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x30 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x39 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x14 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x56 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x6D );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        vTaskDelay( pdMS_TO_TICKS( 200 ) );

        value = HEL_LCD_Command( hlcd, (uint8_t)0x70 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x0D );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x06 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x01 );

        if( value == HAL_ERROR )
        {

            flag_error = HAL_ERROR;
        }

        vTaskDelay( pdMS_TO_TICKS( 10 ) );

        i = 1;
    }

    if( flag_error == HAL_ERROR )
    {

        value = HAL_ERROR;
    }

    return value;
}

/**
 * @brief   Sub-initialization interface for the pins used by the LCD.
 *
 * @param[out] hlcd LCD Handler.
 */
/* cppcheck-suppress misra-c2012-8.7 ; prefix weak declared in librerias HAL */
/* cppcheck-suppress misra-c2012-8.6 ; prefix weak declared in librerias HAL */
__weak void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd )
{

    /* Prevent unused argument(s) compilation warning */
    UNUSED( hlcd );
}

/**
 * @brief   Function to send a command to the LCD.
 *
 * The function must wait as long as necessary for the command to be accepted by the LCD .
 *
 * @param[out] hlcd LCD Handler.
 * @param[in] cmd   8-bit Command.
 * @retval value    Returns an indication of a successful operation or not.
 */
uint8_t HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd )
{

    uint8_t value = HAL_OK;

    HAL_GPIO_WritePin( hlcd->RsPort, hlcd->RsPin, RESET );
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, RESET );

    value = HAL_SPI_Transmit( hlcd->SpiHandler, &cmd, sizeof( cmd ), HAL_MAX_DELAY );

    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );

    return value;
}

/**
 * @brief   Function to send a character to be displayed on the LCD using the SPI.
 *
 * @param[out] hlcd LCD Handler.
 * @param[in] data  8-bit Command.
 * @retval value    Returns an indication of a successful operation or not.
 */
/* cppcheck-suppress misra-c2012-8.7 ; For the moment this function is not required */
uint8_t HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )
{

    uint8_t value = HAL_OK;

    HAL_GPIO_WritePin( hlcd->RsPort, hlcd->RsPin, SET );
    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, RESET );

    value = HAL_SPI_Transmit( hlcd->SpiHandler, &data, sizeof( data ), HAL_MAX_DELAY );

    HAL_GPIO_WritePin( hlcd->CsPort, hlcd->CsPin, SET );

    return value;
}

/**
 * @brief   Function to send a string of characters to the LCD.
 *
 * @param[out] hlcd LCD Handler.
 * @param[out] str  String.
 * @retval value    Returns an indication of a successful operation or not.
 */
uint8_t HEL_LCD_String( LCD_HandleTypeDef *hlcd, char *str )
{

    uint8_t value = HAL_OK;

    char *str_back = str;

    while( *str_back != '\0' )
    {

        value = HEL_LCD_Data( hlcd, (uint8_t)*str_back );
        str_back++;
    }

    return value;
}

/**
 * @brief   Function to locate the LCD cursor.
 *
 * @param[out] hlcd LCD Handler.
 * @param[in] row   Value 0 to 1.
 * @param[in] col   Value 0 to 15.
 * @retval value    Returns an indication of a successful operation or not.
 */
uint8_t HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col )
{

    uint8_t value = HAL_ERROR;

    if( ( row < 2u ) && ( col < 16u ) )
    {

        uint8_t address_offset = 0;

        if( row == 0u )
        {

            address_offset = col;
        }
        else
        {

            address_offset = (uint8_t)0x40 + col;
        }

        value = HEL_LCD_Command( hlcd, (uint8_t)0x80 | address_offset );
    }

    return value;
}

/**
 * @brief   Function to control the LCD backlight.
 *
 * @param[out] hlcd LCD Handler.
 * @param[in] state Describe the intensity.
 */
void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state )
{

    if( state == LCD_OFF )
    {

        HAL_GPIO_WritePin( hlcd->BklPort, hlcd->BklPin, RESET );
    }

    if( state == LCD_ON )
    {

        HAL_GPIO_WritePin( hlcd->BklPort, hlcd->BklPin, SET );
    }

    if( state == LCD_TOGGLE )
    {

        HAL_GPIO_TogglePin( hlcd->BklPort, hlcd->BklPin );
    }
}

/**
 * @brief   Function to control the contrast level of the LCD.
 *
 * @param[out] hlcd    LCD Handler.
 * @param[in] contrast Describe the contrast, 0 to 63.
 * @retval value       Returns an indication of a successful operation or not.
 */
/* cppcheck-suppress misra-c2012-8.7 ; currently, it will not be used in another document but later it will */
uint8_t HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast )
{

    uint8_t value = HAL_ERROR;

    if( contrast < 65u )
    {

        value = HEL_LCD_Command( hlcd, (uint8_t)0x70 | contrast );
    }

    return value;
}