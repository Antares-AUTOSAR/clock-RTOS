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
 * @brief Variable for LCD configuration
 */
static LCD_HandleTypeDef hlcd;

static void Display_Machine( APP_MsgTypeDef *DisplayMsg );
static void Time( APP_MsgTypeDef *DisplayMsg );
static void Date( APP_MsgTypeDef *DisplayMsg );
static void TimeString( char *string, uint8_t hours, uint8_t minutes, uint8_t seconds );
static void DateString( char *string, uint8_t month, uint8_t day, uint16_t year, uint8_t weekday );
/**
 * @brief   Get the abbreviation of the month
 *
 * This function returns the abbreviation of a month given its number
 *
 * @param month Month number (1-12)
 * @return Pointer to the month abbreviation or NULL if the month number is invalid
 */
static char *get_month( uint8_t month )
{
    char *range = NULL;

    if( ( month >= ONE ) && ( month <= TWELVE ) )
    {
        char *month_abbreviations[] =
        {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

        range = month_abbreviations[ month - ONE ];
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
    static SPI_HandleTypeDef SpiHandle = { 0 };

    hlcd.SpiHandler = &SpiHandle;
    hlcd.RstPort    = GPIOD;
    hlcd.RstPin     = GPIO_PIN_2;
    hlcd.RsPort     = GPIOD;
    hlcd.RsPin      = GPIO_PIN_4;
    hlcd.CsPort     = GPIOD;
    hlcd.CsPin      = GPIO_PIN_3;
    hlcd.BklPort    = GPIOB;
    hlcd.BklPin     = GPIO_PIN_4;

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
    HAL_SPI_Init( &SpiHandle );

    (void)HEL_LCD_Init( &hlcd );
    (void)HEL_LCD_Backlight( &hlcd, LCD_ON );
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
 * This function initializes the state machine for the Display module.
 * It sets up an array of state functions and transitions to the initial state
 *
 * @param DisplayMsg: A pointer to the message structure containing state information
 */
static void Display_Machine( APP_MsgTypeDef *DisplayMsg )
{
    static DisplayNode stateMachine[ DISPLAY_STATES ] =
    {
    { Time },
    { Date },
    };

    static APP_Messages state;

    state = DisplayMsg->msg;
    stateMachine[ state ].stateFunc( DisplayMsg );
}

/**
 * @brief   Display the current time on the LCD
 *
 * This function rettrieves the curren time from the Clockmsg structure and
 * formats it as a string in the format 00:00:00
 *
 * @param DisplayMsg: A pointer to the message structure containing state information
 */
static void Time( APP_MsgTypeDef *DisplayMsg )
{
    char string[] = "00:00:00"; /* cppcheck-suppress misra-c2012-7.4  ;Array to print time */

    TimeString( string, DisplayMsg->tm.tm_hour, DisplayMsg->tm.tm_min, DisplayMsg->tm.tm_sec );

    (void)HEL_LCD_SetCursor( &hlcd, 1, 3 );
    (void)HEL_LCD_String( &hlcd, string );

    DisplayMsg->msg = SERIAL_MSG_DATE;
    xQueueSend( displayQueue, DisplayMsg, 0 );
}

/**
 * @brief   Display the current Date on the LCD
 *
 * This function rettrieves the curren date from the Clockmsg structure and
 * formats it as a string in the format 000,00 0000 00
 *
 * @param DisplayMsg:  A pointer to the message structure containing state information
 */
static void Date( APP_MsgTypeDef *DisplayMsg )
{
    char date_string[] = "000,00 0000 00"; /* cppcheck-suppress misra-c2012-7.4  ;Array to print date*/
    DateString( date_string, DisplayMsg->tm.tm_mon, DisplayMsg->tm.tm_mday, DisplayMsg->tm.tm_year, DisplayMsg->tm.tm_wday );

    (void)HEL_LCD_SetCursor( &hlcd, 0, 1 );
    (void)HEL_LCD_String( &hlcd, date_string );
}

/**
 * @brief  Function to convert time to a string representation
 *
 * This function takes time values for hours, minutes, seconds and formats
 * them into a string
 *
 * @param string Pointer to a character buffer to store the formatted time
 * @param hours Hours in 24-hour format (0-23)
 * @param minutes Minutes (0-59)
 * @param seconds Seconds (0-59)
 */
static void TimeString( char *string, uint8_t hours, uint8_t minutes, uint8_t seconds )
{
    string[ SECONDS_ONES ] = '0' + ( seconds % TEN ); /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ SECONDS_TENS ] = '0' + ( seconds / TEN ); /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ MINUTES_ONES ] = '0' + ( minutes % TEN ); /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ MINUTES_TENS ] = '0' + ( minutes / TEN ); /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ HOURS_ONES ]   = '0' + ( hours % TEN );   /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ HOURS_TENS ]   = '0' + ( hours / TEN );   /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality */
}

/**
 * @brief   Function to convert date to a string representation
 *
 * This function takes time values for year, month, day, weekday and formats
 * them into a string
 *
 * @param string Pointer to a character buffer to store the formatted date
 * @param month Month (1-12)
 * @param day Day of the month (1-31)
 * @param year Year
 * @param weekday Weekday(0-6), where 0 is Sunday and 6 is Saturday
 */
static void DateString( char *string, uint8_t month, uint8_t day, uint16_t year, uint8_t weekday )
{
    const char *month_abbrev = get_month( month );

    const char *weekday_abbreviations[] =
    {
    "SU", "MO", "TU", "WE", "TH", "FR", "SA" };

    const char *weekday_abbrev       = weekday_abbreviations[ weekday ];
    string[ FIRST_CHARACTER_MONTH ]  = month_abbrev[ FIRST_CHARACTER_MONTH ];
    string[ SECOND_CHARACTER_MONTH ] = month_abbrev[ SECOND_CHARACTER_MONTH ];
    string[ THIRD_CHARACTER_MONTH ]  = month_abbrev[ THIRD_CHARACTER_MONTH ];
    string[ TENS_DIGIT_DAY ]         = '0' + ( day / TEN ) % TEN;    /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality */
    string[ ONES_DIGIT_DAY ]         = '0' + ( day % TEN );          /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality */
    string[ THOUSANDS_DIGIT_YEAR ] += ( year / ONE_THOUSAND ) % TEN; /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality */
    string[ HUNDREDS_DIGIT_YEAR ] += ( year / ONE_HUNDRED ) % TEN;   /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality */
    string[ TENS_DIGIT_YEAR ] += ( year / TEN ) % TEN;               /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality */
    string[ ONES_DIGIT_YEAR ] += year % TEN;                         /* cppcheck-suppress misra-c2012-10.2  ;Not moving due to changing functionality*/
    string[ FIRST_CHARACTER_WEEK ]  = weekday_abbrev[ CERO ];
    string[ SECOND_CHARACTER_WEEK ] = weekday_abbrev[ ONE ];
}