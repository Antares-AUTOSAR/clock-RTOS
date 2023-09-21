#include "app_serial.h"

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
static uint8_t CanTp_SingleFrameRx( const uint8_t *data, const uint8_t *size );
static uint8_t Valid_Time( const uint8_t *data );
static uint8_t Valid_Alarm( const uint8_t *data );
static uint8_t Valid_Date( const uint8_t *data );
static uint32_t WeekDay( const uint8_t *data );
static void Serial_StMachine( NEW_MsgTypeDef *pdata );
static void SerialTimeState( const NEW_MsgTypeDef *pmsg );
static void SerialDateState( const NEW_MsgTypeDef *pmsg );
static void SerialAlarmState( const NEW_MsgTypeDef *pmsg );
static void SerialOkState( const NEW_MsgTypeDef *pmsg );
static void SerialErrorState( const NEW_MsgTypeDef *pmsg );

/**
 * @brief  Structure type variable for user CAN initialization
 */
FDCAN_HandleTypeDef CANHandler;

/**
 * @brief  Structure type variable for CAN transmissin initialization
 */
static FDCAN_TxHeaderTypeDef CANTxHeader;

/**
 * @brief   **Initialize of CAN port**
 *
 * Is the function to initialize all the required to start working with the CAN port 
 * and the messages reception processing
 *
 * @param   CANHandler  [out] Structure type variable to configure CAN operation mode
 * @param   CANTxHeader [out] Structure type variable to configure CAN transmicion
 * 
 * @note Declaration of the options to configure the FDCAN1 module to transmit to the CAN bus at 100Kbps
 *       and sample point of 75%, the frequency with which the CAN module is powered is
 *       fCAN = fHSI / CANHandler.Init.ClockDivider / CANHandler.Init.NominalPrescaler
 *       fCAN = 32MHz / 1 / 8 = 4MHz
 *       The number of time quantas required is
 *       Ntq = fCAN / CANbaudrate
 *       Ntq = 4MHz / 250Kbps = 16
 *       The sample point percentage is
 *       Sp = ( CANHandler.Init.NominalTimeSeg1 +  1 / Ntq ) * 100
 *       Sp = ( ( 11 + 1 ) / 16 ) * 100 = 75%
 */
void Serial_Init( void )
{
    FDCAN_FilterTypeDef CANFilter;

    CANHandler.Instance                  = FDCAN1;
    CANHandler.Init.Mode                 = FDCAN_MODE_NORMAL;
    CANHandler.Init.FrameFormat          = FDCAN_FRAME_CLASSIC;
    CANHandler.Init.ClockDivider         = FDCAN_CLOCK_DIV1;
    CANHandler.Init.TxFifoQueueMode      = FDCAN_TX_FIFO_OPERATION;
    CANHandler.Init.AutoRetransmission   = DISABLE;
    CANHandler.Init.TransmitPause        = DISABLE;
    CANHandler.Init.ProtocolException    = DISABLE;
    CANHandler.Init.ExtFiltersNbr        = VAL_EXTFILTERSNBR;
    CANHandler.Init.StdFiltersNbr        = VAL_STDFILTERSNDR; /* inicialize filter */
    CANHandler.Init.NominalPrescaler     = VAL_NOMINALPRESCALER;
    CANHandler.Init.NominalSyncJumpWidth = VAL_NOMINALSYNCJUMPWIDTH;
    CANHandler.Init.NominalTimeSeg1      = VAL_NOMINALTIMESEG1;
    CANHandler.Init.NominalTimeSeg2      = VAL_NOMINALTIMESEG2;
    /* Set configuration of CAN module*/
    HAL_FDCAN_Init( &CANHandler );

    /* Declaration of options for configur parameters of CAN transmicion */
    CANTxHeader.IdType      = FDCAN_STANDARD_ID;
    CANTxHeader.FDFormat    = FDCAN_CLASSIC_CAN;
    CANTxHeader.TxFrameType = FDCAN_DATA_FRAME;
    CANTxHeader.Identifier  = VAL_IDENTIFIER;
    CANTxHeader.DataLength  = FDCAN_DLC_BYTES_8;

    /* Configure reception filter to Rx FIFO 0, it will only accept messages with ID 0x111 */
    CANFilter.IdType       = FDCAN_STANDARD_ID;
    CANFilter.FilterIndex  = VAL_FILTERINDEX;
    CANFilter.FilterType   = FDCAN_FILTER_MASK;
    CANFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    CANFilter.FilterID1    = VAL_FILTERID1;
    CANFilter.FilterID2    = VAL_FILTERID2;
    HAL_FDCAN_ConfigFilter( &CANHandler, &CANFilter );

    /* Messages without the filter will by rejected */
    HAL_FDCAN_ConfigGlobalFilter( &CANHandler, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE );

    /* Change FDCAN instance from initialization mode to normal mode */
    HAL_FDCAN_Start( &CANHandler );

    /* Activate interruption by reception in fifo0 to the arrive of a message */
    HAL_FDCAN_ActivateNotification( &CANHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, VAL_BUFFERINDEXES );
}

/**
 * @brief   **Reception of CAN messages**
 *
 * Callback to the CAN interrup, it is activated when a message 
 * is recieve then is store in the variable NewMessage
 *
 * @param   hfdcan      [in]  To handle CAN
 * @param   RxFifo0ITs  [in]  void Valid_Time( uint8_t *data );
 *
 */
/* cppcheck-suppress misra-c2012-8.4 ; this is a library function */
void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
{
    FDCAN_RxHeaderTypeDef CANRxHeader;

    static NEW_MsgTypeDef NewMessage;

    if( ( RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE ) != NUM_0 )
    {
        /* Retrieve Rx messages from RX FIFO0 */
        HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, &NewMessage.Data[ NUM_0 ] );

        xQueueSendFromISR( serialQueue, &NewMessage.Data[ NUM_0 ], NULL );
    }
}

/**
 * @brief   **Call of the serial state machine**
 *
 * This function calls the serial state machine every 10ms
 * with the help of queues, therefore it won't be execute all the time
 *
 */
void Serial_Task( void )
{
    static NEW_MsgTypeDef RecieveMsg = { 0 };

    while( xQueueReceive( serialQueue, &RecieveMsg.Data[ NUM_0 ], 0 ) == pdTRUE )
    {
        if( CanTp_SingleFrameRx( &RecieveMsg.Data[ NUM_1 ], &RecieveMsg.Data[ NUM_0 ] ) == NUM_1 )
        {
            Serial_StMachine( &RecieveMsg );
        }
        else
        {
            Serial_StMachine( &RecieveMsg );
        }
    }
}

/**
 * @brief   **Main state machine**
 *
 * Is going to implement the state machine in charge of calling the function for the
 * new message, after the interruption of CAN is trigger, depending if it is a message of time, 
 * date and alarm.
 *
 * @param   pdata  [in]  Pointer to data struct
 */
void Serial_StMachine( NEW_MsgTypeDef *pdata )
{
    static SerialStates StMachine[ NUM_5 ] = {
    { SerialTimeState },
    { SerialDateState },
    { SerialAlarmState },
    { SerialOkState },
    { SerialErrorState } };

    static NEW_MsgTypeDef NextEvent;
    NextEvent.Data[ NUM_0 ] = pdata->Data[ NUM_1 ];

    StMachine[ NextEvent.Data[ NUM_0 ] - NUM_1 ].ptr_funct( pdata );
}

/**
 * @brief   **Serial time function**
 *
 * Is going to evaluate if the data for time is valid 
 * and send the message to the queue.
 *
 * @param   pdata  [in]  Pointer to data struct
 */
void SerialTimeState( const NEW_MsgTypeDef *pmsg )
{
    static NEW_MsgTypeDef SerialMsg;

    if( Valid_Time( &pmsg->Data[ NUM_0 ] ) == NUM_1 )
    {
        SerialMsg.Data[ NUM_0 ] = pmsg->Data[ NUM_0 ];
        SerialMsg.Data[ NUM_1 ] = OK_STATE;
        SerialMsg.Data[ NUM_2 ] = pmsg->Data[ NUM_2 ];
        SerialMsg.Data[ NUM_3 ] = pmsg->Data[ NUM_3 ];
        SerialMsg.Data[ NUM_4 ] = pmsg->Data[ NUM_4 ];

        xQueueSend( serialQueue, &SerialMsg, 0 );
    }
    else
    {
        SerialMsg.Data[ NUM_1 ] = ERROR_STATE;
        xQueueSend( serialQueue, &SerialMsg, 0 );
    }
}

/**
 * @brief   **Serial date function**
 *
 * Is going to evaluate if the data for date is valid 
 * and send the message to the queue.
 *
 * @param   pdata  [in]  Pointer to data struct
 */
void SerialDateState( const NEW_MsgTypeDef *pmsg )
{
    static NEW_MsgTypeDef SerialMsg;

    if( Valid_Date( &pmsg->Data[ NUM_0 ] ) == NUM_1 )
    {
        SerialMsg.Data[ NUM_0 ] = pmsg->Data[ NUM_0 ];
        SerialMsg.Data[ NUM_1 ] = OK_STATE;
        SerialMsg.Data[ NUM_2 ] = pmsg->Data[ NUM_2 ];
        SerialMsg.Data[ NUM_3 ] = pmsg->Data[ NUM_3 ];
        SerialMsg.Data[ NUM_4 ] = pmsg->Data[ NUM_4 ];
        SerialMsg.Data[ NUM_5 ] = pmsg->Data[ NUM_5 ];
        xQueueSend( serialQueue, &SerialMsg, 0 );
    }
    else
    {
        SerialMsg.Data[ NUM_1 ] = ERROR_STATE;
        xQueueSend( serialQueue, &SerialMsg, 0 );
    }
}

/**
 * @brief   **Serial alarm function**
 *
 * Is going to evaluate if the data for alarm is valid 
 * and send the message to the queue.
 *
 * @param   pdata  [in]  Pointer to data struct
 */
void SerialAlarmState( const NEW_MsgTypeDef *pmsg )
{
    static NEW_MsgTypeDef SerialMsg;

    if( Valid_Alarm( &pmsg->Data[ NUM_0 ] ) == NUM_1 )
    {
        SerialMsg.Data[ NUM_0 ] = pmsg->Data[ NUM_0 ];
        SerialMsg.Data[ NUM_1 ] = OK_STATE;
        SerialMsg.Data[ NUM_2 ] = pmsg->Data[ NUM_2 ];
        SerialMsg.Data[ NUM_3 ] = pmsg->Data[ NUM_3 ];
        xQueueSend( serialQueue, &SerialMsg, 0 );
    }
    else
    {
        SerialMsg.Data[ NUM_1 ] = ERROR_STATE;
        xQueueSend( serialQueue, &SerialMsg, 0 );
    }
}

/**
 * @brief   **Serial ok function**
 *
 * Is going to assign the data to the variable clockMsg
 * and send it to the queue of the clock, then it will transmit
 * a message through CAN to indicate the data is valid.
 *
 * @param   pdata  [in]  Pointer to data struct
 */
void SerialOkState( const NEW_MsgTypeDef *pmsg )
{
    uint8_t i;
    uint8_t msg_ok[ NUM_1 ] = { HEX_55 };
    static APP_MsgTypeDef clockMsg;

    clockMsg.msg        = pmsg->Data[ NUM_1 ];
    clockMsg.tm.tm_hour = pmsg->Data[ NUM_2 ];
    clockMsg.tm.tm_min  = pmsg->Data[ NUM_3 ];
    clockMsg.tm.tm_sec  = pmsg->Data[ NUM_4 ];
    clockMsg.tm.tm_wday = WeekDay( &pmsg->Data[ NUM_0 ] );
    clockMsg.tm.tm_mday = pmsg->Data[ NUM_2 ];
    clockMsg.tm.tm_mon  = pmsg->Data[ NUM_3 ];
    clockMsg.tm.tm_year = pmsg->Data[ NUM_4 ] << NUM_8;
    clockMsg.tm.tm_year += pmsg->Data[ NUM_5 ];
    clockMsg.tm.tm_hour_alarm = pmsg->Data[ NUM_2 ];
    clockMsg.tm.tm_min_alarm  = pmsg->Data[ NUM_3 ];

    i = pmsg->Data[ NUM_0 ];

    CanTp_SingleFrameTx( &msg_ok[ NUM_0 ], i );
    xQueueSend( clockQueue, &clockMsg, 0 );
}

/**
 * @brief   **Serial error function**
 *
 * Is going to transmit a message through CAN to indicate the data is not valid.
 *
 * @param   pdata  [in]  Pointer to data struct
 */
void SerialErrorState( const NEW_MsgTypeDef *pmsg )
{
    uint8_t i;
    uint8_t msn_error[ NUM_1 ] = { HEX_AA };

    i = pmsg->Data[ NUM_0 ];

    CanTp_SingleFrameTx( &msn_error[ NUM_0 ], i );
}

/**
 * @brief   **Validate time**
 *
 * This function validates if the time message is in range,
 * from 0 to 23 for hours, from 0 to 59 for minutes and
 * from 0 to 59 for seconds and returns the result.
 *
 * @param   data          [in]  Pointer to data
 *
 * @retval  The function returns 1 if time is correct and 0 if not 
 */
static uint8_t Valid_Time( const uint8_t *data )
{
    uint8_t ret_val = NUM_0;

    if( ( data[ NUM_2 ] <= HEX_23 ) && ( data[ NUM_3 ] <= HEX_59 ) && ( data[ NUM_4 ] <= HEX_59 ) )
    {
        ret_val = NUM_1;
    }

    return ret_val;
}

/**
 * @brief   **Validate alarm**
 *
 * This function validates if the Alarm message is in range,
 * from 0 to 23 for hours and from 0 to 59 to minutes, 
 * no seconds nedded, and then returns the result.
 *
 * @param   data          [in]  Pointer to data
 *
 * @retval  The function returns 1 if the alarm is correct and 0 if not 
 */
static uint8_t Valid_Alarm( const uint8_t *data )
{
    uint8_t ret_val = NUM_0;

    if( ( data[ NUM_2 ] <= HEX_23 ) && ( data[ NUM_3 ] <= HEX_59 ) )
    {
        ret_val = NUM_1;
    }

    return ret_val;
}

/**
 * @brief   **Validate date**
 *
 * This function validates if the date message is in range,
 * from 1 to 31 for days, from 1 to 12 for months and
 * from 1901 to 2099 for years and returns the result.
 * In addition it also vaidates if the date is in a leap-year
 * and adjust the month of February to manage 29 days.
 * 
 * @param   data          [in]  Pointer to data
 *
 * @retval  The function returns 1 if the date is correct and 0 if not 
 */
static uint8_t Valid_Date( const uint8_t *data )
{
    uint8_t ret_val;
    uint16_t year;
    year = data[ NUM_4 ] << NUM_8;
    year += data[ NUM_5 ];

    if( ( ( data[ NUM_2 ] >= HEX_1 ) && ( data[ NUM_2 ] <= HEX_31 ) ) &&
        ( ( data[ NUM_3 ] >= JANUARY ) && ( data[ NUM_3 ] <= DECEMBER ) ) &&
        ( ( year >= HEX_1901 ) && ( year <= HEX_2099 ) ) )
    {
        if( ( year % NUM_4 ) == NUM_0 ) // Check leap year and february
        {
            if( ( data[ NUM_3 ] == FEBRUARY ) && ( data[ NUM_2 ] <= HEX_29 ) )
            {
                ret_val = NUM_1;
            }
            else
            {
                ret_val = NUM_0;
            }
        }
        else if( ( data[ NUM_3 ] == FEBRUARY ) && ( data[ NUM_2 ] <= HEX_28 ) ) // Check for february
        {
            ret_val = NUM_1;
        }
        else if( ( ( data[ NUM_3 ] == APRIL ) ||
                   ( data[ NUM_3 ] == JUNE ) ||
                   ( data[ NUM_3 ] == SEPTEMBER ) ||
                   ( data[ NUM_3 ] == NOVEMBER ) ) &&
                 ( data[ NUM_2 ] <= HEX_30 ) ) // Check for months with 30 days
        {
            ret_val = NUM_1;
        }
        else if( ( data[ NUM_3 ] == JANUARY ) ||
                 ( data[ NUM_3 ] == MARCH ) ||
                 ( data[ NUM_3 ] == MAY ) ||
                 ( data[ NUM_3 ] == JULY ) ||
                 ( data[ NUM_3 ] == AUGUST ) ||
                 ( data[ NUM_3 ] == OCTOBER ) ||
                 ( data[ NUM_3 ] == DECEMBER ) ) // Otherwise, the month has 31 days
        {
            ret_val = NUM_1;
        }
        else
        {
            ret_val = NUM_0;
        }
    }
    else
    {
        ret_val = NUM_0;
    }

    return ret_val;
}

/**
 * @brief   **Validate the day of the week**
 *
 * This function calculates the day of the week according to the date,
 * to do this, we will use Zeller's congruence. Taking the day, month and year
 * to calculate the year of the century and the century. In this algorithm 
 * January and February are counted as months 13 and 14 of the previous year.
 * With this data we use the formula for the Gregorian calendar.
 * 
 * @param   data          [in]  Pointer to data
 */
uint32_t WeekDay( const uint8_t *data )
{
    uint32_t dayofweek;
    uint32_t days;
    uint32_t month;
    uint32_t MSyear;
    uint32_t LSyear;
    uint32_t const correctdays[ NUM_7 ] = { HEX_5, HEX_6, HEX_0, HEX_1, HEX_2, HEX_3, HEX_4 };
    uint32_t year;
    uint32_t century;
    uint32_t yearcentury;

    days   = ( data[ NUM_2 ] >> NUM_4 ) & HEX_0F;
    days   = ( days * NUM_10 ) + ( data[ NUM_2 ] & HEX_0F );
    month  = ( data[ NUM_3 ] >> NUM_4 ) & HEX_0F;
    month  = ( month * NUM_10 ) + ( data[ NUM_3 ] & HEX_0F );
    MSyear = ( data[ NUM_4 ] >> NUM_4 ) & HEX_0F;
    MSyear = ( MSyear * NUM_10 ) + ( data[ NUM_4 ] & HEX_0F );
    LSyear = ( data[ NUM_5 ] >> NUM_4 ) & HEX_0F;
    LSyear = ( LSyear * NUM_10 ) + ( data[ NUM_5 ] & HEX_0F );

    year = ( MSyear * NUM_100 ) + LSyear;

    if( month < NUM_3 )
    {
        month += NUM_12;
        year--;
    }

    century     = year / NUM_100;
    yearcentury = year % NUM_100;

    dayofweek = ( days + ( ( NUM_13 * ( month + NUM_1 ) ) / NUM_5 ) + yearcentury + ( yearcentury / NUM_4 ) + ( century / NUM_4 ) + ( NUM_5 * century ) ) % NUM_7;

    dayofweek = correctdays[ dayofweek ];

    dayofweek++;

    return dayofweek;
}

/**
 * @brief   **Transmition in CAN-TP single frame format**
 *
 * This function transmit the ok or error messages through the terminal
 * when the data is already procesed, will be send 0x55 if the data was in range
 * and the 0xAA if the data was not in range with CAN-ID of 0x122
 * 
 * @param   data [in] Pointer to data
 * @param   size [in] Size of data
 */
static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size )
{
    for( uint8_t i = NUM_0; i < size; i++ )
    {
        data[ i + NUM_1 ] = data[ NUM_0 ];
    }

    data[ NUM_0 ] = size;

    HAL_FDCAN_AddMessageToTxFifoQ( &CANHandler, &CANTxHeader, data );
}

/**
 * @brief   **CAN-TP single frame format**
 *
 * The function validate if the message received 
 * complies with CAN-TP single frame format.
 *
 * @param   data        [in]     Pointer to data
 * @param   size        [in]     Size of data
 *
 * @retval  The function returns 1 when a certain number of bytes were received, 
 *          otherwise, no message was received
 */
static uint8_t CanTp_SingleFrameRx( const uint8_t *data, const uint8_t *size )
{
    uint8_t msgRecieve = NUM_0;

    if( ( ( *data == NUM_1 ) && ( *size == NUM_4 ) ) ||
        ( ( *data == NUM_2 ) && ( *size == NUM_5 ) ) ||
        ( ( *data == NUM_3 ) && ( *size == NUM_3 ) ) )
    {
        msgRecieve = NUM_1;
    }

    return msgRecieve;
}
