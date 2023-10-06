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

#define SINGLE_FRAME_TYPE       0u /*!< MessageType */
#define FIRST_FRAME_TYPE        1u /*!< MessageType */
#define CONSECUTIVE_FRAME_TYPE  2u /*!< MessageType */
#define FLOW_CONTROL_FRAME_TYPE 3u /*!< MessageType */

#define CONTINUE_TO_SEND        0u /*!< MessageType */
#define WAIT                    1u /*!< MessageType */
#define OVERFLOW_ABORT          2u /*!< MessageType */

#define FLAG_CAN_TP_ON          1u /*!< MessageType */
#define FLAG_CAN_TP_OFF         0u /*!< MessageType */

#define CTS_ON                  1u /*!< MessageType */
#define CTS_OFF                 0u /*!< MessageType */
#define WAIT_ON                 1u /*!< MessageType */
#define WAIT_OFF                0u /*!< MessageType */
#define OVERFLOW_ABORT_ON       1u /*!< MessageType */
#define OVERFLOW_ABORT_OFF      0u /*!< MessageType */

#define OFFSET_FIRST_FRAME      6u /*!< MessageType */

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

static uint8_t flag_can_tp_transmitted             = FLAG_CAN_TP_OFF;
static uint8_t flag_can_tp_flowcontrol_status[ 3 ] = { CTS_OFF, WAIT_OFF, OVERFLOW_ABORT_OFF };

/**
 * @brief   Function initialize the setting FDCAN for the process Serial
 */
void CAN_TP_Init( CAN_TP_Header *header )
{

    (void)header;
}

/**
 * @brief   Function have a state machine for the process Serial of CASIO CAN can set its values
 *
 */
void CAN_TP_Task( CAN_TP_Header *header )
{

    while( header->counter_newmessage != 0u )
    {

        CAN_TP_RxReceived( header );
        header->counter_newmessage--;
    }

    if( flag_can_tp_transmitted == FLAG_CAN_TP_ON )
    {
        CAN_TP_TxTransmitted( header );
    }
}

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
                SEGGER_RTT_printf( 0, "FlowControlFrame_CONTINUE_TO_SEND_Received\n" );
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

                SEGGER_RTT_printf( 0, "Division: %u \n", division );

                if( division == 0u )
                {

                    header->data_length = 0;
                }

                if( header->data_length == 0u )
                {

                    offset_counter_sequence = 0;
                    header->flag_ready      = 1u;
                }
                SEGGER_RTT_printf( 0, "Data length: %d \n", header->data_length );
                SEGGER_RTT_printf( 0, "Flag_ready: %d \n", header->flag_ready );

                break;

            default:

                break;
        }
    }
}


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

            SEGGER_RTT_printf( 0, "FirstFrame\n" );

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
                SEGGER_RTT_printf( 0, "ConsecutiveFrame %d\n", count_sequencenumber );

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
                SEGGER_RTT_printf( 0, "ConsecutiveFrame %d\n", count_sequencenumber );

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

                    SEGGER_RTT_printf( 0, "ConsecutiveFrame rest counter bytes: %d\n", header->rest_counter );
                }

                state = FIRST_FRAME_TYPE;

                flag_can_tp_transmitted = FLAG_CAN_TP_OFF;

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

                SEGGER_RTT_printf( 0, "FlowControlFrame_CONTINUE_TO_SEND_Transmited\n" );

                state = CONSECUTIVE_FRAME_TYPE;
            }

            if( flag_can_tp_flowcontrol_status[ WAIT ] == WAIT_ON )
            {

                SEGGER_RTT_printf( 0, "FlowControlFrame_WAIT\n" );

                // PENDIENTE = WAIT;
                // xQueueSend(xQueue_CAN_SERIA, PENDIENTE , 0);
            }

            if( flag_can_tp_flowcontrol_status[ OVERFLOW_ABORT ] == OVERFLOW_ABORT_ON )
            {

                SEGGER_RTT_printf( 0, "FlowControlFrame_WAIT\n" );

                // PENDIENTE = OVERFLOW_ABORT;
                // xQueueSend(xQueue_CAN_SERIA, PENDIENTE , 0);
            }

            break;

        default:
            break;
    }
}

void CAN_TP_RxMessageBufferSet( CAN_TP_Header *header, uint8_t *buffer, uint32_t bufferSize )
{
    header->buffer_received = buffer;
    header->bufferSize      = bufferSize;
}

void CAN_TP_RxSeparationTimeSet( CAN_TP_Header *header, uint8_t separationTime )
{
    header->separationTime = separationTime;
}

void CAN_TP_RxBlockSizeSet( CAN_TP_Header *header, uint8_t blockSize )
{
    header->blockSize = blockSize;
}

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

    flag_can_tp_transmitted = FLAG_CAN_TP_ON;
}

void CAN_TP_Tick( void )
{
    static uint32_t count_tick = 0;
    count_tick++;
}

void CAN_TP_NewMessage( CAN_TP_Header *header, void *data )
{

    header->buffer_received = (CAN_MsgTypeDef *)data; /* cppcheck-suppress misra-c2012-11.5 ; f */
    header->counter_newmessage++;
}


uint8_t CAN_TP_IsMessageReady( const CAN_TP_Header *header )
{

    return header->flag_ready;
}

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