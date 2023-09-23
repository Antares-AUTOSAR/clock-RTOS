/**
 * @file    app_serial.h
 * @brief   **Message processing implementation**
 *
 * Contains the defines to initialize the CAN port with the apropiate parameters,
 * and other defines needed in the aplication.
 *
 */
#ifndef APP_SERIAL_H__
#define APP_SERIAL_H__

/** 
  * @defgroup Months, months of the year in hexa
  @{ */
#define JANUARY                       0x01u /*!< January in hexa   */
#define FEBRUARY                      0x02u /*!< February in hexa  */
#define MARCH                         0x03u /*!< March in hexa     */
#define APRIL                         0x04u /*!< April in hexa     */
#define MAY                           0x05u /*!< May in hexa       */
#define JUNE                          0x06u /*!< June in hexa      */
#define JULY                          0x07u /*!< July in hexa      */
#define AUGUST                        0x08u /*!< August in hexa    */
#define SEPTEMBER                     0x09u /*!< September in hexa */
#define OCTOBER                       0x10u /*!< October in hexa   */
#define NOVEMBER                      0x11u /*!< November in hexa  */
#define DECEMBER                      0x12u /*!< December in hexa  */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in decimal
  @{ */
#define NUM_0                         0u   /*!< Number 0   */
#define NUM_1                         1u   /*!< Number 1   */
#define NUM_3                         3u   /*!< Number 3   */
#define NUM_4                         4u   /*!< Number 4   */
#define NUM_5                         5u   /*!< Number 5   */
#define NUM_7                         7u   /*!< Number 7   */
#define NUM_8                         8u   /*!< Number 8   */
#define NUM_10                        10u  /*!< Number 10  */
#define NUM_12                        12u  /*!< Number 12  */
#define NUM_13                        13u  /*!< Number 13  */
#define NUM_100                       100u /*!< Number 100 */
/**
  @} */

/** 
  * @defgroup Defines for incoming data elements
  @{ */
#define SINGLE_FRAME_ELEMENT          0u /*!< Single frame element recieved */

#define TIME_DATA_ELEMENT             1u /*!< Time data element recieved */
#define TIME_HOUR_ELEMENT             2u /*!< Time hour element recieved */
#define TIME_MIN_ELEMENT              3u /*!< Time min element recieved  */
#define TIME_SEC_ELEMENT              4u /*!< Time sec element recieved  */

#define DATE_DATA_ELEMENT             1u /*!< Date data element recieved     */
#define DATE_DAY_ELEMENT              2u /*!< Date day element recieved      */
#define DATE_MON_ELEMENT              3u /*!< Date month element recieved    */
#define DATE_MSB_YEAR_ELEMENT         4u /*!< Date MSB year element recieved */
#define DATE_LSB_YEAR_ELEMENT         5u /*!< Date MSB year element recieved */

#define ALARM_DATA_ELEMENT            1u /*!< Alarm data element recieved */
#define ALARM_HOUR_ELEMENT            2u /*!< Alarm hour element recieved */
#define ALARM_MIN_ELEMENT             3u /*!< Alarm min element recieved  */

#define DATA_OK                       0x55u /*!< Message for ok data in hexa         */
#define DATA_ERROR                    0xAAu /*!< Message for error in data in hexa   */
/**
  @} */

/** 
  * @defgroup Mask for least significant nibble
  @{ */
#define HEX_0F                        0x0Fu /*!< Number 0F in hexa   */
/**
  @} */

/** 
  * @defgroup Numbers, defines for numbers in hexa
  @{ */
#define MAX_HOUR_HEX                  0x23u   /*!< Max hour 23 in hexa                         */
#define MAX_MIN_HEX                   0x59u   /*!< Max minutes 59 in hexa                      */
#define MAX_SEC_HEX                   0x59u   /*!< Max seconds 59 in hexa                      */
#define DAYS_IN_FERUARY_HEX           0x28u   /*!< Days in february, 28 in hexa                */
#define DAYS_IN_LEAP_YEAR_FERUARY_HEX 0x29u   /*!< Days in leap year in february, 29 in hexa   */
#define MIN_DAY_HEX                   0x01u   /*!< Min day, 1 in hexa                          */
#define MAX_DAYS_HEX_30               0x30u   /*!< Max days 30 in hexa                         */
#define MAX_DAYS_HEX_31               0x31u   /*!< Max days 31 in hexa                         */
#define MIN_YEAR_HEX                  0x1901u /*!< Min year, 1901 in hexa                      */
#define MAX_YEAR_HEX                  0x2099u /*!< Max year, 2099 in hexa                      */
/**
  @} */

/** 
  * @defgroup Defines for CAN configuration
  @{ */
#define VAL_EXTFILTERSNBR             0u     /*!< Value for ExtFiltersNbr        */
#define VAL_STDFILTERSNDR             1u     /*!< Value for StdFiltersNbr        */
#define VAL_NOMINALPRESCALER          8u     /*!< Value for NominalPrescaler     */
#define VAL_NOMINALSYNCJUMPWIDTH      1u     /*!< Value for NominalSyncJumpWidth */
#define VAL_NOMINALTIMESEG1           11u    /*!< Value for NominalTimeSeg1      */
#define VAL_NOMINALTIMESEG2           4u     /*!< Value for NominalTimeSeg2      */
#define VAL_IDENTIFIER                0x122u /*!< Value for Identifier           */
#define VAL_FILTERINDEX               0u     /*!< Value for FilterIndex          */
#define VAL_FILTERID1                 0x111u /*!< Value for FilterID1            */
#define VAL_FILTERID2                 0x7FFu /*!< Value for FilterID2            */
#define VAL_BUFFERINDEXES             0u     /*!< Value for BufferIndexes        */
/**
  @} */

extern void Serial_Init( void );
extern void Serial_Task( void );

#endif