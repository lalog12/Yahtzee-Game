#include "main.h"
#include "uart.h"
#include "rng.h"
#include "i2c.h"


#define NUM_DICE 5
#define NUM_CATEGORIES 13

void SystemClock_Config(void);

volatile uint16_t high_score = 0;

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  EEPROM_init();

  UART_init();

  RNG_init();


  while (1)
  {


	  print_start_animation();

	  play_yahtzee();
  }

}

uint8_t dice_roll(void) {
    uint8_t roll_value;
    roll_value = (RNG_retrieve() % 6) + 1;  // Get number 1-6



    UART_print(CLEAR);                      // Clear screen
    UART_print(CURSORTL);                   // Move cursor to top-left
    UART_print("*********ROLLING THE DIE*********");
    UART_print("\r\n\r\n");

    // Display final dice face
    UART_print("+-------+");
    UART_print("\r\n");

    switch(roll_value) {
        case 1:
            UART_print("|       |");
            UART_print("\r\n");
            UART_print("|   O   |");
            UART_print("\r\n");
            UART_print("|       |");
            break;
        case 2:
            UART_print("| O     |");
            UART_print("\r\n");
            UART_print("|       |");
            UART_print("\r\n");
            UART_print("|     O |");
            break;
        case 3:
            UART_print("| O     |");
            UART_print("\r\n");
            UART_print("|   O   |");
            UART_print("\r\n");
            UART_print("|     O |");
            break;
        case 4:
            UART_print("| O   O |");
            UART_print("\r\n");
            UART_print("|       |");
            UART_print("\r\n");
            UART_print("| O   O |");
            break;
        case 5:
            UART_print("| O   O |");
            UART_print("\r\n");
            UART_print("|   O   |");
            UART_print("\r\n");
            UART_print("| O   O |");
            break;
        case 6:
            UART_print("| O   O |");
            UART_print("\r\n");
            UART_print("| O   O |");
            UART_print("\r\n");
            UART_print("| O   O |");
            break;
    }
    UART_print("\r\n");
    UART_print("+-------+");
    UART_print("\r\n");

    HAL_Delay(200);  // Show die briefly

    return roll_value;
}


void print_start_animation(void) {
    UART_print(CLEAR);
    // Clear screen
    UART_print(CURSORTL);
    HAL_Delay(500);  // Initial pause

    // First frame - Game title appearing
    UART_print("*********");
    HAL_Delay(200);
    UART_print("YAHTZEE");
    HAL_Delay(200);
     UART_print("*********");
    UART_print("\r\n\r\n");
    HAL_Delay(500);

    // Animated dice roll
    const char* rolling_frames[] = {
        "  Loading...\r\n"
        "   .------.\r\n"
        "  /      /|\r\n"
        " /      / |\r\n"
        "+------+  |\r\n"
        "|      |  +\r\n"
        "|      | /\r\n"
        "+------+/\r\n",

        "  Loading...\r\n"
        "   .------.\r\n"
        "  /  o o /|\r\n"
        " /  o   / |\r\n"
        "+------+  |\r\n"
        "|  o   |  +\r\n"
        "|    o | /\r\n"
        "+------+/\r\n",

        "  Loading...\r\n"
        "   .------.\r\n"
        "  / o  o /|\r\n"
        " /   o  / |\r\n"
        "+------+  |\r\n"
        "| o  o |  +\r\n"
        "|  o   | /\r\n"
        "+------+/\r\n"
    };

    // Show rolling animation
    for(int i = 0; i < 4; i++) {
        for(int frame = 0; frame < 3; frame++) {
            UART_print(CURSORTL);
            UART_print("*********YAHTZEE*********");
            UART_print("\r\n\r\n");
            UART_print(rolling_frames[frame]);
            HAL_Delay(200);
        }
    }


    UART_print("\r\n\r\n");

    // Final message
    UART_print("Press any key to start...");

    newDataReceived = 0;
    while(newDataReceived == 0);

    UART_print(CURSORTL); // Reset cursor position
}


void display_yahtzee_menu(void) {
    UART_print(CLEAR);
    UART_print(CURSORTL);
    UART_print("*********YAHTZEE*********\r\n\r\n");
    UART_print("[1] Roll Dice\r\n");
    UART_print("[2] Hold/Unhold Dice\r\n");
    UART_print("[3] Choose Category\r\n");
    UART_print("[4] View Scorecard\r\n");
    UART_print("[5] Exit Game\r\n\r\n");
}

void roll_yahtzee_dice(GameState* game) {
    for(int i = 0; i < NUM_DICE; i++) {
        if(!game->hold[i]) {
            game->dice[i] = dice_roll();
        }
    }
    game->rolls_left--;
}

void display_dice_status(GameState* game) {
    UART_print("\r\nCurrent Dice: ");
    for(int i = 0; i < NUM_DICE; i++) {
        char num = game->dice[i] + '0';
        transmitUART(num);
        UART_print(" ");
    }

    UART_print("\r\nHold Status:  ");
    for(int i = 0; i < NUM_DICE; i++) {
        UART_print(game->hold[i] ? "H " : "- ");
    }
    UART_print("\r\n");
}

void display_scorecard(GameState* game) {
    char num_str[5];
    const char* categories[] = {
        "Ones", "Twos", "Threes", "Fours", "Fives", "Sixes",
        "3 of a Kind", "4 of a Kind", "Full House",
        "Small Straight", "Large Straight", "Yahtzee", "Chance"
    };

    UART_print(CLEAR);
    UART_print(CURSORTL);
    UART_print("*********SCORECARD*********\r\n\r\n");

    for(int i = 0; i < NUM_CATEGORIES; i++) {
        UART_print(categories[i]);
        UART_print(": ");
        if(game->used[i]) {
            num_str[0] = (game->scores[i]/10) + '0';
            num_str[1] = (game->scores[i]%10) + '0';
            num_str[2] = '\0';
            UART_print(num_str);
        } else {
            UART_print("--");
        }
        UART_print("\r\n");
    }

    // Display total
    uint16_t total = 0;

    for(int i = 0; i < NUM_CATEGORIES; i++) {

        if(game->used[i]) total += game->scores[i];
    }

    // Reading high score (2 bytes)
    high_score = EEPROM_read(HIGH_SCORE_ADDR);


    if (total > high_score) {
    	high_score = total;
        // Writing high score (2 bytes)
        EEPROM_write(HIGH_SCORE_ADDR, total);    // High byte
        HAL_Delay(5);

    }

    UART_print("\r\nTotal Score: ");
    // Convert number to string digits manually
    num_str[0] = (total / 100) + '0';     // Hundreds place
    num_str[1] = (total / 10 % 10) + '0'; // Tens place
    num_str[2] = (total % 10) + '0';      // Ones place
    num_str[3] = '\0';                    // String terminator
    UART_print(num_str);
    UART_print("\r\n");

    num_str[0] = (high_score / 100) + '0';     // Hundreds place
    num_str[1] = (high_score / 10 % 10) + '0'; // Tens place
    num_str[2] = (high_score % 10) + '0';      // Ones place
    num_str[3] = '\0';                    // String terminator
    UART_print("\r\nHigh Score: ");
    UART_print(num_str);
    UART_print("\r\n");


}

void play_yahtzee(void) {
    GameState game = {0};
    game.rolls_left = 3;
    game.turns_left = NUM_CATEGORIES;
    char choice;

    while(game.turns_left > 0) {
        display_yahtzee_menu();

        // Show game status
        UART_print("Rolls left: ");
        transmitUART(game.rolls_left + '0');
        UART_print("\r\nTurns left: ");
        // Convert turns_left to string
        char num_str[3];
        num_str[0] = (game.turns_left / 10) + '0';  // Tens digit
        num_str[1] = (game.turns_left % 10) + '0';  // Ones digit
        num_str[2] = '\0';                          // String terminator
        UART_print(num_str);
        UART_print("\r\n\n");

        display_dice_status(&game);

        // Get player choice
        UART_print("\r\nEnter choice: ");
        newDataReceived = 0;
        while(!newDataReceived);
        choice = receivedChar;

        switch(choice) {
            case '1':
                if(game.rolls_left > 0) {
                    roll_yahtzee_dice(&game);
                } else {
                    UART_print("\r\nNo rolls left! Choose a category.\r\n");
                    HAL_Delay(1000);
                }
                break;

            case '2':
                UART_print("\r\nEnter die number (1-5): ");
                newDataReceived = 0;
                while(!newDataReceived);
                choice = receivedChar;
                if(choice >= '1' && choice <= '5') {
                    int die = choice - '1';
                    game.hold[die] = !game.hold[die];
                }
                break;

            case '3':
                if(game.rolls_left < 3) {  // Only allow scoring if dice have been rolled
                    select_category(&game);
                    game.rolls_left = 3;
                    game.turns_left--;
                    for(int i = 0; i < NUM_DICE; i++) {
                        game.hold[i] = false;
                        game.dice[i] = 0;
                    }
                } else {
                    UART_print("\r\nYou must roll at least once!\r\n");
                    HAL_Delay(1000);
                }
                break;

            case '4':
                display_scorecard(&game);
                UART_print("\r\nPress any key to continue...");
                newDataReceived = 0;
                while(!newDataReceived);
                break;

            case '5':
                return;
        }
    }

    // Game over
    display_scorecard(&game);

    UART_print("\r\nGame Over!\r\n");
    UART_print("Press any key to exit...");  // Add this line
    newDataReceived = 0;                     // Add this line
    while(!newDataReceived);                 // Add this line
    HAL_Delay(2000);
}

// Add this implementation
void select_category(GameState* game) {
    UART_print("\r\nSelect category:\r\n");
    const char* categories[] = {
        "1. Ones", "2. Twos", "3. Threes", "4. Fours", "5. Fives", "6. Sixes",
        "7. Three of a Kind", "8. Four of a Kind", "9. Full House",
        "10. Small Straight", "11. Large Straight", "12. Yahtzee", "13. Chance"
    };

    // Display available categories
    for(int i = 0; i < NUM_CATEGORIES; i++) {
        if(!game->used[i]) {
            UART_print(categories[i]);
            UART_print("\r\n");
        }
    }

    // Get player choice
    UART_print("\r\nEnter category number: ");
    newDataReceived = 0;
    while(!newDataReceived);
    uint32_t choice = receivedChar - '0';

    // For numbers 10-13, wait for second digit
    if(choice == 1) {
        UART_print("1");  // Echo first digit
        newDataReceived = 0;
        while(!newDataReceived);  // Wait for second digit
        uint32_t second_digit = receivedChar - '0';
        if(second_digit >= 0 && second_digit <= 3) {
            choice = 10 + second_digit;
            char echo_buffer[2] = {receivedChar, '\0'};
            UART_print(echo_buffer);  // Echo second digit
        }
    }

    choice--; // Convert to 0-based index

    if(choice >= 0 && choice < NUM_CATEGORIES && !game->used[choice]) {
        game->scores[choice] = calculate_score(choice, game->dice);
        game->used[choice] = true;
    }
}

int calculate_score(uint32_t category, uint8_t dice[]) {
    int score = 0;
    int counts[6] = {0}; // Track count of each dice value

    // Count occurrences of each value
    for(int i = 0; i < NUM_DICE; i++) {
        counts[dice[i]-1]++;
    }

    switch(category) {
        case 0: // Ones
        case 1: // Twos
        case 2: // Threes
        case 3: // Fours
        case 4: // Fives
        case 5: // Sixes
            return counts[category] * (category + 1);

        case 6: // Three of a kind
            for(int i = 0; i < 6; i++) {
                if(counts[i] >= 3) {
                    for(int j = 0; j < NUM_DICE; j++) {
                        score += dice[j];
                    }
                    return score;
                }
            }
            break;

        case 7: // Four of a kind
            for(int i = 0; i < 6; i++) {
                if(counts[i] >= 4) {
                    for(int j = 0; j < NUM_DICE; j++) {
                        score += dice[j];
                    }
                    return score;
                }
            }
            break;

        case 8: // Full house
            {
                bool has_three = false;
                bool has_two = false;
                for(int i = 0; i < 6; i++) {
                    if(counts[i] == 3) has_three = true;
                    if(counts[i] == 2) has_two = true;
                }
                if(has_three && has_two) return 25;
            }
            break;

        case 9: // Small straight
            sort_dice(dice);
            for(int i = 0; i < 2; i++) {
                if(dice[i]+1 == dice[i+1] &&
                   dice[i+1]+1 == dice[i+2] &&
                   dice[i+2]+1 == dice[i+3]) {
                    return 30;
                }
            }
            break;

        case 10: // Large straight
            sort_dice(dice);
            if(dice[0]+1 == dice[1] &&
               dice[1]+1 == dice[2] &&
               dice[2]+1 == dice[3] &&
               dice[3]+1 == dice[4]) {
                return 40;
            }
            break;

        case 11: // Yahtzee
            for(int i = 0; i < 6; i++) {
                if(counts[i] == 5) return 50;
            }
            break;

        case 12: // Chance
            for(int i = 0; i < NUM_DICE; i++) {
                score += dice[i];
            }
            return score;
    }
    return 0;
}

void sort_dice(uint8_t dice[]) {
    for(int i = 0; i < NUM_DICE-1; i++) {
        for(int j = 0; j < NUM_DICE-i-1; j++) {
            if(dice[j] > dice[j+1]) {
                int temp = dice[j];
                dice[j] = dice[j+1];
                dice[j+1] = temp;
            }
        }
    }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
