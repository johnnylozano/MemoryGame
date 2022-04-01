/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
int timer_keep = 0;
int initialDelay = 1;
int start = 1;
int userInput = 0;
int turn = 0;
int chance = 0;
int currentPosition = 0;
int victory = 0;
int resetGame = 0;
int comp_array[] = {0, 0, 0, 0, 0};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);		//clock initialization
  htim2.Instance -> CCR1 = 150;					//sets motor angle to 0 degrees
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  /**
	 	    ******************************************************************************
	 	    * COMPUTER TURN
	 	    * Game starts with computer's turn. First, an array is randomized and stored.
	 	    * Next, the computer performs a sequence of motor movements. LED light will
	 	    * light up whenever the computer is moving, and and the motor arm will point
	 	    * towards the player and LED will cut off indicating the computer's turn
	 	    * is over.
	 	    ******************************************************************************
	 	    */
	 	  HAL_Delay(1000);
	 	  if (start == 1)
	 	  {
	 		  /**
	 		    ******************************************************************************
	 		    * Array Randomizer for computer
	 		    ******************************************************************************
	 		    */
	 		  for (int i=0; i<5; i++)
	 		  {
	 			  comp_array[i] = (rand() % 5) + 1;				//fills array with number 1-5

	 			  if (i > 0)
	 			  {
	 				  if (comp_array[i] == comp_array[i-1])		//checks if adjacent value is the same
	 				  {
	 					  i--;									//if they're the same, redo the randomization
	 				  }
	 			  }
	 		  }


	 		  /**
	 		    ******************************************************************************
	 		    * Computer Motor Movement
	 		    * 	Checks for stored computer array values and cycles through
	 		    * 	motor sequence. LED light will be on while computer is moving.
	 		    * 	Motor will reset and LED light will turn off indicating it is the
	 		    * 	end of the computer's turn.
	 		    ******************************************************************************
	 		    */
	 		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 		  HAL_Delay(100);
	 		  for (int i=0; i<5; i++)
	 		  {
	 		  	  if (comp_array[i] == 1) {						//checks stored computer array values
	 		  		  htim2.Instance -> CCR1 = 130;				//affects motor
	 		  	  }
	 		  	  else if (comp_array[i] == 2) {
	 		  	  		  htim2.Instance -> CCR1 = 110;
	 		  	  	  }
	 		  	  else if (comp_array[i] == 3) {
	 		  	  		  htim2.Instance -> CCR1 = 90;
	 		  	  	  }
	 		  	  else if (comp_array[i] == 4) {
	 		  	  		  htim2.Instance -> CCR1 = 70;
	 		  	  	  }
	 		  	  else if (comp_array[i] == 5) {
	 		  	  		  htim2.Instance -> CCR1 = 50;
	 		  	  	  }
	 		  	  else if (comp_array[i] >= 6) {
	 		  		  htim2.Instance -> CCR1 = 30;
	 		  	  }
	 		  	  else											//fail safe just in case
	 		  	  {
	 		  		comp_array[i] = 0;
	 		  	  }

	 		  	  HAL_Delay(2000);
	 		  }
	 		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	 		  htim2.Instance -> CCR1 = 30;
	 		  userInput = 1;
	 		  start = 0;
	 		}


	 	  /**
	 	    ******************************************************************************
	 	    * USER TURN
	 	    * 	Awaits for user input, moves motor accordingly, and compares input
	 	    * 	to computer generated sequence.
	 	    * 		Wrong input: repositions motor arm to YOUR TURN and flashes led
	 	    * 		and increments chance. After three unsuccessful chances, ends while
	 	    * 		loop and goes to defeat condition.
	 	    *
	 	    * 		Successful input: motor arm moves and stays in correct position.
	 	    * 		Turn will increment and after all 5 turns are successfully matched,
	 	    * 		moves on to victory condition.
	 	    *
	 	    * 		Invalid input: LED flashes and movement is disregarded.
	 	    ******************************************************************************
	 	    */
	 	  while (userInput == 1)
	 	  {
	 		  for (turn=0; turn < 5;)
	 		  {
	 			  if (timer_keep > 0)		/*keeps the system from messing up. If an interrupt occurs when
	 			  						the timer = 1 but the while loop is at if timer = 3, then the
	 			  						timer variable gets reset without actually doing anything.
	 			  						This if statement is my solution to that*/
	 			  {
	 			  	  if (timer_keep == 1) {						//checks timer value
	 			  		  htim2.Instance -> CCR1 = 130;				//affects motor
	 			  	  }
	 			  	  else if (timer_keep == 2) {
	 			  	  		  htim2.Instance -> CCR1 = 110;
	 			  	  	  }
	 			  	  else if (timer_keep == 3) {
	 			  	  		  htim2.Instance -> CCR1 = 90;
	 			  	  	  }
	 			  	  else if (timer_keep == 4) {
	 			  	  		  htim2.Instance -> CCR1 = 70;
	 			  	  	  }
	 			  	  else if (timer_keep == 5) {
	 			  	  		  htim2.Instance -> CCR1 = 50;
	 			  	  	  }
	 			  	  else if (timer_keep >= 6) {
	 					  for (int i = 0; i < 10; i++)
	 					  {
	 						  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 						  HAL_Delay(100);
	 						  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	 						  HAL_Delay(100);
	 					  }
	 					  goto DISREGARD;							//If input is above 6, led will flash and movement is ignored
	 			  	  }
	 			  	  else											//fail safe just in case
	 			  	  {
	 			  		  timer_keep = 0;
	 			  	  }
	 				  /**
	 				    ******************************************************************************
	 				    * Compares player input to computer sequence. If incorrect, motor arm resets
	 				    * and LED flashes for 3 seconds. If correct, moves on to next turn and awaits
	 				    * input.
	 				    ******************************************************************************
	 				    */
	 			  	  if (timer_keep != comp_array[turn])
	 			  	  {
	 			  		  chance = chance + 1;
	 			  		  if (chance < 3)							//Determines if motor arm should reset
	 			  		  {
	 			  			HAL_Delay(500);
	 			  			htim2.Instance -> CCR1 = 30;
	 						  for (int i = 0; i < 15; i++)
	 						  {
	 							  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 							  HAL_Delay(100);
	 							  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	 							  HAL_Delay(100);
	 						  }
	 			  		  }

	 					  if (chance > 2)							//User "loses" after three attempts
	 					  {
	 						  userInput = 0;
	 						  turn = 5;
	 						  currentPosition = timer_keep;
	 					  }
	 			  	  }
	 			  	  else
	 			  	  {
	 			  		  turn = turn + 1;							//turn moves up if correct input
	 			  		  if (turn == 5)							//moves on to the win condition if all 5 are matched
	 			  		  {
	 			  			  currentPosition = timer_keep;
	 			  			  victory = 1;
	 			  			  userInput = 0;
	 			  		  }
	 			  	  }
	 DISREGARD:

	 				  timer_keep = 0;			//timer is reset after motor function
	 				  initialDelay = 1;			//delay is turned back on
	 			  }

	 		  }
	 	  }

	 	  /**
	 	    ******************************************************************************
	 	    * Defeat if statement. LED will flash, and motor will move slowly towards
	 	    * player and back to start.
	 	    ******************************************************************************
	 	    */
	 	  if (chance == 3)
	 	  {
	 		  /**
	 		    ******************************************************************************
	 		    * 10Hz delay for 3 sec
	 		    ******************************************************************************
	 		    */
	 		  for (int i = 0; i < 15; i++)
	 		  {
	 			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	 			  HAL_Delay(100);
	 			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	 			  HAL_Delay(100);
	 		  }

	 		  /**
	 		   ******************************************************************************
	 		   * User Defeat motor movement
	 		   ******************************************************************************
	 		   */
	 		  if (currentPosition == 1)
	 		  {
	 			  htim2.Instance -> CCR1 = 130;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 120;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 50;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 40;
	 		  	  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 30;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 40;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 50;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		  }
	 		  if (currentPosition == 2)
	 		  {
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 50;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 40;
	 		  	  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 30;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 40;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 50;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		  }
	 		  if (currentPosition == 3)
	 		  {
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 50;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 40;
	 		  	  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 30;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 40;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 50;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		  }
	 		  if (currentPosition == 4)
	 		  {
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 50;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 40;
	 		  	  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 30;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 40;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 50;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		  }
	 		  if (currentPosition == 5)
	 		  {

	 			  htim2.Instance -> CCR1 = 50;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		  }
	 		  chance = 0;
	 		  resetGame = 1;
	 	  }//if chance ==3

	 	  /**
	 	   ******************************************************************************
	 	   * User Victory motor movement
	 	   ******************************************************************************
	 	   */
	 	  if (victory == 1)
	 	  {
	 		HAL_Delay(200);
	 		if (currentPosition == 1)
	 		{
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		}
	 		if (currentPosition == 2)
	 		{
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		}
	 		if (currentPosition == 3)
	 		{
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		}
	 		if (currentPosition == 4)
	 		{
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		}
	 		if (currentPosition == 5)
	 		{
	 			  htim2.Instance -> CCR1 = 50;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 60;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 70;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 80;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 90;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 100;
	 			  HAL_Delay(150);
	 			  htim2.Instance -> CCR1 = 110;
	 			  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 120;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 130;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 140;
	 		  	  HAL_Delay(150);
	 		  	  htim2.Instance -> CCR1 = 150;
	 		  	  HAL_Delay(150);
	 		}
	 		victory = 0;
	 		resetGame = 1;
	 	  }	//end else (chance == 3)

	 	  /**
	 	   ******************************************************************************
	 	   * Resets the game
	 	   ******************************************************************************
	 	   */
	 	  if (resetGame == 1)
	 	  {
	 		  timer_keep = 0;
	 		  start = 1;
	 		  userInput = 0;
	 		  turn = 0;
	 		  chance = 0;
	 		  currentPosition = 0;
	 		  victory = 0;
	 		  resetGame = 0;
	 	  }








    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_7;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
