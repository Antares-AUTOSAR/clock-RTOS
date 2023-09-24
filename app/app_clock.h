/**
 * @file    app_clock.h
 * @brief   Interfaces for clock process
 *
 * void Clock_Init( void ) : Use once to Initialize the clock. Don't requires any parameter
 * void Clock_Task( void ) : Run continuosuly. It allows to configure the clock
 *
 */

#ifndef APP_CLOCK_H
#define APP_CLOCK_H

#ifdef UTEST
#define STATIC
#define CLOCK_QUEUE_TESTING   QueueHandle_t clockQueue;
#define DISPLAY_QUEUE_TESTING QueueHandle_t displayQueue;
#else
#define STATIC static
#define CLOCK_QUEUE_TESTING
#define DISPLAY_QUEUE_TESTING
#endif

#define TOTAL_CLOCK_STATES 4U /*!< Total states in event machine */

/**
 * @defgroup RTCInitializationParameters Initial values for RTC
 @{*/
#define RTC_INIT_PASYNC    127  /*!< Value for RTC async prediv    */
#define RTC_INIT_PSYNC     255  /*!< Value for RTC sync prediv     */
#define RTC_INITIAL_HOUR   0x12 /*!< Initial RTC hour              */
#define RTC_INITIAL_MIN    0x00 /*!< Initial RTC minutes           */
#define RTC_INITIAL_SEC    0x00 /*!< Initial RTC seconds           */
#define RTC_INITIAL_SSEC   0x00 /*!< Initial RTC subseconds        */
#define RTC_INITIAL_MDAY   0x01 /*!< Initial RTC day               */
#define RTC_INITIAL_YEAR   0x00 /*!< Initial RTC year 2 last digit */
#define RTC_INITIAL_YEARH  20   /*!< Initial RTC year 2 first dig  */
/**@} */

#define TIMER_DISPLAY_ID   (void *)(uint32_t)1 /*!< ID for timer used to update display  */

void Clock_Init( void );
void Clock_Task( void );


#endif