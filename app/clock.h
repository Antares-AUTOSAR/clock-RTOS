/**
 * @file    clock.h
 * @brief   ****
 *
 * Interfaces for clock process:
 *
 * void Clock_Init( void ) : Use once to Initialize the clock. Don't requires any parameter
 *
 * void Clock_Task( void ) : Run continuosuly. It allows to configure the clock
 *
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "bsp.h"


#define TOTAL_CLOCK_STATES 4U /*!< Total states in event machine */

#define RTC_INIT_PASYNC    127 /*!< Value for RTC async prediv    */
#define RTC_INIT_PSYNC     255 /*!< Value for RTC sync prediv     */

#define RTC_INITIAL_HOUR   0x12 /*!< Initial RTC hour              */
#define RTC_INITIAL_MIN    0x00 /*!< Initial RTC minutes           */
#define RTC_INITIAL_SEC    0x00 /*!< Initial RTC seconds           */
#define RTC_INITIAL_SSEC   0x00 /*!< Initial RTC subseconds        */
#define RTC_INITIAL_MDAY   0x01 /*!< Initial RTC day               */
#define RTC_INITIAL_YEAR   0x00 /*!< Initial RTC year 2 last digit */
#define RTC_INITIAL_YEARH  20   /*!< Initial RTC year 2 first dig  */

#define TIMER_DISPLAY_ID   1    /*!< ID for timer used to update display  */

/**
 * @brief   Struct for store function for each state of event machine
 */
typedef struct
{
    void ( *stateFunc )( APP_MsgTypeDef *receivedMessage ); /*!< Pointer to function which perform state statements */
} StateNode;


void Clock_Init( void );
void Clock_Task( void );


#endif