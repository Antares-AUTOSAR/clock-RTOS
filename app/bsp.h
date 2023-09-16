#ifndef BSP_H_
#define BSP_H_

#include "FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "stm32g0xx.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/**
 * @brief   Struct for time data (time, date and alarm)
 */
typedef struct _APP_TmTypeDef
{
    uint32_t tm_sec;    /**< seconds,  range 0 to 59          */
    uint32_t tm_min;    /**< minutes, range 0 to 59           */
    uint32_t tm_hour;   /**< hours, range 0 to 23             */
    uint32_t tm_mday;   /**< day of the month, range 1 to 31  */
    uint32_t tm_mon;    /**< month, range 0 to 11             */
    uint32_t tm_year;   /**< years in rage 1900 2100          */
    uint32_t tm_wday;   /**< day of the week, range 0 to 6    */
    uint32_t tm_yday;   /**< day in the year, range 0 to 365  */
    uint32_t tm_isdst;  /**< daylight saving time             */
    uint32_t tm_hour_a; /**< alarm hour,  range 0 to 23       */
    uint32_t tm_min_a;  /**< alarm minutes,  range 0 to 59    */
} APP_TmTypeDef;


/**
 * @brief   Struct for store message type and time data
 */
typedef struct _APP_MsgTypeDef
{
    uint8_t msg;      /**< message type, allowed values 0 to 4    */
    APP_TmTypeDef tm; /**< Struct for store time data             */
} APP_MsgTypeDef;


/**
 * @brief   Enum with message types used as events
 */
typedef enum
{
    SERIAL_MSG_ALARM,
    SERIAL_MSG_DATE,
    SERIAL_MSG_TIME,
    CLOCK_MSG_PRINT
} APP_Messages;

extern QueueHandle_t clockQueue;
extern QueueHandle_t displayQueue;

extern TimerHandle_t xTimerDisplay;

extern RTC_HandleTypeDef RtcHandler;

#endif
