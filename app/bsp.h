#ifndef BSP_H_
#define BSP_H_

#include "FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "stm32g0xx.h"
#include "task.h"
#include "queue.h"

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

/*Queue identifier to use*/
extern QueueHandle_t serialQueue;
extern QueueHandle_t clockQueue;
extern QueueHandle_t displayQueue;

#endif
