# MemoryGame
Memory game built in C using a Nucleo-L476RG board and a SG92R micro servo motor to create a "Simon Says" style game where the board creates a random 5 angular pattern on the motor arm. The user must then match the pattern through the use of a button input and a blinking LED indicator.  

## Installation
Recommended to use STM32CubeIDE v 1.7.0

New file dropdown menu -> STM32 Project from an Existing STM32CubeMX Configuration File (.ioc). Use "MemoryGame.ioc" for this step. After the project loads, click on "MemoryGame.ioc", and click Build 'Debug' for project 'Memory Game'. Then, build the "main.c" and "stm32l4xx_it.c" files in the source folder.

## Hardware Requirements 
- NUCLEO-L476RG - STMicroelectronics
- SG92R Servo Motor

## How to Play
- When the program starts, an LED will light up, indicating that it is the CPUs turn
- The CPU perform a random 5 sequence pattern which moves the motor arm to different angles accordingly
- The arm will rotate towards the player, and the LED will turn off, indicating that it is now the User's turn
- The User will press a button. For each second that the button is held, the LED will blink. Each blink indicates position 1, 2, 3, 4, or 5. It will continue to blink after 5, but these inputs will be disregarded by the game. Depending on how many seconds the button was held, the motor arm will move to the corresponding position
- The user has 3 tries to match the CPU's pattern
- In the event of a loss, the motor arm will move slowly taunting the player
- In the event of a win, the motor arm will perform a quick victory pattern
- After a win or loss, the game will reset, and the CPU will perform another random sequence
