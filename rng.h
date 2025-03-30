#ifndef INC_RNG_H_
#define INC_RNG_H_


/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private macro -------------------------------------------------------------*/
#define ERROR -1
#define RST_VAL 0
#define PLL_M 1
#define PLL_N 12
#define PLL_Q 2
#define PLL48M1CLK 2

/* Private function prototypes -----------------------------------------------*/
void RNG_init(void);
uint32_t RNG_retrieve(void);


#endif /* INC_RNG_H_ */
