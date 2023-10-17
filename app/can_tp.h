/**
 * @file    can_tp.h
 * @brief   Board Support Package
 *
 * File provides the neccesary drivers, libraries, and configurations for the CAN BUS.
 *
 */
#ifndef CAN_TP_H_
#define CAN_TP_H_

#include "bsp.h"

#define MAX_ARRAY_SIZE 25 /*!<  Maximum allowed size for the array */

/**
 * @brief   struct store the element saved the messages of the module FDCAN for CAN Message Elements.
 */
typedef struct _CAN_MsgTypeDef
{

    uint8_t Messagetype; /*!< It represent the type de message, it can be TIME, DATE and ALARM*/
    uint8_t Data[ 7 ];   /*!< Data payload of the message. */

} CAN_MsgTypeDef;

/**
 * @brief   struct store the element saved the messages of the module FDCAN for CAN_TP Header information.
 */
typedef struct _CAN_TP_Header
{

    FDCAN_HandleTypeDef *CANHandler;    /*!< Pointer to FDCAN handler. */
    FDCAN_TxHeaderTypeDef *CANTxHeader; /*!< Pointer to FDCAN Tx Header. */

    // Struct for Received
    uint32_t bufferSize;    /*!< Size of the message buffer. */
    uint16_t data_length;   /*!< Length of the data in the received message. */
    uint8_t separationTime; /*!< Separation time for CAN_TP communication. */
    uint8_t blockSize;      /*!< Block size for CAN_TP communication. */
    uint8_t flag_ready;     /*!< Flag indicating if message is ready for received. */

    CAN_MsgTypeDef *buffer_received; /*!< Pointer to the received message buffer. */

    // Struct for Transmitted
    uint32_t length;                          /*!< Length of the transmitted message. */
    uint32_t sequencenumber_index;            /*!< Counter for transmitted message numbers. */
    uint32_t sequencenumber_index_counter;    /*!< Counter for transmitted message multipliers. */
    uint32_t last_sequencenumber_short_bytes; /*!< Counter for remaining bytes in transmitted message. */
    uint8_t *buffer_transmited;               /*!< Pointer to the transmitted message buffer. */
    uint8_t flag_transmitted;                 /*!< Flag indicating if the message has been transmitted. */

    uint32_t counter_newmessage;       /*!< Counter for new received messages. */
    uint8_t message[ MAX_ARRAY_SIZE ]; /*!< Array to store the received message. */


} CAN_TP_Header;

void CAN_TP_Init( CAN_TP_Header *header );
void CAN_TP_Periodic_Task( CAN_TP_Header *header );
void CAN_TP_Tick( void );
void CAN_TP_RxMessageBufferSet( CAN_TP_Header *header, uint8_t *buffer, uint32_t bufferSize );
void CAN_TP_RxSeparationTimeSet( CAN_TP_Header *header, uint8_t separationTime );
void CAN_TP_RxBlockSizeSet( CAN_TP_Header *header, uint8_t blockSize );
void CAN_TP_TransmitMessage( CAN_TP_Header *header, const uint8_t *data, uint32_t length );
uint8_t CAN_TP_IsMessageReady( const CAN_TP_Header *header );
void CAN_TP_MessageGet( CAN_TP_Header *header, uint8_t *data, uint8_t data_length );
void CAN_TP_NewMessage( CAN_TP_Header *header, void *buffer );

#endif
