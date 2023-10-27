/**
 * @file    can_tp.c
 * @brief   Board Support Package
 *
 * File provides the neccesary drivers, libraries, and configurations for the CAN BUS.
 *
 @verbatim
 ==============================================================================
                    ##### How to use this driver #####
 ==============================================================================

 Interruption necessary:

 - HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
 - HAL_FDCAN_TxFifoEmptyCallback( FDCAN_HandleTypeDef *hfdcan )

 In the interrupcion HAL_FDCAN_RxFifo0Callback, you need to put the function
 CAN_TP_NewMessage( CAN_TP_Header *header, void *data );

 For example:

    void HAL_FDCAN_RxFifo0Callback( FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs )
    {

        FDCAN_RxHeaderTypeDef CANRxHeader;

        static uint8_t RxData[ 8 ] = { 0 };

        HAL_FDCAN_GetRxMessage( hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, RxData );

        CAN_TP_NewMessage( &features_packet, (void *)RxData );

    }


 In the interrupcion HAL_FDCAN_TxFifoEmptyCallback, you need to put the function
 void CAN_TP_TransmitInterruptMessage( CAN_TP_Header *header );

 For example:

    void HAL_FDCAN_TxFifoEmptyCallback( FDCAN_HandleTypeDef *hfdcan )
    {
        CAN_TP_TransmitInterruptMessage(&features_packet);
    }

 Important!!! You must select the size of message that your bufferRx will receive:

 #define MAX_ARRAY_SIZE 25

 For the receiving this is a example:

    static CAN_TP_Header features_packet;

    void vTask1( void *pvParameters )
    {
        uint8_t rxBuffer[ 22 ]  = { 0 };
        uint8_t messageRx[ 22 ] = { 0 };
        uint8_t value           = 1u;

        CAN_TP_Init( &features_packet );

        CAN_TP_RxBlockSizeSet( &features_packet, 10 );

        CAN_TP_RxSeparationTimeSet( &features_packet, 10 );

        CAN_TP_RxMessageBufferSet( &features_packet, rxBuffer, sizeof( rxBuffer ) );

        for( ;; )
        {
            CAN_TP_Periodic_Task( &features_packet );

            if( value == CAN_TP_IsMessageReady( &features_packet ) )
            {

                CAN_TP_MessageGet( &features_packet, messageRx, sizeof( messageRx ) );

                SEGGER_RTT_printf( 0, "Message:" );

                for( uint8_t i = 0; i < sizeof( messageRx ); i++ )
                {

                    SEGGER_RTT_printf( 0, " %x", messageRx[ i ] );
                }

                SEGGER_RTT_printf( 0, "\n" );
            }

            vTaskDelay( 100 );
        }

        (void)pvParameters;
    }

 For the transmision this is a example:
  @endverbatim
 */


#include "bsp.h"
#include "can_tp.h"
#include <string.h>

#define SINGLE_FRAME_TYPE       0u /*!< Frame type to mark a single frame */
#define FIRST_FRAME_TYPE        1u /*!< Frame type to mark the first frame */
#define CONSECUTIVE_FRAME_TYPE  2u /*!< Frame type to mark consecutive frame */
#define FLOW_CONTROL_FRAME_TYPE 3u /*!< Frame type to mark the flow control frame */

#define CONTINUE_TO_SEND        0u /*!< Indicate the flow control flag 0 */
#define WAIT                    1u /*!< Indicate the flow control flag 1 */
#define OVERFLOW_ABORT          2u /*!< Indicate the flow control flag 2 */

#define FLAG_CAN_TP_ON          1u /*!< The flag indicates when CAN TP is ready to transmit a message */
#define FLAG_CAN_TP_OFF         0u /*!< The flag indicates when CAN TP is not ready to transmit a message */

#define CTS_ON                  1u /*!< Indicate the number when the flow control flag 0 is ready */
#define CTS_OFF                 0u /*!< Indicate the number when the flow control flag 0 is not ready */
#define WAIT_ON                 1u /*!< Indicate the number when the flow control flag 1 is ready */
#define WAIT_OFF                0u /*!< Indicate the number when the flow control flag 1 is not ready */
#define OVERFLOW_ABORT_ON       1u /*!< Indicate the number when the flow control flag 2 is ready */
#define OVERFLOW_ABORT_OFF      0u /*!< Indicate the number when the flow control flag 2 is not ready */

#define OFFSET_FIRST_FRAME      6u /*!< Offset data of the first frame because it transmitted 6 bytes of data */

void CAN_TP_Tick( void );
void CAN_TP_RxMessageBufferSet( CAN_TP_Header *header, uint8_t *buffer, uint32_t bufferSize );
void CAN_TP_RxSeparationTimeSet( CAN_TP_Header *header, uint8_t separationTime );
void CAN_TP_RxBlockSizeSet( CAN_TP_Header *header, uint8_t blockSize );

void CAN_TP_NewMessage( CAN_TP_Header *header, void *data );
void CAN_TP_TransmitMessage( CAN_TP_Header *header, const uint8_t *data, uint32_t length );

static void CAN_TP_TxTransmit_Period_Task( CAN_TP_Header *header );
static void CAN_TP_RxReceive_Period_Task( CAN_TP_Header *header );

uint8_t CAN_TP_IsMessageReady( const CAN_TP_Header *header );
void CAN_TP_MessageGet( CAN_TP_Header *header, uint8_t *data, uint8_t data_length );

/**
 * @brief Array to store CAN_TP flow control status flags.
 *
 * This array holds the status flags indicating the flow control conditions
 * for CAN_TP transmission. Index 0 represents 'Continue to Send' (CTS),
 * index 1 represents 'Wait', and index 2 represents 'Overflow Abort'.
 * Initial values are set to indicate OFF states for all conditions.
 */
static uint8_t flag_can_tp_flowcontrol_status[ 3 ] = { CTS_OFF, WAIT_OFF, OVERFLOW_ABORT_OFF };


/**
 * @brief Initializes the CAN_TP configuration for the  CAN_TP_Periodic_Task
 *
 * Function necessary for work with normality
 *
 * @param header CAN_TP header data structure.
 */
void CAN_TP_Init( CAN_TP_Header *header )
{
    header->flag_transmitted  = FLAG_CAN_TP_OFF;
    header->flag_interruption = FLAG_CAN_TP_OFF;
}

/**
 * @brief Task process for CAN_TP.
 *
 * Function switches the CAN TP periodically if  it transmits or receives.
 *
 * @param header CAN_TP header data structure.
 */
void CAN_TP_Periodic_Task( CAN_TP_Header *header )
{

    while( header->counter_newmessage != 0u )
    {
        // Si el buffer Rx esta HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo);
        // Enviar un WAIT y en el transmisor debera de transmitir otra vez el ultimo packete de data
        // Si el buffer no recibe en el tiempo estimado los mensages, es overflow/abort
        CAN_TP_RxReceive_Period_Task( header );
        header->counter_newmessage--;
    }

    if( ( header->flag_transmitted == FLAG_CAN_TP_ON ) && ( header->flag_interruption == FLAG_CAN_TP_OFF ) )
    {
        CAN_TP_TxTransmit_Period_Task( header );
    }
}

/**
 * @brief Handles the received CAN_TP messages.
 *
 * This function processes the received CAN_TP messages, including flow control frames,
 * first frames, and consecutive frames. It updates the transmission state accordingly.
 * Also save the decoded message from the reception.
 *
 * @param header CAN_TP header data structure.
 */
static void CAN_TP_RxReceive_Period_Task( CAN_TP_Header *header )
{

    uint8_t message_type = header->buffer_received->Messagetype >> 4;

    if( message_type == FLOW_CONTROL_FRAME_TYPE )
    {
        uint8_t event_flowcontrol = 0;

        event_flowcontrol = header->buffer_received->Messagetype << 4;
        event_flowcontrol = event_flowcontrol >> 4;


        switch( event_flowcontrol )
        {
            case CONTINUE_TO_SEND:

                flag_can_tp_flowcontrol_status[ CONTINUE_TO_SEND ] = CTS_ON;
                break;
            case WAIT:

                flag_can_tp_flowcontrol_status[ WAIT ] = WAIT_ON;
                break;
            case OVERFLOW_ABORT:

                flag_can_tp_flowcontrol_status[ OVERFLOW_ABORT ] = OVERFLOW_ABORT_ON;
                break;

            default:

                break;
        }
    }
    else
    {

        uint8_t counter_dlc = 0;

        switch( message_type )
        {

            case SINGLE_FRAME_TYPE:

                while( counter_dlc < 7u )
                {

                    header->message[ counter_dlc ] = header->buffer_received->Data[ counter_dlc ];
                    counter_dlc++;
                }

                header->flag_ready = 1u;

                break;

            case FIRST_FRAME_TYPE:

                uint32_t value_buffer_tx = HAL_FDCAN_GetTxFifoFreeLevel( header->CANHandler );

                if( value_buffer_tx > 0u )
                {
                    uint8_t message_flowcontrol[ 3 ] = { 0x30, 0x00, 0x00 };

                    header->data_length = header->buffer_received->Messagetype << 12;
                    header->data_length = ( header->data_length >> 4 ) | header->buffer_received->Data[ 1 ];

                    while( counter_dlc < 6u )
                    {

                        header->message[ counter_dlc ] = header->buffer_received->Data[ counter_dlc + 1u ];
                        counter_dlc++;
                    }

                    header->data_length = header->data_length - 6u;

                    header->CANTxHeader->DataLength = FDCAN_DLC_BYTES_3;

                    HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_flowcontrol );

                    header->CANTxHeader->DataLength = FDCAN_DLC_BYTES_8;
                }

                break;
            case CONSECUTIVE_FRAME_TYPE:

                static uint8_t offset_counter_sequence = 0;

                uint8_t division = 0;

                while( counter_dlc < 7u )
                {

                    header->message[ counter_dlc + 6u + ( offset_counter_sequence * 7u ) ] = header->buffer_received->Data[ counter_dlc ];
                    counter_dlc++;
                }

                offset_counter_sequence++;

                header->data_length = header->data_length - 7u;

                division = header->data_length / 7u;

                if( division == 0u )
                {

                    header->data_length = 0;
                }

                if( header->data_length == 0u )
                {

                    offset_counter_sequence = 0;
                    header->flag_ready      = 1u;
                }

                break;

            default:

                break;
        }
    }
}


/**
 * @brief Transmits CAN_TP messages based on the current state.
 *
 * This function manages the transmission of CAN_TP messages, including first frames
 * and consecutive frames. It handles the flow control logic and updates the state
 * for transmission.
 * To make the transmissions, right shifts are made depending on the type of frame for
 * the general data bytes of the frame and in the data bytes a while is used to extract
 * the message sent by the user and encode it in the corresponding variable.
 *
 * @param header CAN_TP header data structure.
 */
static void CAN_TP_TxTransmit_Period_Task( CAN_TP_Header *header )
{

    static uint8_t state                   = FIRST_FRAME_TYPE;
    static uint8_t count_sequencenumber    = 1;
    static uint8_t count_multiplier_offset = 0;
    uint8_t counter_dlc                    = 0;

    uint32_t value_buffer_tx = HAL_FDCAN_GetTxFifoFreeLevel( header->CANHandler );

    if( header->length < 8u )
    {
        state = SINGLE_FRAME_TYPE;
    }

    switch( state )
    {

        case SINGLE_FRAME_TYPE:

            if( value_buffer_tx > 0u )
            {
                counter_dlc                       = 0; // data length code (DLC)
                uint8_t message_single_frame[ 8 ] = { 0 };
                message_single_frame[ 0 ]         = header->length;

                while( counter_dlc < 7u )
                {
                    message_single_frame[ counter_dlc + 1u ] = header->buffer_transmited[ counter_dlc ];
                    counter_dlc++;
                }

                HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_single_frame );

                state = FIRST_FRAME_TYPE;

                header->flag_transmitted = FLAG_CAN_TP_OFF;
            }

            break;

        case FIRST_FRAME_TYPE:

            if( value_buffer_tx > 0u )
            {
                counter_dlc                      = 0; // data length code (DLC)
                uint8_t message_first_frame[ 8 ] = { 0 };

                if( header->length > 0xFFu ) // Case when the length is bigger that 255 so it need to use 12 bits.
                {

                    message_first_frame[ 0 ] = ( 0x10u ) | ( header->length >> 8u ); // The 12 bits in the byte 0 only save 4 bits.
                    message_first_frame[ 1 ] = header->length;                       // The 8 bits rest save in the byte 1.
                }
                else
                {

                    message_first_frame[ 0 ] = 0x10;
                    message_first_frame[ 1 ] = header->length;
                }

                while( counter_dlc < 6u )
                {

                    message_first_frame[ counter_dlc + 2u ] = header->buffer_transmited[ counter_dlc ];
                    counter_dlc++;
                }

                HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_first_frame );

                state = FLOW_CONTROL_FRAME_TYPE;
            }

            break;
        case CONSECUTIVE_FRAME_TYPE:

            uint8_t counter_i = 0;

            while( counter_i < value_buffer_tx )
            {
                uint8_t message_consecutive_frame[ 8 ] = { 0 };

                if( ( count_sequencenumber < 17u ) && ( header->sequencenumber_index_counter > 0u ) )
                {
                    counter_dlc                    = 0;
                    message_consecutive_frame[ 0 ] = 0x20u | count_sequencenumber;

                    while( counter_dlc < 7u )
                    {

                        message_consecutive_frame[ counter_dlc + 1u ] = header->buffer_transmited[ counter_dlc + OFFSET_FIRST_FRAME + ( ( count_sequencenumber - 1u ) * 7u ) + ( count_multiplier_offset * 16u * 7u ) ];
                        counter_dlc++;
                    }


                    HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_consecutive_frame );

                    count_sequencenumber++;
                    state = CONSECUTIVE_FRAME_TYPE;
                }

                if( ( count_sequencenumber == 17u ) && ( header->sequencenumber_index_counter != 0u ) )
                {

                    count_sequencenumber = 1;
                    header->sequencenumber_index_counter--;
                    count_multiplier_offset++;
                }

                if( ( header->sequencenumber_index_counter == 0u ) && ( header->sequencenumber_index > 0u ) )
                {

                    counter_dlc                    = 0;
                    message_consecutive_frame[ 0 ] = 0x20u | count_sequencenumber;

                    while( counter_dlc < 7u )
                    {

                        message_consecutive_frame[ counter_dlc + 1u ] = header->buffer_transmited[ counter_dlc + OFFSET_FIRST_FRAME + ( ( count_sequencenumber - 1u ) * 7u ) + ( count_multiplier_offset * 16u * 7u ) ];
                        counter_dlc++;
                    }

                    if( ( header->sequencenumber_index == count_sequencenumber ) )
                    {

                        header->sequencenumber_index = 0;
                    }

                    HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_consecutive_frame );


                    count_sequencenumber++;
                    state = CONSECUTIVE_FRAME_TYPE;
                }

                if( ( header->sequencenumber_index_counter == 0u ) && ( header->sequencenumber_index == 0u ) )
                {

                    if( header->last_sequencenumber_short_bytes != 0u )
                    {

                        uint32_t const defines_dlc[ 6 ] = { FDCAN_DLC_BYTES_2, FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5, FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7 };
                        counter_dlc                     = 0;

                        message_consecutive_frame[ 0 ] = 0x20u | count_sequencenumber;

                        while( counter_dlc < header->last_sequencenumber_short_bytes )
                        {

                            message_consecutive_frame[ counter_dlc + 1u ] = header->buffer_transmited[ counter_dlc + OFFSET_FIRST_FRAME + ( ( count_sequencenumber - 1u ) * 7u ) + ( count_multiplier_offset * 16u * 7u ) ];
                            counter_dlc++;
                        }

                        header->CANTxHeader->DataLength = defines_dlc[ header->last_sequencenumber_short_bytes - 1u ];

                        HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_consecutive_frame );

                        header->CANTxHeader->DataLength = FDCAN_DLC_BYTES_8;
                    }

                    state = FIRST_FRAME_TYPE;

                    header->flag_transmitted = FLAG_CAN_TP_OFF;

                    header->flag_interruption = FLAG_CAN_TP_OFF;

                    count_sequencenumber = 1;

                    count_multiplier_offset = 0;

                    header->sequencenumber_index = 0;

                    for( uint8_t i = 0; i < header->length; i++ )
                    {

                        header->buffer_transmited[ i ] = 0;
                    }
                }

                counter_i++;
            }

            break;
        case FLOW_CONTROL_FRAME_TYPE:

            if( flag_can_tp_flowcontrol_status[ CONTINUE_TO_SEND ] == CTS_ON )
            {


                state                     = CONSECUTIVE_FRAME_TYPE;
                header->flag_interruption = FLAG_CAN_TP_ON;
                header->flag_transmitted  = FLAG_CAN_TP_OFF;
                HAL_FDCAN_TxFifoEmptyCallback( header->CANHandler );
            }

            if( flag_can_tp_flowcontrol_status[ WAIT ] == WAIT_ON )
            {
            }

            if( flag_can_tp_flowcontrol_status[ OVERFLOW_ABORT ] == OVERFLOW_ABORT_ON )
            {
            }

            break;

        default:
            break;
    }
}

/**
 * @brief Sets the received message buffer and its size.
 *
 * Function to indicate the size and memory space of the Rx
 * buffer where the messages received from the data transmission will be saved.
 *
 * @param header CAN_TP header data structure.
 * @param buffer Received message buffer.
 * @param bufferSize Size of the message buffer.
 *
 */
void CAN_TP_RxMessageBufferSet( CAN_TP_Header *header, uint8_t *buffer, uint32_t bufferSize )
{
    header->buffer_received = buffer;
    header->bufferSize      = bufferSize;
}


/**
 * @brief Sets the separation time between received messages.
 *
 * Function to indicate the separation time of message transmission
 * for the Rx buffer. If kept at 0, no waiting limits will apply between
 * received messages.
 *
 * @param header CAN_TP header data structure.
 * @param separationTime Separation time between messages.
 */
void CAN_TP_RxSeparationTimeSet( CAN_TP_Header *header, uint8_t separationTime )
{
    header->separationTime = separationTime;
}

/**
 * @brief Sets the block size for received messages.
 *
 * Function to indicate the blocks that will be received in the
 * transmission of messages. If kept at 0, message limits will not
 * be taken into account for transmission.
 *
 * @param header CAN_TP header data structure.
 * @param blockSize Block size.
 */
void CAN_TP_RxBlockSizeSet( CAN_TP_Header *header, uint8_t blockSize )
{
    header->blockSize = blockSize;
}

/**
 * @brief Initiates the transmission of a CAN_TP_Transmited message.
 *
 * The function analyzes whether to send a single frame or multiple consecutive frames
 * depending on the length of the data. In the case of multiple consecutive frames, the
 * number of consecutive frames that will be sent is calculated.
 *
 * @param header CAN_TP header data structure.
 * @param data Pointer to the message data.
 * @param length Length of the message.
 */
void CAN_TP_TransmitMessage( CAN_TP_Header *header, const uint8_t *data, uint32_t length )
{

    if( header->flag_transmitted != FLAG_CAN_TP_ON )
    {
        if( length > 7u )
        {

            header->sequencenumber_index_counter = ( length - OFFSET_FIRST_FRAME ) / 512u; // 16

            if( header->sequencenumber_index_counter == 0u )
            {

                header->sequencenumber_index = ( length - OFFSET_FIRST_FRAME ) / 7u;

                if( header->sequencenumber_index > 0u )
                {

                    header->last_sequencenumber_short_bytes = ( length - OFFSET_FIRST_FRAME ) - ( 7u * header->sequencenumber_index );
                }
                else
                {

                    header->last_sequencenumber_short_bytes = length - OFFSET_FIRST_FRAME;
                }
            }
            else
            {

                header->last_sequencenumber_short_bytes = ( length - OFFSET_FIRST_FRAME ) - ( 16u * 7u * header->sequencenumber_index_counter );

                header->sequencenumber_index = header->last_sequencenumber_short_bytes / 7u;

                if( header->sequencenumber_index > 0u )
                {

                    header->last_sequencenumber_short_bytes = header->last_sequencenumber_short_bytes - ( 7u * header->sequencenumber_index );
                }
            }
        }

        header->buffer_transmited = data;

        header->length = length;

        header->flag_transmitted = FLAG_CAN_TP_ON;
    }
}

/**
 * @brief Timing function for CAN_TP.
 *
 * Function that counts CAN TP ticks to ensure separation time and
 * sending of a wait, overflow/abort.
 *
 */
void CAN_TP_Tick( void )
{
    static uint32_t count_tick = 0;
    count_tick++;
}

/**
 * @brief Handles a new message received by CAN_TP.
 *
 * Function to implement in HAL_FDCAN_RxFifo0Callback. This function
 * tells us when to enter CAN_TP_RxReceive_Period_Task
 *
 * @param header CAN_TP header data structure.
 * @param data Pointer to the received message data.
 */
void CAN_TP_NewMessage( CAN_TP_Header *header, void *data )
{

    header->buffer_received = (CAN_MsgTypeDef *)data; /* cppcheck-suppress misra-c2012-11.5 ; f */
    header->counter_newmessage++;
}

/**
 * @brief Checks if the message is ready to be processed.
 *
 * Function that tells us if we can read the complete message received through CAN_TP
 *
 * @param header CAN_TP header data structure.
 * @return 1 if the message is ready, 0 otherwise.
 */
uint8_t CAN_TP_IsMessageReady( const CAN_TP_Header *header )
{

    return header->flag_ready;
}

/**
 * @brief Gets the processed message.
 *
 * Function to extract all the message received by CAN TP into
 * a variable and then delete all the message received in the
 * Rx buffer.
 *
 * @param header CAN_TP header data structure.
 * @param data Pointer to the message data.
 * @param data_length Length of the data to retrieve.
 */
void CAN_TP_MessageGet( CAN_TP_Header *header, uint8_t *data, uint8_t data_length )
{

    for( uint8_t i = 0; i < data_length; i++ )
    {

        data[ i ] = header->message[ i ]; /* cppcheck-suppress misra-c2012-17.8 ; f */
    }

    for( uint8_t i = 0; i < sizeof( header->message ); i++ )
    {

        header->message[ i ] = 0;
    }

    header->flag_ready = 0u;
}

/**
 * @brief Message transmission with interruption.
 *
 * Function that calls CAN_TP_TxTransmit_Period_Task when the flag_interruption is activated,
 * this happens when a flowcontrol typeframe CTS is received.
 *
 * @param header CAN_TP header data structure.

 */
void CAN_TP_TransmitInterruptMessage( CAN_TP_Header *header )
{

    if( header->flag_interruption == FLAG_CAN_TP_ON )
    {
        CAN_TP_TxTransmit_Period_Task( header );
    }
}