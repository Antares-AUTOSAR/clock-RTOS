#include "app_serial.h"

static void CanTp_SingleFrameTx( uint8_t *data, uint8_t size );
static uint8_t CanTp_SingleFrameRx( const uint8_t *data, const uint8_t *size );
static uint8_t Valid_Time( const uint8_t *data );
static uint8_t Valid_Alarm( const uint8_t *data );
static uint8_t Valid_Date( const uint8_t *data );
static uint32_t WeekDay( const uint8_t *data );
static void Serial_StMachine( const APP_MsgTypeDef *data );
void SerialTimeState( void );
void SerialDateState( void );
void SerialAlarmState( void );
void SerialOkState( void );
void SerialErrorState( void );


static uint8_t NewMessage[ NUM_8 ];

FDCAN_HandleTypeDef CANHandler;

static FDCAN_TxHeaderTypeDef CANTxHeader;

APP_MsgTypeDef MSGHandler;

QueueHandle_t ClockQueue;

void Serial_Init( void )
{
    /* Declaration of the options to configure the FDCAN1 module to transmit to the CAN bus at 100Kbps
       and sample point of 75%, the frequency with which the CAN module is powered is
       fCAN = fHSI / CANHandler.Init.ClockDivider / CANHandler.Init.NominalPrescaler
       fCAN = 32MHz / 1 / 8 = 4MHz
       The number of time quantas required is
       Ntq = fCAN / CANbaudrate
       Ntq = 4MHz / 250Kbps = 16
       The sample point percentage is
       Sp = ( CANHandler.Init.NominalTimeSeg1 +  1 / Ntq ) * 100
       Sp = ( ( 11 + 1 ) / 16 ) * 100 = 75%
    */

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

/* cppcheck-suppress misra-c2012-8.4 ; this is a library function */
void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
{
    FDCAN_RxHeaderTypeDef CANRxHeader;

    if( ( RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE ) != NUM_0 )
    {
        /* Retrieve Rx messages from RX FIFO0 */
        HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, NewMessage );

        xQueueSendFromISR( SerialQueue, &NewMessage, NULL );
    }
}