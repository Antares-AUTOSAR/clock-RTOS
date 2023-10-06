/**
 * @file    can_tp.h
 * @brief   Board Support Package
 *
 * File provides the neccesary drivers, libraries, and configurations for the CAN BUS.
 *
 */
#ifndef CAN_TP_H_
#define CAN_TP_H_

#define MAX_ARRAY_SIZE 25 // Tamaño máximo permitido del arreglo

#include <stdint.h>
#include "bsp.h"

/**
 * @brief   struct store the elemnt saved the messages of the module FDCAN.
 */
typedef struct _CAN_MsgTypeDef
{

    uint8_t Messagetype; /*!< It represent the type de message, it can be TIME, DATE and ALARM*/
    uint8_t Data[ 7 ];

} CAN_MsgTypeDef;

/**
 * @brief   struct store the elemnt saved the messages of the module FDCAN.
 */
typedef struct _CAN_TP_Header
{

    FDCAN_HandleTypeDef *CANHandler;
    FDCAN_TxHeaderTypeDef *CANTxHeader;

    // Struct for Received
    uint32_t bufferSize;
    uint16_t data_length;
    uint8_t separationTime;
    uint8_t blockSize;

    CAN_MsgTypeDef *buffer_received;

    // Struct for Transmited
    uint32_t length;
    uint32_t number_counter;
    uint32_t multiplier_counter;
    uint32_t rest_counter;
    uint8_t flag_ready;
    uint8_t *buffer_transmited;

    uint32_t counter_newmessage;
    uint8_t message[ MAX_ARRAY_SIZE ];

} CAN_TP_Header;

extern void CAN_TP_Init( CAN_TP_Header *header );
extern void CAN_TP_Task( CAN_TP_Header *header );
extern void CAN_TP_Tick( void );
extern void CAN_TP_RxMessageBufferSet( CAN_TP_Header *header, uint8_t *buffer, uint32_t bufferSize );
extern void CAN_TP_RxSeparationTimeSet( CAN_TP_Header *header, uint8_t separationTime );
extern void CAN_TP_RxBlockSizeSet( CAN_TP_Header *header, uint8_t blockSize );
extern void CAN_TP_MessageSend( CAN_TP_Header *header, const uint8_t *data, uint32_t length );
extern uint8_t CAN_TP_IsMessageReady( const CAN_TP_Header *header );
extern void CAN_TP_MessageGet( CAN_TP_Header *header, uint8_t *data, uint8_t data_length );
extern void CAN_TP_NewMessage( CAN_TP_Header *header, void *buffer );

#endif
