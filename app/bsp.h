/**
 * @file bsp.h
 * @brief Init the structs
 * Inicialization of structs with Data, Time and Alarm values.
 */

#ifndef BSP_H_
#define BSP_H_

#include "FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "stm32g0xx.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/**
 * @brief Struct with array to queue
 */
typedef struct
{
    uint8_t Data[ 8 ]; /*!< Array to save the elements*/
} NEW_MsgTypeDef;


/**
 * @brief  Struct with the date, time and alarm elements
 */
typedef struct _APP_TmTypeDef
{
    uint32_t tm_sec;        /*!< seconds,  range 0 to 59          */
    uint32_t tm_min;        /*!< minutes, range 0 to 59           */
    uint32_t tm_hour;       /*!< hours, range 0 to 23             */
    uint32_t tm_mday;       /*!< day of the month, range 1 to 31  */
    uint32_t tm_mon;        /*!< month, range 0 to 11             */
    uint32_t tm_year;       /*!<* years in rage 1900 2100          */
    uint32_t tm_wday;       /*!< day of the week, range 0 to 6    */
    uint32_t tm_yday;       /*!< day in the year, range 0 to 365  */
    uint32_t tm_isdst;      /*!< daylight saving time             */
    uint32_t tm_min_alarm;  /*!< minutes, range 0 to 59 ALARM     */
    uint32_t tm_hour_alarm; /*!< hours, range 0 to 23   ALARM     */
} APP_TmTypeDef;

/**
 * @brief  Struct with the msg element
 */
typedef struct _APP_MsgTypeDef
{
    uint8_t msg;      /*!< Store the message type to send */
    APP_TmTypeDef tm; /*!< time and date in stdlib tm format */
} APP_MsgTypeDef;

/**
 * @brief   Enum with message types used as events
 */
typedef enum
{
    SERIAL_MSG_ALARM, /**< Received Alarm     - message type 0 */
    SERIAL_MSG_DATE,  /**< Received Date      - message type 1 */
    SERIAL_MSG_TIME,  /**< Received Time      - message type 2 */
    CLOCK_MSG_PRINT   /**< Print clock data   - message type 3 */
} APP_Messages;

/*Queue identifier to use*/
extern QueueHandle_t serialQueue;
extern QueueHandle_t clockQueue;
extern QueueHandle_t displayQueue;

/*Timer handlers*/
extern TimerHandle_t xTimerDisplay;

/*RTC handler*/
extern RTC_HandleTypeDef RtcHandler;

#endif