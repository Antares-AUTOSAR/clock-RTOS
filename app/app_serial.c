/**
 * @file    app_serial.c
 * @brief   **Message processing implementation**
 *
 * Contains the functions to initialize the CAN port with the apropiate parameters,
 * the callback to the CAN interruption, also the main state machine to message processing were
 * the CAN frame will be evaluate to aprove it or not and to be assigned to the struct
 * variable by the fuction in charge of it.
 *
 */
#include "app_serial.h"
#include "bsp.h"

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size );
static uint8_t Validate_Time( uint8_t hour, uint8_t minutes, uint8_t seconds );
static uint8_t Validate_Alarm( uint8_t hour, uint8_t minutes );
static uint8_t Validate_Date( uint8_t days, uint8_t month, uint16_t year );
static uint32_t WeekDay( uint8_t days, uint8_t month, uint16_t year );
static void Serial_StMachine( NEW_MsgTypeDef *pdata );
static void SerialTimeState( const NEW_MsgTypeDef *pmsg );
static void SerialDateState( const NEW_MsgTypeDef *pmsg );
static void SerialAlarmState( const NEW_MsgTypeDef *pmsg );
static void SerialOkState( const NEW_MsgTypeDef *pmsg );
static void SerialErrorState( const NEW_MsgTypeDef *pmsg );
static uint8_t BCD_conver( uint8_t data );

FDCAN_HandleTypeDef CANHandler;

/**
 * @brief Struct with pointer to function
 */
typedef struct SerialStates
{
    void ( *ptr_funct )( const NEW_MsgTypeDef *pdata ); /*!< Pointer to function */
} SerialStates;

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
        HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, &NewMessage.Data[ SINGLE_FRAME_ELEMENT ] );

        xQueueSendFromISR( serialQueue, &NewMessage.Data[ SINGLE_FRAME_ELEMENT ], NULL );
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

    while( xQueueReceive( serialQueue, &RecieveMsg.Data, 0 ) == pdTRUE )
    {
        if( CanTp_SingleFrameRx( (uint8_t *)RecieveMsg.Data, &RecieveMsg.Data[ SINGLE_FRAME_ELEMENT ] ) == TRUE )
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
    NextEvent.Data[ NUM_0 ] = pdata->Data[ NUM_0 ];

    StMachine[ NextEvent.Data[ NUM_0 ] - NUM_1 ].ptr_funct( pdata );
}

/**
 * @brief   **Serial time function**
 *
 * Is going to evaluate if the data for time is valid
 * and send the message to the queue.
 *
 * @param   pmsg  [in]  Pointer to data struct
 */
void SerialTimeState( const NEW_MsgTypeDef *pmsg )
{
    static NEW_MsgTypeDef SerialMsg;
    static APP_MsgTypeDef messageStruct = { 0 };

    SerialMsg.Data[ MESSAGE ]  = ERROR_STATE;
    messageStruct.tm.tm_hour = BCD_conver( pmsg->Data[ TIME_HOUR_ELEMENT ] );
    messageStruct.tm.tm_min  = BCD_conver( pmsg->Data[ TIME_MIN_ELEMENT ] );
    messageStruct.tm.tm_sec  = BCD_conver( pmsg->Data[ TIME_SEC_ELEMENT ] );

    if( Validate_Time( messageStruct.tm.tm_hour, messageStruct.tm.tm_min, messageStruct.tm.tm_sec ) == TRUE )
    {

        messageStruct.msg = SERIAL_MSG_TIME;
        xQueueSend( clockQueue, &messageStruct, TICKS );
        SerialMsg.Data[ MESSAGE ] = OK_STATE;
    }

    /*Send a message to the same queue to trasition to another state*/
    xQueueSend( serialQueue, &SerialMsg, TICKS );
}

/**
 * @brief   **Serial date function**
 *
 * Is going to evaluate if the data for date is valid
 * and send the message to the queue.
 *
 * @param   pmsg  [in]  Pointer to data struct
 */
void SerialDateState( const NEW_MsgTypeDef *pmsg )
{
    static NEW_MsgTypeDef SerialMsg;
    static APP_MsgTypeDef messageStruct = { 0 };

    SerialMsg.Data[ MESSAGE ]  = ERROR_STATE;
    messageStruct.tm.tm_mday = BCD_conver( pmsg->Data[ DATE_DAY_ELEMENT ] );
    messageStruct.tm.tm_mon  = BCD_conver( pmsg->Data[ DATE_MON_ELEMENT ] );
    messageStruct.tm.tm_year = ( ( BCD_conver( pmsg->Data[ DATE_MSB_YEAR_ELEMENT ] ) * NUM_100 ) + BCD_conver( pmsg->Data[ DATE_LSB_YEAR_ELEMENT ] ) );

    if( Validate_Date( messageStruct.tm.tm_mday, messageStruct.tm.tm_mon, messageStruct.tm.tm_year ) == TRUE )
    {
        messageStruct.msg        = SERIAL_MSG_DATE;
        messageStruct.tm.tm_wday = WeekDay( messageStruct.tm.tm_mday, messageStruct.tm.tm_mon, messageStruct.tm.tm_year );

        xQueueSend( clockQueue, &messageStruct, TICKS ); /*Send the data*/
        SerialMsg.Data[ MESSAGE ] = OK_STATE;
    }

    /*Send a message to the same queue to trasition to another state*/
    xQueueSend( serialQueue, &SerialMsg, TICKS );
}

/**
 * @brief   **Serial alarm function**
 *
 * Is going to evaluate if the data for alarm is valid
 * and send the message to the queue.
 *
 * @param   pmsg  [in]  Pointer to data struct
 */
void SerialAlarmState( const NEW_MsgTypeDef *pmsg )
{
    static NEW_MsgTypeDef SerialMsg;
    static APP_MsgTypeDef messageStruct = { 0 };

    SerialMsg.Data[ MESSAGE ]        = ERROR_STATE;
    messageStruct.tm.tm_hour_alarm = BCD_conver( pmsg->Data[ ALARM_HOUR_ELEMENT ] );
    messageStruct.tm.tm_min_alarm  = BCD_conver( pmsg->Data[ ALARM_MIN_ELEMENT ] );

    if( Validate_Alarm( messageStruct.tm.tm_hour_alarm, messageStruct.tm.tm_min_alarm ) == TRUE )
    {
        messageStruct.msg = SERIAL_MSG_ALARM;

        xQueueSend( clockQueue, &messageStruct, TICKS );
        SerialMsg.Data[ MESSAGE ] = OK_STATE;
    }

    /*Send a message to the same queue to trasition to another state*/
    xQueueSend( serialQueue, &SerialMsg, TICKS );
}

/**
 * @brief   **Serial ok function**
 *
 * Is going to assign the data to the variable clockMsg
 * and send it to the queue of the clock, then it will transmit
 * a message through CAN to indicate the data is valid.
 *
 * @param   pmsg  [in]  Pointer to data struct
 */
void SerialOkState( const NEW_MsgTypeDef *pmsg )
{
    uint8_t msg_ok[ 8 ] = { 0 };

    msg_ok[ NUM_0 ] = DATA_OK;
    CanTp_SingleFrameTx( msg_ok, pmsg->Data[ SINGLE_FRAME_ELEMENT ] );
}

/**
 * @brief   **Serial error function**
 *
 * Is going to transmit a message through CAN to indicate the data is not valid.
 *
 * @param   pmsg  [in]  Pointer to data struct
 */
void SerialErrorState( const NEW_MsgTypeDef *pmsg )
{
    uint8_t msn_error[ 8 ] = { 0 };

    msn_error[ NUM_0 ] = DATA_ERROR;
    CanTp_SingleFrameTx( msn_error, pmsg->Data[ SINGLE_FRAME_ELEMENT ] );
}

/**
 * @brief BCD converter
 * Function to unpacked from CAN-TP single frame format to BCD
 *
 * @param   data  [in]  value to convert
 *
 * @retval 	Returns an unsigned integer
 */
static uint8_t BCD_conver( uint8_t data )
{
    uint8_t BCDdata = 0u;

    BCDdata = ( ( ( data >> 4 ) * (uint8_t)10 ) + ( data & (uint8_t)0x0F ) );
    return BCDdata;
}

/**
 * @brief   **Validate time**
 *
 * This function validates if the time message is in range,
 * from 0 to 23 for hours, from 0 to 59 for minutes and
 * from 0 to 59 for seconds and returns the result.
 *
 * @param   hour          [in]  Hour value
 * @param   minutes       [in]  Minutes value
 * @param   seconds       [in]  Seconds value
 *
 * @retval  The function returns 1 if time is correct and 0 if not
 */
static uint8_t Validate_Time( uint8_t hour, uint8_t minutes, uint8_t seconds )
{
    uint8_t ret_val = NUM_0;

    if( ( hour <= MAX_HOUR_HEX ) &&
        ( minutes <= MAX_MIN_HEX ) &&
        ( seconds <= MAX_SEC_HEX ) )
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
 * @param   hour          [in]  Hour value
 * @param   minutes       [in]  Minutes value
 *
 * @retval  The function returns 1 if the alarm is correct and 0 if not
 */
static uint8_t Validate_Alarm( uint8_t hour, uint8_t minutes )
{
    uint8_t ret_val = NUM_0;

    if( ( hour <= MAX_HOUR_HEX ) &&
        ( minutes <= MAX_SEC_HEX ) )
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
 * @param   days          [in]  Day value
 * @param   month         [in]  Month value
 * @param   year          [in]  Year value
 *
 * @retval  The function returns 1 if the date is correct and 0 if not
 */
static uint8_t Validate_Date( uint8_t days, uint8_t month, uint16_t year )
{
    uint8_t ret_val;

    if( ( ( days >= MIN_DAY_HEX ) && ( days <= MAX_DAYS_HEX_31 ) ) &&
        ( ( month >= JANUARY ) && ( month <= DECEMBER ) ) &&
        ( ( year >= MIN_YEAR_HEX ) && ( year <= MAX_YEAR_HEX ) ) )
    {
        if( ( year % NUM_4 ) == NUM_0 ) /* Check leap year and february*/
        {
            if( ( month == FEBRUARY ) &&
                ( days <= DAYS_IN_LEAP_YEAR_FERUARY_HEX ) )
            {
                ret_val = NUM_1;
            }
            else
            {
                ret_val = NUM_0;
            }
        }
        else if( ( month == FEBRUARY ) &&
                 ( days <= DAYS_IN_FERUARY_HEX ) ) /* Check for february*/
        {
            ret_val = NUM_1;
        }
        else if( ( ( month == APRIL ) ||
                   ( month == JUNE ) ||
                   ( month == SEPTEMBER ) ||
                   ( month == NOVEMBER ) ) &&
                 ( days <= MAX_DAYS_HEX_30 ) ) /* Check for months with 30 days*/
        {
            ret_val = NUM_1;
        }
        else if( ( month == JANUARY ) ||
                 ( month == MARCH ) ||
                 ( month == MAY ) ||
                 ( month == JULY ) ||
                 ( month == AUGUST ) ||
                 ( month == OCTOBER ) ||
                 ( month == DECEMBER ) ) /* Otherwise, the month has 31 days*/
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
 * @param   days          [in]  Day value
 * @param   month         [in]  Month value
 * @param   year          [in]  Year value
 *
 * @retval  The function returns the values 0-Saturday, 1-Sunday, 2-Monday,
 *          3-Tuesday, 4-Wednesday, 5-Thursday, 6-Friday
 */
uint32_t WeekDay( uint8_t days, uint8_t month, uint16_t year )
{
    /*Zeller algorithm*/
    uint16_t aux         = ( 14u - (uint16_t)month ) / 12u;
    uint16_t yearZeller  = year - aux;
    uint16_t monthZeller = month + ( 12u * aux ) - 2u;
    uint16_t diaSemana   = 0;

    diaSemana = ( ( days + yearZeller + ( yearZeller / 4u ) - ( yearZeller / 100u ) + ( yearZeller / 400u ) + ( 31u * monthZeller ) / 12u ) % 7u );

    return (uint8_t)diaSemana;
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

    if( ( size > (uint8_t)0 ) && ( size < (uint8_t)8 ) )
    {
        for( uint8_t i = 7; i >= (uint8_t)1; i-- )
        {
            data[ i ] = data[ i - 1u ];
        }
        data[ 0 ] = size;
        HAL_FDCAN_AddMessageToTxFifoQ( &CANHandler, &CANTxHeader, data );
    }
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
static uint8_t CanTp_SingleFrameRx( uint8_t *data, uint8_t *size )
{
    uint8_t flagB0 = 0u;

    if( ( ( data[ 0 ] >> 4 ) == (uint8_t)0 ) && ( data[ 0 ] < (uint8_t)8 ) )
    {
        *size = data[ 0 ] & (uint8_t)0x0F;
        for( uint8_t i = 0; i < (uint8_t)7; i++ )
        {
            data[ i ] = ( data[ i + 1u ] );
        }
        flagB0 = 1;
    }
    else
    {
        flagB0 = 0;
    }

    return flagB0;
}
