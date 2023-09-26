/**
 * @file    app_display.h
 * @brief   Interfaces for display process
 *
 */

#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#define DISPLAYS 2U /*!< Total states in event machine */

void Display_Init( void );
void Display_Task( void );

#endif