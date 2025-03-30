#ifndef INC_I2C_H_    // Header guard start
#define INC_I2C_H_

/* Include STM32L4 HAL Library */
#include "stm32l4xx_hal.h"

// GPIO Configuration Defines
#define AF              0b10        // Alternate Function mode value
#define AF4             0b0100      // AF4 value for I2C peripheral
#define TIME_FACTOR     0x00100D14  // I2C timing configuration for desired speed
#define DEV_ADDRESS     0b1010001   // EEPROM device address (0x51)
#define DONT_CARE_SHIFT 1           // Shift for 7-bit address alignment
#define SMALL_DELAY     1000        // Delay value for timing purposes
#define HIGH_SCORE_ADDR 0x8d1c
#define EEPROM_ADDR     0x51        // Default EEPROM address


// I2C Control Defines
#define I2C_START_BIT       1       // Value to generate START condition
#define I2C_WRITE_BYTES    4        // Total bytes for write operation (2 address + 2 data)
#define I2C_ADDR_BYTES     2        // Number of address bytes to send/receive
#define I2C_DATA_BYTE      1        // Number of data bytes to send/receive
#define I2C_ADDR_HIGH_SHIFT 8       // Shift value for high byte of address
#define I2C_ADDR_LOW_MASK  0xFF     // Mask for low byte of address
#define I2C_ADDR_SHIFT     1        // Shift for 7-bit address in I2C format

/* Function Prototypes */
void EEPROM_init(void);                     // Initialize I2C for EEPROM
uint16_t EEPROM_read(uint16_t address);      // Read byte from EEPROM address
void EEPROM_write(uint16_t address, uint16_t data);  // Write byte to EEPROM address

#endif /* INC_I2C_H_ */   // Header guard end
