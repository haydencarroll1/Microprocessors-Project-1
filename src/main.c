#include <stm32f031x6.h>
#include <stdbool.h>
#include "display.h"
#include "prbs.h"
#include "sound.h"
#include "setup.h"

#define MAX_ASTEROIDS 20
#define MAX_ASTEROID_SPEED 10

#define YELLOW RGBToWord(246, 235, 20)
#define GREEN RGBToWord(79, 175, 68)
#define ORANGE RGBToWord(255, 149, 38)
#define RED RGBToWord(239, 68, 35)
#define BLUE RGBToWord(42, 52, 146)

void setupGame();
void menu();
void menuInterface();
void initAsteroids();
void updateAsteroids();
void drawAsteroids();
void clearAsteroid(uint16_t x,uint16_t y);
void clearAsteroids();
void gameCrashed();
void countdown();
void gameLoop();
void handlePlayerInput(int *hmoved, int *vmoved);
void updateRocketPosition(int hmoved, int vmoved, int toggle);
bool checkCollision();
void levelUp();
void resetAsteroids();
bool checkHighScore();
void printHighScore();

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
	{5, 2, "Level 1", "N/A", 30, "Embarrassing...", 10},  // Level 1
    {6, 3, "Level 2", "Think you're good?", 2, "Were you trying?", 10},  // Level 2
    {7, 4, "Level 3", "Getting There!", 15, "Not the worst.", 15},  // Level 3
	{7, 5, "Level 4", "This all you got?", 5, "Try again I guess", 1},  // Level 4
	{7, 6, "Level 5", "Okay, not too bad", 5, "Not bad, go again", 5},  // Level 5
	{7, 7, "Level 6", "This is crazy", 16, "I'm Impressed", 15},  // Level 6
	{10, 8, "Level 7", "How???", 40, "You are quite good", 2},  // Level 7
	{11, 9, "Level 8", "You're too good", 9, "Im getting worried", 2},  // Level 8 
	{12, 10, "Level 9", "I'm very impressed!", 1, "That was Insane!!", 5},  // Level 9
};

int currentLevel; // Initializes the game level variable 

// Declare an array to store asteroids
struct Asteroid asteroids[MAX_ASTEROIDS];

const uint16_t asteroid1[]= {
	0,0,5220,38252,55180,59449,3683,12627,0,0,0,54380,29267,44866,12387,44635,4427,37467,45675,0,21628,13412,60490,36170,10033,1841,60746,52298,45387,44618,12098,60754,51257,42801,9777,10041,9777,10554,61242,61258,20307,27458,18233,42545,26161,1585,1577,10554,44090,27186,27970,59705,50993,58921,9777,1585,9777,34353,43057,35642,58929,51257,34609,17969,9777,9777,34353,18481,2362,36170,36658,11586,10289,26161,34609,9777,50993,19002,11834,12346,0,61002,35650,34865,59185,51257,19258,36154,44602,0,0,0,3386,61002,35394,45122,44858,20530,0,0,
};
const uint16_t rocket[]= {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56053,0,0,0,0,0,0,0,0,0,0,0,56301,56053,0,0,0,0,0,0,0,0,0,6100,56053,63965,13772,0,0,0,0,0,0,0,0,6100,54750,63965,13772,0,0,0,0,0,0,0,0,6100,51603,54750,13772,0,0,0,0,0,0,0,40438,6100,26752,27824,13772,0,0,0,0,0,0,0,20891,12995,59784,4033,12995,20891,0,0,0,0,512,20891,11097,53635,35168,35168,19570,59481,20891,0,0,0,20891,11097,3137,54106,35168,35168,13858,62066,11097,20891,0,0,35946,59712,4106,5130,63763,47122,14363,4106,59712,35946,512,25088,18433,60937,61481,55314,55331,7443,55314,53289,11785,18433,25608,25088,18433,60937,45834,63258,7443,55331,5914,45834,60937,59392,25088,512,34568,19465,53770,54802,5914,5914,5650,53514,19465,34560,8704,0,17664,51457,60937,37130,45834,45834,53514,60937,51457,17408,0,0,8704,1280,51465,19465,11785,60937,19465,51465,1536,8704,0
};
const uint16_t explosion[]= {
	0,0,57091,0,0,0,0,0,0,40707,0,0,0,0,40707,40707,14602,31490,0,7683,56835,0,0,0,48899,40707,0,0,6161,22553,0,56593,23555,0,0,40707,48899,40707,55818,47626,30752,38432,5409,31520,31264,0,24323,57091,0,7427,63505,46112,30240,30496,22560,30752,15136,31257,40451,57091,48899,47122,22801,54816,38432,30240,30240,37664,46368,23065,6922,40707,55818,7435,46873,37920,30752,38176,38176,37664,30496,62752,38169,7435,40707,6923,22802,5401,38432,30496,37920,29984,38688,5913,37408,47378,0,0,7427,7698,55840,46880,30496,55840,62744,54809,0,45600,0,0,48899,40707,15890,32281,31264,23065,39186,30994,0,0,0,0,48899,57091,32515,0,7939,63762,5906,55818,57091,0,0,0,48899,32515,0,0,57091,40707,48899,0,0,0,
};
const uint16_t asteroid[]= {
0,0,50473,50473,33560,33560,50473,50473,0,0,0,50473,9513,9513,33560,57608,57608,33560,50473,0,50473,57608,9513,9249,49944,9249,16912,57608,50473,50473,50473,9513,16912,49944,0,49944,49944,9249,57608,50473,33560,57608,57608,0,0,0,16912,9249,57608,33560,50473,57608,9249,49944,0,0,49944,57608,57608,33560,50473,33560,16912,58929,49944,49944,57608,57608,33560,50473,50473,33560,33560,26425,16912,9249,57608,33560,57608,50473,0,50473,33560,57608,33825,33560,33560,57608,50473,0,0,0,50473,50473,33560,33560,50473,50473,0,0,
};

uint16_t high_score = 411;

uint16_t number_of_asteroids;

uint16_t score = 0;

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
    
	menuInterface();

    clear();
    delay(20);
    gameLoop();
}

void menuInterface() {
    int counter = 0;
    int menu_selection = 0;

    while (1) {
        counter++;
        if (counter < 50) {
            printText("Press < or >", 23, 100, ORANGE, 0);
        } else if (counter < 80) {
            fillRectangle(23, 100, 210, 10, 0);
        } else {
            counter = 0;
        }

        // Display menu options
        printText("- Play", 5, 112, menu_selection == 0 ? RED : BLUE, 0);
        printText("- Difficulty", 5, 124, menu_selection == 1 ? RED : BLUE, 0);
        printText("- Settings", 5, 136, menu_selection == 2 ? RED : BLUE, 0);

        // Detect button press to navigate the menu
        if ((GPIOA->IDR & (1 << 11)) == 0) {
            if (menu_selection < 2) {
                menu_selection++;
                delay(50);
            } else {
                menu_selection = 0;
                delay(50);
            }
        } else if ((GPIOA->IDR & (1 << 8)) == 0) {
            if (menu_selection > 0) {
                menu_selection--;
                delay(50);
            } else {
                menu_selection = 2;
                delay(50);
            }
        }

        // Detect button press to select an option
        if ((GPIOB->IDR & (1 << 4)) == 0 || (GPIOB->IDR & (1 << 5)) == 0) {
            switch (menu_selection) {
                case 0:
                    // Start the game (Play option)
                    clear();
                    delay(20);
                    gameLoop();
                    return;
                case 1:
                    // Handle difficulty settings
                    clear();
                    int difficulty_selection = 0;

                    while (1) {
                        printText("Select Difficulty", 5, 50, YELLOW, 0);
                        printText("- Easy", 5, 80, difficulty_selection == 0 ? RED : BLUE, 0);
                        printText("- Normal", 5, 100, difficulty_selection == 1 ? RED : BLUE, 0);
                        printText("- Hard", 5, 120, difficulty_selection == 2 ? RED : BLUE, 0);
                        printText("Press < or > to choose", 5, 150, ORANGE, 0);

                        if ((GPIOB->IDR & (1 << 4)) == 0 || (GPIOB->IDR & (1 << 5)) == 0) {
                            // Save the selected difficulty and update game settings
                            switch (difficulty_selection) {
                                case 0:
                                    // Set game difficulty to Easy
									currentLevel = 0;
                                    break;
                                case 1:
                                    // Set game difficulty to Normal
									currentLevel = 2;
                                    break;
                                case 2:
                                    // Set game difficulty to Hard
									currentLevel = 4;
                                    break;
                            }
                            break;
                        }

                        if ((GPIOA->IDR & (1 << 11)) == 0) {
                            if (difficulty_selection < 2) {
                                difficulty_selection++;
                                delay(50);
                            }
                        } else if ((GPIOA->IDR & (1 << 8)) == 0) {
                            if (difficulty_selection > 0) {
                                difficulty_selection--;
                                delay(50);
                            }
                        }
                    }

                    // After setting the difficulty, return to the main menu
                    menu_selection = 0; // Set menu selection back to Play
                    break;
                case 2:
                    // Handle settings (Settings option)
                    break;
            }
        }

        delay(50);
    }
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

	struct Level currentLevelInfo;
	score = 0;
	uint16_t previous_score = 1;
	number_of_asteroids = 1;
	bool game_running = true;
	bool new_high_score = false;

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
		printText(currentLevelInfo.levelName, 74, 5, BLUE, 0);

		drawAsteroids();
		updateAsteroids();

		// This is a way to increase the number of asteroids after the score reaches certain points
		if (score >= previous_score + (currentLevelInfo.numAsteroids + currentLevel) * 10  && currentLevel <= 9) {
			levelUp();
			previous_score = score;
		}

		if(checkHighScore() && new_high_score == false){
			new_high_score = true;
			printHighScore();
		}

		handlePlayerInput(&hmoved, &vmoved);
		updateRocketPosition(hmoved, vmoved, toggle);

		if (checkCollision()) {
			game_running = false;
		}

		delay(35);
    }

	gameCrashed();
}

void initAsteroids() {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].x = 5 + random(1,10) * 12; // Random X position
		int j;
		for(j; j < MAX_ASTEROIDS; j++) {
			if(asteroids[i].x == asteroids[j].x) {
				asteroids[i].x = 3 + (random(0,10) * 12);
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

		if (asteroids[i].y > number_of_asteroids * (random(200/currentLevelInfo.numAsteroids,400/currentLevelInfo.numAsteroids)) && number_of_asteroids < currentLevelInfo.numAsteroids){
			number_of_asteroids++;
		}
        // If asteroid goes off the screen, respawn it at the top
        if (asteroids[i].y > 150) {
			playNote(B4);
			score++;
			printNumber(score, 5, 5, BLUE, 0);
			fillRectangle(asteroids[i].x, asteroids[i].y - asteroids[i].speed, 10, 10, 0);
			asteroids[i].x = 3 + random(0,10) * 12;
			for(int j = 0; j < currentLevelInfo.numAsteroids; j++) {
				if(asteroids[i].x == asteroids[j].x && i != j) {
					asteroids[i].x = 3 + random(0,10) * 12;
					j = 0;
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
		if (rocket_y > 68) {
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

bool checkCollision() {
	bool is_crashed = false;
	struct Level currentLevelInfo = levels[currentLevel];

	for(int i = 0; i < currentLevelInfo.numAsteroids; i++) {
		if(isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+4,rocket_y+2) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+7,rocket_y+2) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x + 1,rocket_y + 10) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+10,rocket_y+10) ){
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
	printText("High score:", 8, 58, YELLOW, 0);
	printNumber(high_score, 85, 58, YELLOW, 0);
	printText(levels[currentLevel].levelDeathMessage, levels[currentLevel].deathMessageScreenPosition, 70, ORANGE, 0);
	delay(3000);
	fillRectangle(0, 70, 130, 10, 0);
	fillRectangle(rocket_x, rocket_y, 12, 12, 0);
	printText("To Restart", 28, 85, YELLOW, 0);
	int counter = 0;
	while(1) {
		counter++;
		if(counter < 50) {
			printText("Press < or >", 23, 73, ORANGE, 0);
			printText("Press V to return", 3, 105, ORANGE, 0);
			printText("to main menu", 10, 115, ORANGE, 0);
		}
		else if(counter<80) {
			fillRectangle(20, 73, 100, 10, 0);
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
			menu();
			// printText("Thank you for", 18, 5, BLUE, 0);
			// printText("playing", 37, 15, BLUE, 0);
			// printTextX2("Space", 32, 30, YELLOW, 0);
			// printTextX2("Dodger", 28, 50, YELLOW, 0);
			// break;
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

bool checkHighScore(){
	if(score > high_score){
		high_score = score;
		return true;
	}
	else return false;
}

void printHighScore(){
	clearAsteroids();
	resetAsteroids();
	playNote(A3);
	fillRectangle(74,5,70,10,0);
	printText("Congratulations!!", 10, 20, RED, 0);
	printText("New High Score", 15, 30, RED, 0);
	printNumberX2(high_score, 30, 45, BLUE, 0);
	delay(500);
	playNote(0);
	delay(2000);
	fillRectangle(10, 20, 120, 45, 0);
}