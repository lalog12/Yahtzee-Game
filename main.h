#ifndef INC_MAIN_H_
#define INC_MAIN_H_

/* Include STM32L4 HAL Library */
#include "stm32l4xx_hal.h"
#include <stdbool.h>


#define NUM_DICE 5
#define NUM_CATEGORIES 13
/* Define Functions */

typedef struct {
    uint8_t dice[NUM_DICE];
    bool hold[NUM_DICE];
    uint16_t scores[NUM_CATEGORIES];
    bool used[NUM_CATEGORIES];
    uint8_t rolls_left;
    uint8_t turns_left;
} GameState;

extern volatile uint16_t high_score;

/* Function Prototypes */
void Error_Handler(void);
void print_start_animation(void);              // Initialize UART peripheral
uint8_t dice_roll(void);  // Send ESC sequence followed by string
void roll_yahtzee_dice(GameState* game);
void display_dice_status(GameState* game);
void display_scorecard(GameState* game);
void play_yahtzee(void);
void select_category(GameState* game);
int calculate_score(uint32_t category, uint8_t dice[]);
void sort_dice(uint8_t dice[]);



#endif /* INC_UART_H_ */
