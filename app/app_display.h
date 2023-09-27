/**
 * @file    app_display.h
 * @brief   Interfaces for display process
 *
 */

#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#define DISPLAYS     2U /*!< Total states in event machine */

/**
 * @defgroup Declarations of counters
 * @{
 */
#define CERO         0u    /*!< Value for counter: 0 */
#define ONE          1u    /*!< Value for counter: 1 */
#define TWO          2u    /*!< Value for counter: 2 */
#define THREE        3u    /*!< Value for counter: 3 */
#define FOUR         4u    /*!< Value for counter: 4 */
#define FIVE         5u    /*!< Value for counter: 5 */
#define SIX          6u    /*!< Value for counter: 6 */
#define SEVEN        7u    /*!< Value for counter: 7 */
#define EIGHT        8u    /*!< Value for counter: 8 */
#define NINE         9u    /*!< Value for counter: 9 */
#define TEN          10u   /*!< Value for counter: 10 */
#define ELEVEN       11u   /*!< Value for counter: 11 */
#define TWELVE       12u   /*!< Value for counter: 12 */
#define THIRTEEN     13u   /*!< Value for counter: 13 */
#define FIFTEEN      15u   /*!< Value for counter: 15 */
#define ONE_HUNDRED  100U  /*!< Value for counter: 100 */
#define ONE_THOUSAND 1000U /*!< Value for counter: 1000 */
/**
 * @}
 */

void Display_Init( void );
void Display_Task( void );

#endif