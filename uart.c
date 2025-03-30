#include "uart.h"


/*******************************************************************************
 * UART Initialization
 *
 * Configures UART2 with:
 * - 115200 baud rate (BRR = 35 for 4MHz clock)
 * - 8 data bits
 * - 1 stop bit
 * - No parity
 * - Receive interrupt enabled
 ******************************************************************************/
const char* COLOR_RED = "\x1B[31m";      // Sets text color to red
const char* COLOR_GREEN = "\x1B[32m";    // Sets text color to green
const char* COLOR_BLUE = "\x1B[34m";     // Sets text color to blue
const char* COLOR_WHITE = "\x1B[37m";    // Sets text color to white

volatile uint8_t receivedChar = 0;        // Stores the most recently received character
volatile uint8_t newDataReceived = 0;     // Flag indicating new data has arrived


void UART_init(void) {
    // Enable peripheral clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;      // Enable port A clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;   // Enable USART2 clock

    // Configure GPIO pins for UART
    // Set PA2 (TX) and PA3 (RX) to alternate function mode
    GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOA->MODER |= (2 << GPIO_MODER_MODE2_Pos) | (2 << GPIO_MODER_MODE3_Pos);

    // Set alternate function 7 (UART2) for PA2 and PA3
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
    GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_0);
    GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL3_2 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_0);

    // Initialize UART registers
    USART2->CR1 = 0;                           // Clear control registers
    USART2->CR2 = 0;
    USART2->CR3 = 0;
    USART2->BRR = 35;                          // Set baud rate for 115200

    // Enable UART features
    USART2->CR1 |= USART_CR1_RXNEIE;          // Enable receive interrupt
    USART2->CR1 |= (USART_CR1_UE |            // Enable UART
                    USART_CR1_TE |             // Enable transmitter
                    USART_CR1_RE);             // Enable receiver

    // Configure UART interrupt
    NVIC_EnableIRQ(USART2_IRQn);              // Enable UART2 interrupt in NVIC
    NVIC_SetPriority(USART2_IRQn, 0);         // Set highest priority
}


/* Print string to UART
 * Transmits each character until null terminator is reached
 */
void UART_print(const char* str) {
    while(*str) {
        transmitUART(*str++);
    }
}

/* Transmit single byte via UART
 * Waits for transmit buffer to be empty before sending
 */
void transmitUART(uint8_t data) {
    while(!(USART2->ISR & USART_ISR_TXE));    // Wait for empty transmit buffer
    USART2->TDR = data;                        // Send character
}


/*******************************************************************************
 * UART2 Interrupt Service Routine
 *
 * Triggered when:
 * - A new character is received (RXNE flag set)
 *
 * Actions:
 * - Reads received character
 * - Processes special color commands (R,G,B,W)
 * - Echoes standard characters back to terminal
 ******************************************************************************/
void USART2_IRQHandler(void) {
    if (USART2->ISR & USART_ISR_RXNE) {           // Check if new data received
        receivedChar = USART2->RDR;                // Read received character
        newDataReceived = 1;                       // Set new data flag

        // Process special color commands
        switch(receivedChar) {
            case 'R':
            	UART_print(COLOR_RED);          // Set text color to red
                break;
            case 'G':
            	UART_print(COLOR_GREEN);        // Set text color to green
                break;
            case 'B':
            	UART_print(COLOR_BLUE);         // Set text color to blue
                break;
            case 'W':
            	UART_print(COLOR_WHITE);        // Set text color to white
                break;
            default:
               // transmitUART(receivedChar);        // Echo regular characters
        }
    }
  }
