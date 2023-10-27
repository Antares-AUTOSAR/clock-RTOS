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
 * @defgroup SerialTestingMacros unit testing
 @{*/
#ifdef UTEST
#define STATIC /*!< Macro defined for testing purposes.*/
#else
#define STATIC static /*!< Macro defined for testing purposes.*/
#endif
/**
@} */

/**
  * @defgroup Months, months of the year in hexa
  @{ */
#define JANUARY                       1u  /*!< January in hexa   */
#define FEBRUARY                      2u  /*!< February in hexa  */
#define MARCH                         3u  /*!< March in hexa     */
#define APRIL                         4u  /*!< April in hexa     */
#define MAY                           5u  /*!< May in hexa       */
#define JUNE                          6u  /*!< June in hexa      */
#define JULY                          7u  /*!< July in hexa      */
#define AUGUST                        8u  /*!< August in hexa    */
#define SEPTEMBER                     9u  /*!< September in hexa */
#define OCTOBER                       10u /*!< October in hexa   */
#define NOVEMBER                      11u /*!< November in hexa  */
#define DECEMBER                      12u /*!< December in hexa  */
/**
  @} */

/**
  * @defgroup Numbers, defines for numbers in decimal
  @{ */
#define NUM_0                         0u   /*!< Number 0   */
#define NUM_1                         1u   /*!< Number 1   */
#define NUM_4                         4u   /*!< Number 4   */
#define NUM_5                         5u   /*!< Number 5   */
#define NUM_100                       100u /*!< Number 100 */
/**
  @} */

/**
  * @defgroup Defines for incoming data elements
  @{ */
#define SINGLE_FRAME_ELEMENT          0u /*!< Single frame element recieved */

#define TIME_HOUR_ELEMENT             1u /*!< Time hour element recieved */
#define TIME_MIN_ELEMENT              2u /*!< Time min element recieved  */
#define TIME_SEC_ELEMENT              3u /*!< Time sec element recieved  */

#define DATE_DAY_ELEMENT              1u /*!< Date day element recieved      */
#define DATE_MON_ELEMENT              2u /*!< Date month element recieved    */
#define DATE_MSB_YEAR_ELEMENT         3u /*!< Date MSB year element recieved */
#define DATE_LSB_YEAR_ELEMENT         4u /*!< Date MSB year element recieved */

#define ALARM_HOUR_ELEMENT            1u /*!< Alarm hour element recieved */
#define ALARM_MIN_ELEMENT             2u /*!< Alarm min element recieved  */

#define DATA_OK                       0x55u /*!< Message for ok data in hexa         */
#define DATA_ERROR                    0xAAu /*!< Message for error in data in hexa   */
/**
  @} */

/**
  * @defgroup Max and min values of data in hexa
  @{ */
#define MAX_HOUR_HEX                  23u   /*!< Max hour 23 in hexa                         */
#define MAX_MIN_HEX                   59u   /*!< Max minutes 59 in hexa                      */
#define MAX_SEC_HEX                   59u   /*!< Max seconds 59 in hexa                      */
#define DAYS_IN_FERUARY_HEX           28u   /*!< Days in february, 28 in hexa                */
#define DAYS_IN_LEAP_YEAR_FERUARY_HEX 29u   /*!< Days in leap year in february, 29 in hexa   */
#define MIN_DAY_HEX                   01u   /*!< Min day, 1 in hexa                          */
#define MAX_DAYS_HEX_30               30u   /*!< Max days 30 in hexa                         */
#define MAX_DAYS_HEX_31               31u   /*!< Max days 31 in hexa                         */
#define MIN_YEAR_HEX                  1901u /*!< Min year, 1901 in hexa                      */
#define MAX_YEAR_HEX                  2099u /*!< Max year, 2099 in hexa                      */
#define TRUE                          1u    /*!< True in a comparison                        */
#define MESSAGE                       1u    /*!< Message location                            */
#define TICKS                         0u    /*!< The maximum amount of time                  */
/**
  @} */

/**
  * @defgroup Values for CAN configuration
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

/**
 * @brief   Enum with states for state machines
 */
typedef enum
{
    SERIAL_TIME,  /**< Clock machine state for set alarm */
    SERIAL_DATE,  /**< Clock machine state for set date  */
    SERIAL_ALARM, /**< Clock machine state for set time  */
    SERIAL_OK,    /**< Clock machine state for set print */
    SERIAL_ERROR, /**< Clock machine state for set print */
    SERIAL_IDLE   /**< Clock machine state idle          */
} MACHINE_SERIAL;

extern void Serial_Init( void );
extern void Serial_Task( void );

#endif