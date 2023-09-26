/**
 * @file    app_display.c
 * @brief   Implementation of state machine of LCD
 *
 * In this file we are going to see the initialization of the lcd and the application
 *
 */
#include "bsp.h"
#include "app_display.h"
#include "hel_lcd.h"

/**
 * @brief   Struct for store function for each state of event machine
 */
typedef struct
{
    void ( *stateFunc )( APP_MsgTypeDef *DisplayMsg ); /*!< Pointer to function which perform state statements */
} DisplayNode;

/**
 * @brief
 */
static LCD_HandleTypeDef hlcd;

static void Time( APP_MsgTypeDef *DisplayMsg );
static void Display_Machine( APP_MsgTypeDef *DisplayMsg );

static char *get_month( uint8_t month )
{
    char *range = NULL;

    if( ( month >= 1u ) && ( month <= 12u ) )
    {
        char *month_abbreviations[] =
        {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

        range = month_abbreviations[ month - 1u ];
    }
    return range;
}

/**
 * @brief   Initializate the LCD
 *
 * This function gives the format of spi and initializate the lcd
 *
 */
void Display_Init( void )
{
    static SPI_HandleTypeDef SpiHandle;
    hlcd.SpiHandler = &SpiHandle;

    hlcd.RstPort = GPIOD;
    hlcd.RstPin  = GPIO_PIN_2;
    hlcd.RsPort  = GPIOD;
    hlcd.RsPin   = GPIO_PIN_4;
    hlcd.CsPort  = GPIOD;
    hlcd.CsPin   = GPIO_PIN_3;
    hlcd.BklPort = GPIOB;
    hlcd.BklPin  = GPIO_PIN_4;

    SpiHandle.Instance               = SPI1;
    SpiHandle.Init.Mode              = SPI_MODE_MASTER;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLED;
    HAL_SPI_Init( &SpiHandle ); /* cppcheck-suppress misra-c2012-17.7 ;This function is defined in the library*/

    (void)HEL_LCD_Init( &hlcd );
    (void)HEL_LCD_Backlight( &hlcd, 1 );
    (void)HEL_LCD_Contrast( &hlcd, 15 );
}

/**
 * @brief Display task function
 *
 * This function checks the queue of pending tasks every 100ms and process them calling the display machine
 *
 */
void Display_Task( void )
{
    static APP_MsgTypeDef DisplayMsg = { 0 };

    while( xQueueReceive( displayQueue, &DisplayMsg, 0 ) == pdPASS )
    {
        Display_Machine( &DisplayMsg );
    }
}

/**
 * @brief   Initializate the State machine of Display
 *
 * This function recive the flag of Clock.c and will display Date and Time. Afterwards
 * depending on the message recieved from alarm will be display different cases
 */
static void Display_Machine( APP_MsgTypeDef *DisplayMsg )
{
    static DisplayNode stateMachine[ DISPLAYS ] =
    {
    { Time },
    { Date },
    };

    /*  Variable to handle the current state  */
    static APP_Messages event;

    event = DisplayMsg->msg;
    stateMachine[ event ].stateFunc( DisplayMsg );
}

static void Time( APP_MsgTypeDef *DisplayMsg )
{
    char string[] = "00:00:00"; /* cppcheck-suppress misra-c2012-7.4  ;Array to print time */

    string[ 7 ] = '0' + ( DisplayMsg->tm.tm_sec % 10u );  /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ 6 ] = '0' + ( DisplayMsg->tm.tm_sec / 10u );  /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ 4 ] = '0' + ( DisplayMsg->tm.tm_min % 10u );  /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ 3 ] = '0' + ( DisplayMsg->tm.tm_min / 10u );  /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ 1 ] = '0' + ( DisplayMsg->tm.tm_hour % 10u ); /* cppcheck-suppress misra-c2012-10.2 ;Not moving due to changing functionality*/
    string[ 0 ] = '0' + ( DisplayMsg->tm.tm_hour / 10u ); /* cppcheck-suppress misra-c2012-10.2 ;Not moving due to changing functionality */

    (void)HEL_LCD_SetCursor( &hlcd, 1, 3 );
    (void)HEL_LCD_String( &hlcd, string );
    DisplayMsg->msg = DISPLAY_DATE;
    xQueueSend( displayQueue, DisplayMsg, 0 );
}


