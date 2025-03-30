#include "i2c.h"

void EEPROM_init(void)
{
	/******************* Enable Clocks *******************/
	RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOBEN;      // Enable Port B Clock

	RCC -> APB1ENR1 |= RCC_APB1ENR1_I2C1EN;      // Enable I2C1 Clock

	/****************** Configure Pins *******************/

	GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);  // Clear MODER bits
	GPIOB -> MODER |= ( (AF << GPIO_MODER_MODE6_Pos) |	// Port B, Pin 6 to AF
			( AF << GPIO_MODER_MODE7_Pos) );			// Port B, Pin 7 to AF

	GPIOB -> AFR[0] |= (AF4 << GPIO_AFRL_AFSEL6_Pos) |	// AF connected to I2C1 SCL
			(AF4 << GPIO_AFRL_AFSEL7_Pos);				// AF connected to I2C1 SDA

	GPIOB -> OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7;  // PB6, PB7 output open-drain

	GPIOB -> OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;  //PB6, PB7 very high speed


	/****************** Configure I2C1 *******************/
	I2C1 -> CR1 &= ~I2C_CR1_PE;

	I2C1 -> TIMINGR = TIME_FACTOR;	// Configuring timing

	I2C1 -> CR1 |= (I2C_CR1_PE);	// Enabling I2C1 peripheral
}


void EEPROM_write(uint16_t address, uint16_t data)
{
	/****************** Configure I2C1 *******************/

    I2C1->CR2 &= ~(1 << I2C_CR2_START_Pos);     // Disable START


    I2C1->CR2 |= (I2C_CR2_AUTOEND);	    // STOP condition sent when NBYTES are transferred


    I2C1->CR2 &= ~(I2C_CR2_ADD10);      // 7 bit addressing mode


    I2C1->CR2 &= ~(I2C_CR2_SADD);
    I2C1->CR2 |= (DEV_ADDRESS << DONT_CARE_SHIFT);     // Set device address


    I2C1->CR2 &= ~(I2C_CR2_RD_WRN);       // Set write direction


    I2C1->CR2 &= ~(I2C_CR2_NBYTES);      // Set to write bytes ( 2 address + 2 data)
    I2C1->CR2 |= (I2C_WRITE_BYTES << I2C_CR2_NBYTES_Pos);

	/****************** Write Data *******************/

    I2C1->CR2 |= (I2C_START_BIT << I2C_CR2_START_Pos);      // start


    while(!(I2C1->ISR & I2C_ISR_TXIS));      // wait for flag
    I2C1->TXDR = address >> I2C_ADDR_HIGH_SHIFT;        // Send high byte of memory address


    while(!(I2C1->ISR & I2C_ISR_TXIS));       // Wait for TX buffer empty and ACK received
    I2C1->TXDR = address & I2C_ADDR_LOW_MASK;       // Send low byte of memory address


    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = (data >> 8) & 0xFF;  // Send high byte of data

    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = data & 0xFF;  // Send low byte of data
}

uint16_t EEPROM_read(uint16_t address)
{
    uint16_t data = 0;
    uint8_t high_byte, low_byte;

    // Write address first
    I2C1->CR2 = 0;
    I2C1->CR2 |= (DEV_ADDRESS << 1);
    I2C1->CR2 |= (2 << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;
    I2C1->CR2 |= I2C_CR2_START;

    // Send address bytes
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = (address >> 8) & 0xFF;

    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = address & 0xFF;

    // Wait for transfer complete
    while(!(I2C1->ISR & I2C_ISR_TC));

    // Now read the data
    I2C1->CR2 = 0;
    I2C1->CR2 |= (DEV_ADDRESS << 1);
    I2C1->CR2 |= (2 << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_RD_WRN;
    I2C1->CR2 |= I2C_CR2_START;

    // Read high byte
    while(!(I2C1->ISR & I2C_ISR_RXNE));
    high_byte = I2C1->RXDR;

    // Read low byte
    while(!(I2C1->ISR & I2C_ISR_RXNE));
    low_byte = I2C1->RXDR;

    // Combine bytes properly
    data = (uint16_t)high_byte << 8 | low_byte;

    return data;
}
