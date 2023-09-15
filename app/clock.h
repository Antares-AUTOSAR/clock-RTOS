/**
 * @file    clock.h
 * @brief   ****
 *
 * Interfaces for clock process:
 *
 * void Clock_Init( void ) : Use once to Initialize the clock. Don't requires any parameter
 *
 * void Clock_Task( void ) : Run continuosuly. It allows to configure the clock
 *
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "bsp.h"


#define TOTAL_CLOCK_STATES 4U /*!< Total states in event machine */


/**
 * @brief   Struct for store function for each state of event machine
 */
typedef struct
{
    void ( *stateFunc )( APP_MsgTypeDef *receivedMessage ); /*!< Pointer to function which perform state statements */
} StateNode;


void Clock_Init( void );
void Clock_Task( void );
void Clock_Update_DateAndTime( void );


#endif