#include <stm32f031x6.h>
// #include "stm32f0xx_hal.h"
#include <stdbool.h>
#include "display.h"
#include <prbs.c>
#include <sound.c>
#include <setup.c>

#define MAX_ASTEROIDS 8
#define MAX_ASTEROID_SPEED 8

#define YELLOW RGBToWord(246, 235, 20)
#define GREEN RGBToWord(79, 175, 68)
#define ORANGE RGBToWord(255, 149, 38)
#define RED RGBToWord(239, 68, 35)
#define BLUE RGBToWord(42, 52, 146)

// #define FLASH_SECTOR     FLASH_SECTOR_7 // Example sector, change to the appropriate sector
// #define HIGHEST_LEVEL_ADDRESS 0x0800C000 // Address of the highest completed level in flash memory

void setupGame();
void initAsteroids();
void updateAsteroids();
void drawAsteroids();
void clearAsteroid(uint16_t x,uint16_t y);
void clearAsteroids();
void menu();
void gameCrashed();
void countdown();
void gameLoop();
void handlePlayerInput(int *hmoved, int *vmoved);
void updateRocketPosition(int hmoved, int vmoved, int toggle);
bool checkCollision();
void levelUp();
void resetAsteroids();

// void loadGame(int level);
// void saveHighestLevel(int level);;
// int loadHighestLevel();

struct Asteroid{
    uint16_t x;
	uint16_t y;
	uint16_t speed;
};

// Define level information
struct Level {
    int numAsteroids;
    int maxAsteroidSpeed;
	char levelName[10];
	char levelmessage[20];
	int levelScreenPosition;
	char levelDeathMessage[20];	
	int deathMessageScreenPosition;
// we can add anything else that we need per level
};

// Define an array of levels up to 5 levels
struct Level levels[] = {
	{1, 3, "Level 0", "N/A", 30, "Embarrassing...", 10},  // Level 0
    {2, 3, "Level 1", "Think you're good?", 2, "Were you trying?", 10},  // Level 1
    {3, 4, "Level 2", "Getting There!", 15, "Not the worst.", 15},  // Level 2
	{4, 5, "Level 3", "This all you got?", 5, "Try again I guess", 1},  // Level 3
	{5, 6, "Level 4", "Okay, not too bad", 5, "Not bad, go again", 5},  // Level 4
	{6, 7, "Level 5", "This is crazy", 16, "I'm Impressed", 15},  // Level 5
	{7, 8, "Level 6", "How???", 40, "You are quite good", 2},  // Level 6
	{8, 9, "Level 7", "You're too good", 9, "Im getting worried", 2},  // Level 7
	{9, 10, "Level 8", "I'm very impressed!", 1, "That was Insane!!", 5},  // Level 8
};

int currentLevel; // Initializes the game level variable 

// Declare an array to store asteroids
struct Asteroid asteroids[MAX_ASTEROIDS];

const uint16_t asteroid[]= {
	0,0,5220,38252,55180,59449,3683,12627,0,0,0,54380,29267,44866,12387,44635,4427,37467,45675,0,21628,13412,60490,36170,10033,1841,60746,52298,45387,44618,12098,60754,51257,42801,9777,10041,9777,10554,61242,61258,20307,27458,18233,42545,26161,1585,1577,10554,44090,27186,27970,59705,50993,58921,9777,1585,9777,34353,43057,35642,58929,51257,34609,17969,9777,9777,34353,18481,2362,36170,36658,11586,10289,26161,34609,9777,50993,19002,11834,12346,0,61002,35650,34865,59185,51257,19258,36154,44602,0,0,0,3386,61002,35394,45122,44858,20530,0,0,
};
const uint16_t rocket[]= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56053,0,0,0,0,0,0,0,0,0,0,0,56301,56053,0,0,0,0,0,0,0,0,0,6100,56053,63965,13772,0,0,0,0,0,0,0,0,6100,54750,63965,13772,0,0,0,0,0,0,0,0,6100,51603,54750,13772,0,0,0,0,0,0,0,40438,6100,26752,27824,13772,0,0,0,0,0,0,0,20891,12995,59784,4033,12995,20891,0,0,0,0,512,20891,11097,53635,35168,35168,19570,59481,20891,0,0,0,20891,11097,3137,54106,35168,35168,13858,62066,11097,20891,0,0,35946,59712,4106,5130,63763,47122,14363,4106,59712,35946,512,25088,18433,60937,61481,55314,55331,7443,55314,53289,11785,18433,25608,25088,18433,60937,45834,63258,7443,55331,5914,45834,60937,59392,25088,512,34568,19465,53770,54802,5914,5914,5650,53514,19465,34560,8704,0,17664,51457,60937,37130,45834,45834,53514,60937,51457,17408,0,0,8704,1280,51465,19465,11785,60937,19465,51465,1536,8704,0
};
const uint16_t explosion[]= {
	0,0,57091,0,0,0,0,0,0,40707,0,0,0,0,40707,40707,14602,31490,0,7683,56835,0,0,0,48899,40707,0,0,6161,22553,0,56593,23555,0,0,40707,48899,40707,55818,47626,30752,38432,5409,31520,31264,0,24323,57091,0,7427,63505,46112,30240,30496,22560,30752,15136,31257,40451,57091,48899,47122,22801,54816,38432,30240,30240,37664,46368,23065,6922,40707,55818,7435,46873,37920,30752,38176,38176,37664,30496,62752,38169,7435,40707,6923,22802,5401,38432,30496,37920,29984,38688,5913,37408,47378,0,0,7427,7698,55840,46880,30496,55840,62744,54809,0,45600,0,0,48899,40707,15890,32281,31264,23065,39186,30994,0,0,0,0,48899,57091,32515,0,7939,63762,5906,55818,57091,0,0,0,48899,32515,0,0,57091,40707,48899,0,0,0,
};

uint16_t number_of_asteroids;
uint16_t score;

uint16_t rocket_x;
uint16_t rocket_y;
uint16_t old_rocket_x;
uint16_t old_rocket_y;

int main() {
    setupGame();
    menu();
	return 0;
}

void setupGame() {
    initClock();
    initSysTick();
    setupIO();
	initSound();
}  


void menu() {
    printTextX2("Welcome to", 5, 5, YELLOW, 0);
    printTextX2("Space", 32, 30, YELLOW, 0);
    printTextX2("Dodger", 28, 50, YELLOW, 0);
    printText("Dodge the Meteors", 5, 80, RED, 0);
    
    // Display the highest completed level on the menu screen
    // char highestLevelText[20];
    // printText(highestLevelText, 10, 80, GREEN, 0);
    
    int counter = 0;
    while(1){
        counter++;
        if(counter < 50) {
            printText("Press < or >", 23, 100, ORANGE, 0);
        }
        else if(counter < 80) {
            fillRectangle(23, 100, 210, 10, 0);
        }
        else {
            counter = 0;
        }

        // Detect button press restart
        if ((GPIOB->IDR & (1 << 4)) == 0 || (GPIOB->IDR & (1 << 5)) == 0) {
            break;
        }

        delay(10);
    }
    clear();
    delay(20);
    gameLoop();
}


void countdown() {
	clear();

	for(int i = 0; i < 4; i++){
		if(i == 0) {
			printTextX2("3", 58, 40, RED, 0);
			playNote(E3);
		}
		else if(i == 1) {
			printTextX2("2", 58, 40, RED, 0);
			playNote(F3);
		}
		else if(i == 2) {
			printTextX2("1", 58, 40, RED, 0);
			playNote(G3);
		}
		else if(i == 3) {
			clear();
			printTextX2("DODGE!", 30, 40, RED, 0);
			playNote(D3);
			break;
		}
		delay(200);
		playNote(0);
		delay(800);
	}
	delay(500);
	playNote(0);
	clear();
}

void gameLoop() {
	countdown();

	int toggle = 0;
	int hmoved = 0;
	int vmoved = 0;
	//char levelText[16];

	struct Level currentLevelInfo;

	score = 0;
	uint16_t previous_score = 1;
	number_of_asteroids = 1;
	bool game_running = true;

	currentLevel = 0;

	rocket_x = 53;
	rocket_y = 120;
	old_rocket_x = rocket_x;
	old_rocket_y = rocket_y;
	putImage(rocket_x,rocket_y,12,16,rocket,0,0);

	initAsteroids();

	while (game_running) {
		playNote(0);
		hmoved = vmoved = 0;
		currentLevelInfo = levels[currentLevel];
		// Print the Level and Score
		//snprintf(levelText, sizeof(levelText), "Level %d", currentLevel);
		printText(currentLevelInfo.levelName, 74, 5, BLUE, 0);

		drawAsteroids();
		updateAsteroids();

		// This is a way to increase the number of asteroids after the score reaches certain points
		if (score >= previous_score + (currentLevelInfo.numAsteroids + currentLevel) * 10  && currentLevel <= 9) {
			levelUp();
			previous_score = score;
		}

		handlePlayerInput(&hmoved, &vmoved);
		updateRocketPosition(hmoved, vmoved, toggle);

		if (checkCollision()) {
			game_running = false;
		}

		delay(30);
    }

	gameCrashed();
}

void initAsteroids() {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].x = 5 + random(1,10) * 12; // Random X position
		for(int j; j < MAX_ASTEROIDS; j++) {
			if(asteroids[i].x == asteroids[j].x) {
				asteroids[i].x = 5 + (random(1,10) * 12);
				j = 0;
			}
		}
        asteroids[i].y = 0; // Start at the top of the screen
        asteroids[i].speed = 2;
    }
}

// Function to draw an asteroid at a specific position
void drawAsteroids() {
	struct Level currentLevelInfo = levels[currentLevel];
	for(int i = 0; i < number_of_asteroids; i++) {
		clearAsteroid(asteroids[i].x, asteroids[i].y - asteroids[i].speed);
		putImage(asteroids[i].x, asteroids[i].y, 10, 10, asteroid, 0, 0); // Adjust the size and image as needed
		printNumber(score, 5, 5, BLUE, 0);
		printText(currentLevelInfo.levelName, 74, 5, BLUE, 0);
	}
}

void updateAsteroids() {
	struct Level currentLevelInfo = levels[currentLevel];
    for (int i = 0; i < number_of_asteroids; i++) {
        asteroids[i].y += asteroids[i].speed;

		if (asteroids[i].y > number_of_asteroids * (random(40,100)) && number_of_asteroids < currentLevelInfo.numAsteroids){
			number_of_asteroids++;
		}
        // If asteroid goes off the screen, respawn it at the top
        if (asteroids[i].y > 150) {
			playNote(B4);
			score++;
			printNumber(score, 5, 5, BLUE, 0);
			fillRectangle(asteroids[i].x, asteroids[i].y - asteroids[i].speed, 10, 10, 0);
			asteroids[i].x = random(1,10) * 12;
			for(int j = 0; j < currentLevelInfo.numAsteroids; j++) {
				if(asteroids[i].x == asteroids[j].x && i != j) {
					asteroids[i].x = random(1,10) * 12;
				}
			}
			asteroids[i].y = 0;
        }

		if(asteroids[i].speed < currentLevelInfo.maxAsteroidSpeed) {
			asteroids[i].speed = currentLevelInfo.maxAsteroidSpeed;
		}
    }
}

void handlePlayerInput(int *hmoved, int *vmoved){
	if ((GPIOB->IDR & (1 << 4))==0) { // right pressed					
		if (rocket_x < 110) {
			rocket_x += 4;
			*hmoved = 1;
		}						
	}
	if ((GPIOB->IDR & (1 << 5))==0) { // left pressed		
		if (rocket_x > 3) {
			rocket_x -= 4;
			*hmoved = 1;
		}			
	}
	if ( (GPIOA->IDR & (1 << 11)) == 0) { // down pressed
		if (rocket_y < 140) {
			rocket_y += 2;			
			*vmoved = 1;
		}
	}
	if ( (GPIOA->IDR & (1 << 8)) == 0) { // up pressed		
		if (rocket_y > 55) {
			rocket_y -= 2;
			*vmoved = 1;
		}
	}
}

void updateRocketPosition(int hmoved, int vmoved, int toggle) {
	if ((vmoved) || (hmoved)){	
		fillRectangle(old_rocket_x,old_rocket_y,12,16,0);
		old_rocket_x = rocket_x;
		old_rocket_y = rocket_y;				
		putImage(rocket_x,rocket_y,12,16,rocket,0,0);
	}
}
// void drawRocket(uint16_t rocket_x, uint16_t rocket_y, uint16_t hmoved, uint16_t toggle){

// }

bool checkCollision() {
	bool is_crashed = false;
	struct Level currentLevelInfo = levels[currentLevel];

	for(int i = 0; i < currentLevelInfo.numAsteroids; i++) {
		if(isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x,rocket_y) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+10,rocket_y) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x,rocket_y+10) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+10,rocket_y+10) ){
			is_crashed = true;					
			break;
		}
	}
	return is_crashed;
}

// Function to clear the area where an asteroid was previously drawn
void clearAsteroid(uint16_t x, uint16_t y) {
    fillRectangle(x, y, 10, 10, 0);
}

// Function to clear all asteroids, could be useful for level ups so whole screen isnt cleared
void clearAsteroids() {
	for(int i = 0; i < MAX_ASTEROIDS; i++) {
		clearAsteroid(asteroids[i].x, asteroids[i].y - asteroids[i].speed);
	}
}

// This function is called when the player crashes into asteroid, displays message asking to press < or > to restart
void gameCrashed() {
	clear();
	putImage(rocket_x,rocket_y,12,12,explosion,0,0);
	playNote(D2);
	delay(1000);
	playNote(0);
	printTextX2("You have", 15, 5, YELLOW, 0);
	printTextX2("CRASHED", 20, 25, RED, 0);
	printText("Final score:", 5, 45, BLUE, 0);
	printNumber(score, 88, 45, BLUE, 0);
	printText(levels[currentLevel].levelDeathMessage, levels[currentLevel].deathMessageScreenPosition, 60, ORANGE, 0);
	delay(4000);
	fillRectangle(0, 60, 130, 10, 0);
	fillRectangle(rocket_x, rocket_y, 12, 12, 0);
	printText("To Restart", 28, 80, YELLOW, 0);
	int counter = 0;
	while(1) {
		counter++;
		if(counter < 50) {
			printText("Press < or >", 23, 65, ORANGE, 0);
			printText("Press V to quit", 10, 100, ORANGE, 0);
		}
		else if(counter<80) {
			fillRectangle(20, 65, 100, 10, 0);
		}
		else {
			counter = 0;
		}
		if ((GPIOB->IDR & (1 << 4))==0 || (GPIOB->IDR & (1 << 5))==0) {
			gameLoop();
			break;
		}
		else if((GPIOA->IDR & (1 << 11)) == 0)
		{
			clear();
			printText("Thank you for", 18, 5, BLUE, 0);
			printText("playing", 37, 15, BLUE, 0);
			printTextX2("Space", 32, 30, YELLOW, 0);
			printTextX2("Dodger", 28, 50, YELLOW, 0);
			break;
		}
		delay(12);
	}
	

}

void levelUp(){
	currentLevel++;
	clearAsteroids();
	resetAsteroids();
	printNumber(score, 5, 5, BLUE, 0);
	printText(levels[currentLevel].levelName, 74, 5, BLUE, 0);
	playNote(A3);
	delay(500);
	playNote(0);
	fillRectangle(74,5,70,10,0);
	printText("You've reached", 15, 20, RED, 0);
	printTextX2(levels[currentLevel].levelName, 20, 40, RED, 0);
	printText(levels[currentLevel].levelmessage, levels[currentLevel].levelScreenPosition, 70, ORANGE, 0);
	delay(5000);
	printText(levels[currentLevel].levelName, 74, 5, BLUE, 0);
	fillRectangle(1,10,150,100,0);
	}

void resetAsteroids(){
	for(int i = 0; i < number_of_asteroids; i++){
		asteroids[i].y = 0;
	}
	number_of_asteroids = 1;
}

// void loadGame(int level) {
//     if (level >= 1 && level <= sizeof(levels) / sizeof(levels[0])) {
//         currentLevel = level;

//         setupGame();

//         clear();
//         gameLoop();
//     } else {
//         printText("Invalid Level Selection", 10, 60, RED, 0);
//         delay(2000); // Wait 2 seconds
//         clear();
//         menu(); // Return to the menu after an invalid level selection
//     }
// }

// void saveHighestLevel(int level) {
//     HAL_Init();
//     FLASH_EraseInitTypeDef eraseInitStruct;
//     uint32_t sectorError;
//     HAL_StatusTypeDef status;
//     eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
//     eraseInitStruct.Sector = FLASH_SECTOR_0;
//     eraseInitStruct.NbSectors = 1;

//     HAL_FLASH_Unlock();

//     status = HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);
//     if (status == HAL_OK) {
//         status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_BASE + 0x40000, level);
//     }
//     HAL_FLASH_Lock();
// }

// // Function to load the highest completed level from non-volatile memory
// int loadHighestLevel() {	
//     HAL_Init();

//     int highestLevel = *(__IO int*)HIGHEST_LEVEL_ADDRESS;

//     return highestLevel;
// }