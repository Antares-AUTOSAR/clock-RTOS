/**
 * @file    hel_lcd.h
 * @brief   Board Support LCD for microcontroller STM32
 *
 * File provides the neccesary drivers, libraries, and configurations for the LCD.
 *
 */
#ifndef HEL_LCD_H_
#define HEL_LCD_H_

#include "bsp.h"
#include <stdint.h>

/**
 * @brief Struct for LCD HANDLER.
 */
typedef struct _LCD_HandleTypeDef
{

    SPI_HandleTypeDef *SpiHandler; /*!< SPI handler address of the spi to use with the LCD*/
    GPIO_TypeDef *RstPort;         /*!< Port where the pin to control the LCD reset pin is*/
    uint32_t RstPin;               /*!< Pin to control the LCD reset pin */
    GPIO_TypeDef *RsPort;          /*!< Port where the pin to control the LCD RS pin*/
    uint32_t RsPin;                /*!< Pin to control the LCD RS pin */
    GPIO_TypeDef *CsPort;          /*!< Port where the pin to control the LCD chip select is*/
    uint32_t CsPin;                /*!< Pin to control the LCD chip select pin */
    GPIO_TypeDef *BklPort;         /*!< Port where the pin to control the LCD backlight is*/
    uint32_t BklPin;               /*!< Pin to control the LCD backlight pin */

} LCD_HandleTypeDef;

/*Prototype of LCD's functions*/
extern uint8_t HEL_LCD_Init( LCD_HandleTypeDef *hlcd );
extern void HEL_LCD_MspInit( LCD_HandleTypeDef *hlcd );
extern uint8_t HEL_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd );
extern uint8_t HEL_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data );
extern uint8_t HEL_LCD_String( LCD_HandleTypeDef *hlcd, char *str );
extern uint8_t HEL_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );
extern void HEL_LCD_Backlight( LCD_HandleTypeDef *hlcd, uint8_t state );
extern uint8_t HEL_LCD_Contrast( LCD_HandleTypeDef *hlcd, uint8_t contrast );

#endif
