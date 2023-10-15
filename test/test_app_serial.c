#include "unity.h"
#include "app_serial.h"
#include "bsp.h"

#include "FreeRTOS.h"
#include "mock_queue.h"


void setUp( void )
{
}

void tearDown( void )
{
}

/* Define queue for clock */
QueueHandle_t clockQueue;

void serialInit__testInit( void )
{
    Serial_Init( );
}
