#include "RNG.h"

void RNG_init(void){
/* Configure PLL */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;       // clear PLL source

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;    // set MSI as PLL source

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM |
    		RCC_PLLCFGR_PLLN |
			RCC_PLLCFGR_PLLQ); // clear PLL config bits

    RCC->PLLCFGR |= (PLL_M << RCC_PLLCFGR_PLLM_Pos) | // PLLM divider = 2
    (PLL_N << RCC_PLLCFGR_PLLN_Pos) | // PLLN multiplier = 12
    (PLL_Q << RCC_PLLCFGR_PLLQ_Pos); // PLLQ divider = 2

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN; // enable the PLL48M1CLK output of the main PLL

    RCC->CR |= RCC_CR_PLLON;            // turn on the PLL

    while(!(RCC->CR & RCC_CR_PLLRDY));  // wait until PLL is ready

    /* configure 48MHz clk powering RNG */
    RCC->CCIPR &= ~RCC_CCIPR_CLK48SEL;

    // clear prev source
    RCC->CCIPR |= (PLL48M1CLK << RCC_CCIPR_CLK48SEL_Pos); // 48MHz clk source = PLL48M1CLK

    /* enable RNG */
    RCC->AHB2ENR|= RCC_AHB2ENR_RNGEN; // enable AHB Clk

    RNG->CR |= RNG_CR_RNGEN;  // enable RNG

    RNG->SR &= ~(RNG_SR_CEIS | RNG_SR_SEIS); // clear previous error flags
}


uint32_t RNG_retrieve(void){

	//check for seed error or clock error
	if((RNG->SR & RNG_SR_SEIS) | (RNG->SR & RNG_SR_CEIS)){

	return ERROR;
	}

	while(!(RNG->SR & RNG_SR_DRDY)); //wait for data to be ready

	uint32_t random_num = RNG->DR;   //read data

	//check that RN generated is valid and return
	if(!random_num){
		return ERROR;
	}

	else{
		return random_num;
	}
}
