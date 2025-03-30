#ifndef INC_UART_H_
#define INC_UART_H_

/* Include STM32L4 HAL Library */
#include "stm32l4xx_hal.h"

/* Terminal Control Characters and Sequences */
#define CLEAR "\x1B[2J"           // ANSI escape sequence to clear screen
#define CURSORTL "\x1B[H"            // ANSI escape sequence to move cursor to top-left


extern const char* COLOR_RED;
extern const char* COLOR_GREEN;
extern const char* COLOR_BLUE;
extern const char* COLOR_WHITE;

/* Global variables for interrupt-driven UART reception */
extern volatile uint8_t receivedChar;       // Stores the most recently received character
extern volatile uint8_t newDataReceived;    // Flag indicating new data has arrived

/* Function Prototypes */
void UART_init(void);              // Initialize UART peripheral
void UART_print(const char* string);     // Send string over UART
void transmitUART(uint8_t data);

#endif /* INC_UART_H_ */
