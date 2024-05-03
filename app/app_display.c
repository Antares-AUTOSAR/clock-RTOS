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
    Display_M ( *stateFunc )( APP_MsgTypeDef *DisplayMsg ); /*!< Pointer to function which perform state statements */
} DisplayNode;

/**
 * @brief Variable for LCD configuration
 */
LCD_HandleTypeDef Hlcd; /* cppcheck-suppress misra-c2012-8.4  ;Not moving due to unit testing*/

/**
 * @brief Variable for SPI configuration
 */
SPI_HandleTypeDef SpiHandle; /* cppcheck-suppress misra-c2012-8.4  ;Not moving due to unit testing*/

/**
 * @brief  Strucuture that gives format to timer
 */
static TIM_HandleTypeDef TimHandle;

/**
 * @brief  Flag that will tell if the buzzer needs to be activated
 */
uint8_t buzzer_flag = 0; /* cppcheck-suppress misra-c2012-8.4  ;Not moving due to unit testing*/

/**
 * @brief  Flag that will tell if the buzzer needs to be activated
 */
uint8_t buzzer = 0; /* cppcheck-suppress misra-c2012-8.4  ;Not moving due to unit testing*/

/**
 * @brief  Struct for handling Software timer for the buzzer
 */
static TimerHandle_t xTimerBuzzer;

/**
 * @brief  Struct for handling Software timer for 1mn for the buzzer
 */
static TimerHandle_t xTimer1Mn_Buzzer;

static void Display_Machine( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Time( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Date( APP_MsgTypeDef *DisplayMsg );
STATIC void TimeString( char *string, uint8_t hours, uint8_t minutes, uint8_t seconds );
STATIC void DateString( char *string, uint8_t month, uint8_t day, uint16_t year, uint8_t weekday );
STATIC char *get_month( uint8_t month );
STATIC Display_M Alarm_A( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Alarm( APP_MsgTypeDef *DisplayMsg );
STATIC Display_M Alarm_Clean( APP_MsgTypeDef *DisplayMsg );
STATIC void Display_Buzzer( TimerHandle_t pxTimer );
STATIC void Display_1Mn_Buzzer( TimerHandle_t pxTimer );

/**
 * @brief   Get the abbreviation of the month
 *
 * This function returns the abbreviation of a month given its number
 *
 * @param month Month number (1-12)
 * @return Pointer to the month abbreviation or NULL if the month number is invalid
 */
STATIC char *get_month( uint8_t month )
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
STATIC void TimeString( char *string, uint8_t hours, uint8_t minutes, uint8_t seconds )
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
STATIC void DateString( char *string, uint8_t month, uint8_t day, uint16_t year, uint8_t weekday )
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

/**
 * @brief   Initializate the LCD
 *
 * This function gives the format of spi and initializate the lcd
 *
 */
void Display_Init( void )
{
    TIM_OC_InitTypeDef sConfig;

    Hlcd.SpiHandler = &SpiHandle;
    Hlcd.RstPort    = GPIOD;
    Hlcd.RstPin     = GPIO_PIN_2;
    Hlcd.RsPort     = GPIOD;
    Hlcd.RsPin      = GPIO_PIN_4;
    Hlcd.CsPort     = GPIOD;
    Hlcd.CsPin      = GPIO_PIN_3;
    Hlcd.BklPort    = GPIOB;
    Hlcd.BklPin     = GPIO_PIN_4;

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

    (void)HEL_LCD_Init( &Hlcd );
    (void)HEL_LCD_Backlight( &Hlcd, LCD_ON );
    (void)HEL_LCD_Contrast( &Hlcd, 15 );

    TimHandle.Instance         = TIM14;
    TimHandle.Init.Prescaler   = 10;
    TimHandle.Init.Period      = 3200;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init( &TimHandle );

    sConfig.OCMode     = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.Pulse      = 1600;
    HAL_TIM_PWM_ConfigChannel( &TimHandle, &sConfig, TIM_CHANNEL_1 );

    xTimerBuzzer = xTimerCreate( "Timer", 1000, pdTRUE, TIMER_BUZZER_ID, Display_Buzzer );
    xTimerStart( xTimerBuzzer, TICKS );

    xTimer1Mn_Buzzer = xTimerCreate( "Timer", 60000, pdTRUE, TIMER_BUZZER_ID, Display_1Mn_Buzzer );
    xTimerStart( xTimer1Mn_Buzzer, TICKS );
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

    while( xQueueReceive( displayQueue, &DisplayMsg, 0 ) == pdTRUE )
    {
        (void)Display_Machine( &DisplayMsg );
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
    { Alarm_A },
    { Alarm },
    { Alarm_Clean },
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
 * @return DISPLAY_MSG_TIME: State for testing and knowing a message have been sent
 */
STATIC Display_M Time( APP_MsgTypeDef *DisplayMsg )
{
    char string[] = "00:00:00"; /* cppcheck-suppress misra-c2012-7.4  ;Array to print time */

    TimeString( string, DisplayMsg->tm.tm_hour, DisplayMsg->tm.tm_min, DisplayMsg->tm.tm_sec );

    (void)HEL_LCD_SetCursor( &Hlcd, 1, 3 );
    (void)HEL_LCD_String( &Hlcd, string );

    DisplayMsg->msg = SERIAL_MSG_DATE;
    xQueueSend( displayQueue, DisplayMsg, 0 );
    return DISPLAY_MSG_TIME;
}

/**
 * @brief   Display the current Date on the LCD
 *
 * This function rettrieves the curren date from the Clockmsg structure and
 * formats it as a string in the format 000,00 0000 00
 *
 * @param DisplayMsg:  A pointer to the message structure containing state information
 * @return DISPLAY_IDLE_STATE: State for testing, even though it doesnt make the transition of another state it must return to IDLE
 */
STATIC Display_M Date( APP_MsgTypeDef *DisplayMsg )
{
    char date_string[] = "000,00 0000 00"; /* cppcheck-suppress misra-c2012-7.4  ;Array to print date*/
    DateString( date_string, DisplayMsg->tm.tm_mon, DisplayMsg->tm.tm_mday, DisplayMsg->tm.tm_year, DisplayMsg->tm.tm_wday );

    (void)HEL_LCD_SetCursor( &Hlcd, 0, 1 );
    (void)HEL_LCD_String( &Hlcd, date_string );

    return DISPLAY_IDLE_STATE;
}

/**
 * @brief   Prints the letter A
 *
 * This function prints the letter A when the alarm has been setted
 * @param DisplayMsg:  A pointer to the message structure containing state information
 * @return DISPLAY_1: State for testing and knowing a message have been sent
 */
STATIC Display_M Alarm_A( APP_MsgTypeDef *DisplayMsg )
{
    UNUSED( DisplayMsg );

    (void)HEL_LCD_SetCursor( &Hlcd, 1, 0 );
    (void)HEL_LCD_Data( &Hlcd, 'A' );

    return DISPLAY_1;
}

/**
 * @brief  Prints the ALARM!!!
 *
 * This function prints ALARM!!! when the alarm has been activated. Furthermore, it indicates that the buzzer must be activated
 * @param DisplayMsg:  A pointer to the message structure containing state information
 * @return DISPLAY_2: State for testing and knowing a message have been sent
 */
STATIC Display_M Alarm( APP_MsgTypeDef *DisplayMsg )
{
    UNUSED( DisplayMsg );
    char string[] = "ALARM!!!"; /* cppcheck-suppress misra-c2012-7.4  ;Array to print time */
    buzzer        = ACTIVE;
    buzzer_flag   = ACTIVE;

    (void)HEL_LCD_SetCursor( &Hlcd, 1, 0 );
    (void)HEL_LCD_Data( &Hlcd, ' ' );

    (void)HEL_LCD_SetCursor( &Hlcd, 1, 3 );
    (void)HEL_LCD_String( &Hlcd, string );
    (void)HEL_LCD_Backlight( &Hlcd, LCD_ON );

    return DISPLAY_2;
}

/**
 * @brief  Clean Alarm
 *
 * This function prints stop and turn off before the one-minute lapse expire by receiving a new date, time, or alarm through the CAN bus
 * @param DisplayMsg:  A pointer to the message structure containing state information
 * @return DISPLAY_3: State for testing and knowing a message have been sent
 */
STATIC Display_M Alarm_Clean( APP_MsgTypeDef *DisplayMsg )
{
    UNUSED( DisplayMsg );

    buzzer_flag = INACTIVE;
    buzzer      = INACTIVE;
    HAL_TIM_PWM_Stop( &TimHandle, TIM_CHANNEL_1 );

    xTimerStop( xTimerBuzzer, TICKS );
    xTimerStop( xTimer1Mn_Buzzer, TICKS );
    (void)HEL_LCD_Backlight( &Hlcd, LCD_ON );
    xTimerStart( xTimerDisplay, TICKS );

    return DISPLAY_3;
}

/**
 * @brief   Function for sotware timer of the buzzer
 * @param   pxTimer Timer handler
 *
 * This function with the help of the alarm that has been triggered, start to blink the LCD and making blink the buzzer
 */
STATIC void Display_Buzzer( TimerHandle_t pxTimer )
{
    UNUSED( pxTimer );

    if( buzzer_flag == ACTIVE )
    {
        if( buzzer == ACTIVE )
        {
            buzzer = INACTIVE;
            HAL_TIM_PWM_Start( &TimHandle, TIM_CHANNEL_1 );
            (void)HEL_LCD_Backlight( &Hlcd, LCD_OFF );
        }
        else
        {
            buzzer = ACTIVE;
            HAL_TIM_PWM_Stop( &TimHandle, TIM_CHANNEL_1 );
            (void)HEL_LCD_Backlight( &Hlcd, LCD_ON );
        }
    }
}

/**
 * @brief  Function for software timer for the 1mn of the alarm
 *
 * This function after 1mn the alarm will be desactivated either by the time or by other message
 * @param   pxTimer Timer handler
 */
STATIC void Display_1Mn_Buzzer( TimerHandle_t pxTimer )
{
    UNUSED( pxTimer );

    if( buzzer_flag == ACTIVE )
    {
        HAL_TIM_PWM_Stop( &TimHandle, TIM_CHANNEL_1 );
        xTimerStop( xTimerBuzzer, TICKS );
        (void)HEL_LCD_Backlight( &Hlcd, LCD_ON );
    }
}