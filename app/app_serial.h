#ifndef APP_SERIAL_H__
#define APP_SERIAL_H__

#include "bsp.h"

/**
  * @defgroup States for serial
  @{ */
#define TIME_STATE               1u /*!< Time state     */
#define DATE_STATE               2u /*!< Date state     */
#define ALARM_STATE              3u /*!< Alarm state    */
#define OK_STATE                 4u /*!< Ok state       */
#define ERROR_STATE              5u /*!< Error state    */
#define TOTAL_STATES             5u /*!< Total of state */
/**
  @} */

/**
  * @defgroup Months, months of the year in hexa
  @{ */
#define JANUARY                  0x01u /*!< January in hexa   */
#define FEBRUARY                 0x02u /*!< February in hexa  */
#define MARCH                    0x03u /*!< March in hexa     */
#define APRIL                    0x04u /*!< April in hexa     */
#define MAY                      0x05u /*!< May in hexa       */
#define JUNE                     0x06u /*!< June in hexa      */
#define JULY                     0x07u /*!< July in hexa      */
#define AUGUST                   0x08u /*!< August in hexa    */
#define SEPTEMBER                0x09u /*!< September in hexa */
#define OCTOBER                  0x10u /*!< October in hexa   */
#define NOVEMBER                 0x11u /*!< November in hexa  */
#define DECEMBER                 0x12u /*!< December in hexa  */
/**
  @} */

/**
  * @defgroup Numbers, defines for numbers in decimal
  @{ */
#define NUM_0                    0u   /*!< Number 0   */
#define NUM_1                    1u   /*!< Number 1   */
#define NUM_2                    2u   /*!< Number 2   */
#define NUM_3                    3u   /*!< Number 3   */
#define NUM_4                    4u   /*!< Number 4   */
#define NUM_5                    5u   /*!< Number 5   */
#define NUM_7                    7u   /*!< Number 7   */
#define NUM_8                    8u   /*!< Number 8   */
#define NUM_10                   10u  /*!< Number 10  */
#define NUM_12                   12u  /*!< Number 12  */
#define NUM_13                   13u  /*!< Number 13  */
#define NUM_100                  100u /*!< Number 100 */
/**
  @} */

/**
  * @defgroup Numbers, defines for numbers in hexa
  @{ */
#define HEX_0                    0x00u   /*!< Number 0 in hexa    */
#define HEX_1                    0x01u   /*!< Number 1 in hexa    */
#define HEX_2                    0x02u   /*!< Number 2 in hexa    */
#define HEX_3                    0x03u   /*!< Number 3 in hexa    */
#define HEX_4                    0x04u   /*!< Number 4 in hexa    */
#define HEX_5                    0x05u   /*!< Number 5 in hexa    */
#define HEX_6                    0x06u   /*!< Number 6 in hexa    */
#define HEX_0F                   0x0Fu   /*!< Number F in hexa    */
#define HEX_23                   0x23u   /*!< Number 23 in hexa   */
#define HEX_28                   0x28u   /*!< Number 28 in hexa   */
#define HEX_29                   0x29u   /*!< Number 29 in hexa   */
#define HEX_30                   0x30u   /*!< Number 30 in hexa   */
#define HEX_31                   0x31u   /*!< Number 31 in hexa   */
#define HEX_55                   0x55u   /*!< Number 55 in hexa   */
#define HEX_59                   0x59u   /*!< Number 59 in hexa   */
#define HEX_AA                   0xAAu   /*!< Number AA in hexa   */
#define HEX_0F                   0x0Fu   /*!< Number 0F in hexa   */
#define HEX_1901                 0x1901u /*!< Number 1901 in hexa */
#define HEX_2099                 0x2099u /*!< Number 2099 in hexa */
/**
  @} */

/**
  * @defgroup Defines for CAN configuration
  @{ */
#define VAL_EXTFILTERSNBR        0u     /*!< Value for ExtFiltersNbr        */
#define VAL_STDFILTERSNDR        1u     /*!< Value for StdFiltersNbr        */
#define VAL_NOMINALPRESCALER     8u     /*!< Value for NominalPrescaler     */
#define VAL_NOMINALSYNCJUMPWIDTH 1u     /*!< Value for NominalSyncJumpWidth */
#define VAL_NOMINALTIMESEG1      11u    /*!< Value for NominalTimeSeg1      */
#define VAL_NOMINALTIMESEG2      4u     /*!< Value for NominalTimeSeg2      */
#define VAL_IDENTIFIER           0x122u /*!< Value for Identifier           */
#define VAL_FILTERINDEX          0u     /*!< Value for FilterIndex          */
#define VAL_FILTERID1            0x111u /*!< Value for FilterID1            */
#define VAL_FILTERID2            0x7FFu /*!< Value for FilterID2            */
#define VAL_BUFFERINDEXES        0u     /*!< Value for BufferIndexes        */
/**
  @} */

/**
  * @defgroup Time for serial task
  @{ */
#define TEN_MS                   10u /*!< Ten miliseconds */
/**
  @} */

typedef struct SerialStates
{
    void ( *ptr_funct )( void );
} SerialStates;

extern void Serial_Init( void );
extern void Serial_Task( void );

#endif