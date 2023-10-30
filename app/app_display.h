/**
 * @file    app_display.h
 * @brief   Interfaces for display process
 *
 */

#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#define DISPLAY_STATES         5U /*!< Total states in event machine */

/**
 * @defgroup Declarations of counters
 * @{
 */
#define CERO                   0u    /*!< Value for counter: 0 */
#define ONE                    1u    /*!< Value for counter: 1 */
#define TEN                    10u   /*!< Value for counter: 10 */
#define TWELVE                 12u   /*!< Value for counter: 12 */
#define ONE_HUNDRED            100U  /*!< Value for counter: 100 */
#define ONE_THOUSAND           1000U /*!< Value for counter: 1000 */
/**
 * @}
 */

/**
 * @defgroup Digits array for Date
 * @{
 */
#define FIRST_CHARACTER_MONTH  0u  /*!< Position 0: First character of the month */
#define SECOND_CHARACTER_MONTH 1u  /*!< Position 1: Second character of the month */
#define THIRD_CHARACTER_MONTH  2u  /*!< Position 2: First character of the month */
#define TENS_DIGIT_DAY         4u  /*!< Position 4: Tens digits of the day */
#define ONES_DIGIT_DAY         5u  /*!< Position 5: Ones digits of the day */
#define THOUSANDS_DIGIT_YEAR   7u  /*!< Position 7: Thousands digit of the year */
#define HUNDREDS_DIGIT_YEAR    8u  /*!< Position 8: Hundreds digit of the year */
#define TENS_DIGIT_YEAR        9u  /*!< Position 9: Tens digit of the year */
#define ONES_DIGIT_YEAR        10u /*!< Position 10: Ones digit of the year */
#define FIRST_CHARACTER_WEEK   12u /*!< Position 12: First character of the week */
#define SECOND_CHARACTER_WEEK  13u /*!< Position 13: Second character of the week */
/**
 * @}
 */

/**
 * @defgroup Array abbreviation for Time
 * @{
 */
#define SECONDS_ONES           7u /*!< Position 7: Seconds ones digit */
#define SECONDS_TENS           6u /*!< Position 6: Seconds tens digit */
#define MINUTES_ONES           4u /*!< Position 5: Minutes ones digit*/
#define MINUTES_TENS           3u /*!< Position 4: Minutes tens digit */
#define HOURS_ONES             1u /*!< Position 3: Hours ones digit */
#define HOURS_TENS             0u /*!< Position 1: Hours tens digit */
/**
 * @}
 */

#define TIMER_BUZZER_ID        (void *)(uint32_t)1 /*!< ID for timer used to update display  */
#define ACTIVE                 1u                  /*!< Buzzer state */
#define INACTIVE               0u                  /*!< Buzzer state  */
#define TICKS                  0u                  /*!< The maximum amount of time */

/**
 * @defgroup Testing static Functions
 * @{
 */
#ifdef UTEST
#define STATIC
#else
#define STATIC static /*!< precompilation direcctive */
#endif
/**
 * @}
 */

void Display_Init( void );
void Display_Task( void );
#endif