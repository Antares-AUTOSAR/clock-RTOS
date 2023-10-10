/**
 * @file    can_tp.c
 * @brief   Board Support Package
 *
 * File provides the neccesary drivers, libraries, and configurations for the CAN BUS.
 *
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
void CAN_TP_MessageSend( CAN_TP_Header *header, const uint8_t *data, uint32_t length );

static void CAN_TP_TxTransmitted( CAN_TP_Header *header );
static void CAN_TP_RxReceived( CAN_TP_Header *header );

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
 * @brief Initializes the CAN_TP configuration for the CAN_TP_TASK
 *
 * Function necessary for work with normality
 *
 * @param header CAN_TP header data structure.
 */
void CAN_TP_Init( CAN_TP_Header *header )
{
    header->flag_transmitted = FLAG_CAN_TP_OFF;
}

/**
 * @brief Task process for CAN_TP.
 *
 * Function switches the CAN TP periodically if  it transmits or receives.
 *
 * @param header CAN_TP header data structure.
 */
void CAN_TP_Task( CAN_TP_Header *header )
{

    while( header->counter_newmessage != 0u )
    {

        CAN_TP_RxReceived( header );
        header->counter_newmessage--;
    }

    if( header->flag_transmitted == FLAG_CAN_TP_ON )
    {
        CAN_TP_TxTransmitted( header );
    }
}

/**
 * @brief Handles the received CAN_TP messages.
 *
 * This function processes the received CAN_TP messages, including flow control frames,
 * first frames, and consecutive frames. It updates the transmission state accordingly.
 *
 * @param header CAN_TP header data structure.
 */
static void CAN_TP_RxReceived( CAN_TP_Header *header )
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

                break;

            case FIRST_FRAME_TYPE:

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
 *
 * @param header CAN_TP header data structure.
 */
static void CAN_TP_TxTransmitted( CAN_TP_Header *header )
{

    static uint8_t state                   = FIRST_FRAME_TYPE;
    static uint8_t count_sequencenumber    = 1;
    static uint8_t count_multiplier_offset = 0;
    uint8_t counter_dlc                    = 0;
    switch( state )
    {

        case FIRST_FRAME_TYPE:

            counter_dlc            = 0;
            uint8_t message_f[ 8 ] = { 0 };

            if( header->length > 0xFFu )
            {

                message_f[ 0 ] = ( 0x10u ) | ( header->length >> 8u );
                message_f[ 1 ] = header->length;
            }
            else
            {

                message_f[ 0 ] = 0x10;
                message_f[ 1 ] = header->length;
            }

            while( counter_dlc < 6u )
            {

                message_f[ counter_dlc + 2u ] = header->buffer_transmited[ counter_dlc ];
                counter_dlc++;
            }

            HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_f );

            state = FLOW_CONTROL_FRAME_TYPE;

            break;
        case CONSECUTIVE_FRAME_TYPE:

            uint8_t message_c[ 8 ] = { 0 };

            if( ( count_sequencenumber < 17u ) && ( header->multiplier_counter > 0u ) )
            {
                counter_dlc    = 0;
                message_c[ 0 ] = 0x20u | count_sequencenumber;

                while( counter_dlc < 7u )
                {

                    message_c[ counter_dlc + 1u ] = header->buffer_transmited[ counter_dlc + OFFSET_FIRST_FRAME + ( ( count_sequencenumber - 1u ) * 7u ) + ( count_multiplier_offset * 16u * 7u ) ];
                    counter_dlc++;
                }


                HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_c );

                count_sequencenumber++;
                state = CONSECUTIVE_FRAME_TYPE;
            }

            if( ( count_sequencenumber == 17u ) && ( header->multiplier_counter != 0u ) )
            {

                count_sequencenumber = 1;
                header->multiplier_counter--;
                count_multiplier_offset++;
            }

            if( ( header->multiplier_counter == 0u ) && ( header->number_counter > 0u ) )
            {

                counter_dlc    = 0;
                message_c[ 0 ] = 0x20u | count_sequencenumber;

                while( counter_dlc < 7u )
                {

                    message_c[ counter_dlc + 1u ] = header->buffer_transmited[ counter_dlc + OFFSET_FIRST_FRAME + ( ( count_sequencenumber - 1u ) * 7u ) + ( count_multiplier_offset * 16u * 7u ) ];
                    counter_dlc++;
                }

                if( ( header->number_counter == count_sequencenumber ) )
                {

                    header->number_counter = 0;
                }

                HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_c );


                count_sequencenumber++;
                state = CONSECUTIVE_FRAME_TYPE;
            }

            if( ( header->multiplier_counter == 0u ) && ( header->number_counter == 0u ) )
            {

                if( header->rest_counter != 0u )
                {

                    uint32_t const defines_dlc[ 6 ] = { FDCAN_DLC_BYTES_2, FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5, FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7 };
                    counter_dlc                     = 0;

                    message_c[ 0 ] = 0x20u | count_sequencenumber;

                    while( counter_dlc < header->rest_counter )
                    {

                        message_c[ counter_dlc + 1u ] = header->buffer_transmited[ counter_dlc + OFFSET_FIRST_FRAME + ( ( count_sequencenumber - 1u ) * 7u ) + ( count_multiplier_offset * 16u * 7u ) ];
                        counter_dlc++;
                    }

                    header->CANTxHeader->DataLength = defines_dlc[ header->rest_counter - 1u ];

                    HAL_FDCAN_AddMessageToTxFifoQ( header->CANHandler, header->CANTxHeader, message_c );

                    header->CANTxHeader->DataLength = FDCAN_DLC_BYTES_8;
                }

                state = FIRST_FRAME_TYPE;

                header->flag_transmitted = FLAG_CAN_TP_OFF;

                count_sequencenumber = 1;

                count_multiplier_offset = 0;

                header->number_counter = 0;

                for( uint8_t i = 0; i < header->length; i++ )
                {

                    header->buffer_transmited[ i ] = 0;
                }
            }

            break;
        case FLOW_CONTROL_FRAME_TYPE:

            if( flag_can_tp_flowcontrol_status[ CONTINUE_TO_SEND ] == CTS_ON )
            {


                state = CONSECUTIVE_FRAME_TYPE;
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
 * @param header CAN_TP header data structure.
 * @param data Pointer to the message data.
 * @param length Length of the message.
 */
void CAN_TP_MessageSend( CAN_TP_Header *header, const uint8_t *data, uint32_t length )
{

    header->multiplier_counter = ( length - OFFSET_FIRST_FRAME ) / 512u; // 16
    if( header->multiplier_counter == 0u )
    {

        header->number_counter = ( length - OFFSET_FIRST_FRAME ) / 7u;

        if( header->number_counter > 0u )
        {

            header->rest_counter = ( length - OFFSET_FIRST_FRAME ) - ( 7u * header->number_counter );
        }
        else
        {

            header->rest_counter = length - OFFSET_FIRST_FRAME;
        }
    }
    else
    {

        header->rest_counter = ( length - OFFSET_FIRST_FRAME ) - ( 16u * 7u * header->multiplier_counter );

        header->number_counter = header->rest_counter / 7u;

        if( header->number_counter > 0u )
        {

            header->rest_counter = header->rest_counter - ( 7u * header->number_counter );
        }
    }

    header->buffer_transmited = data;

    header->length = length;

    header->flag_transmitted = FLAG_CAN_TP_ON;
}

/**
 * @brief Timing function for CAN_TP.
 */
void CAN_TP_Tick( void )
{
    static uint32_t count_tick = 0;
    count_tick++;
}

/**
 * @brief Handles a new message received by CAN_TP.
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