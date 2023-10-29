#include <stm32f031x6.h>
#include <stdbool.h>
#include "display.h"
#include <prbs.c>
#include <sound.c>
#include <serial.c>
#include <setup.c>

#define MAX_ASTEROIDS 10
#define MAX_ASTEROID_SPEED 5

void initAsteroids();
uint16_t updateAsteroids(uint16_t counter);
void drawAsteroids();
void clearAsteroid(uint16_t x,uint16_t y);
void clearAsteroids();
void menu();

struct Asteroid{
    uint16_t x;
	uint16_t y;
	uint16_t speed;
};

// Declare an array to store asteroids
struct Asteroid asteroids[MAX_ASTEROIDS];

const uint16_t asteroid[]=
{
	0,0,5220,38252,55180,59449,3683,12627,0,0,0,54380,29267,44866,12387,44635,4427,37467,45675,0,21628,13412,60490,36170,10033,1841,60746,52298,45387,44618,12098,60754,51257,42801,9777,10041,9777,10554,61242,61258,20307,27458,18233,42545,26161,1585,1577,10554,44090,27186,27970,59705,50993,58921,9777,1585,9777,34353,43057,35642,58929,51257,34609,17969,9777,9777,34353,18481,2362,36170,36658,11586,10289,26161,34609,9777,50993,19002,11834,12346,0,61002,35650,34865,59185,51257,19258,36154,44602,0,0,0,3386,61002,35394,45122,44858,20530,0,0,
};
const uint16_t rocket[]=
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56053,0,0,0,0,0,0,0,0,0,0,0,56301,56053,0,0,0,0,0,0,0,0,0,6100,56053,63965,13772,0,0,0,0,0,0,0,0,6100,54750,63965,13772,0,0,0,0,0,0,0,0,6100,51603,54750,13772,0,0,0,0,0,0,0,40438,6100,26752,27824,13772,0,0,0,0,0,0,0,20891,12995,59784,4033,12995,20891,0,0,0,0,512,20891,11097,53635,35168,35168,19570,59481,20891,0,0,0,20891,11097,3137,54106,35168,35168,13858,62066,11097,20891,0,0,35946,59712,4106,5130,63763,47122,14363,4106,59712,35946,512,25088,18433,60937,61481,55314,55331,7443,55314,53289,11785,18433,25608,25088,18433,60937,45834,63258,7443,55331,5914,45834,60937,59392,25088,512,34568,19465,53770,54802,5914,5914,5650,53514,19465,34560,8704,0,17664,51457,60937,37130,45834,45834,53514,60937,51457,17408,0,0,8704,1280,51465,19465,11785,60937,19465,51465,1536,8704,0
};
const uint16_t explosion[]=
{
	0,0,57091,0,0,0,0,0,0,40707,0,0,0,0,40707,40707,14602,31490,0,7683,56835,0,0,0,48899,40707,0,0,6161,22553,0,56593,23555,0,0,40707,48899,40707,55818,47626,30752,38432,5409,31520,31264,0,24323,57091,0,7427,63505,46112,30240,30496,22560,30752,15136,31257,40451,57091,48899,47122,22801,54816,38432,30240,30240,37664,46368,23065,6922,40707,55818,7435,46873,37920,30752,38176,38176,37664,30496,62752,38169,7435,40707,6923,22802,5401,38432,30496,37920,29984,38688,5913,37408,47378,0,0,7427,7698,55840,46880,30496,55840,62744,54809,0,45600,0,0,48899,40707,15890,32281,31264,23065,39186,30994,0,0,0,0,48899,57091,32515,0,7939,63762,5906,55818,57091,0,0,0,48899,32515,0,0,57091,40707,48899,0,0,0,
};

uint16_t number_of_asteroids;

int main()
{
	initClock();
	initSysTick();
	setupIO();
	menu();
	while(1)
	{
		clear();
		int toggle = 0;
		int hmoved = 0;
		int vmoved = 0;

		bool game_running = true;

		number_of_asteroids = 1;
		uint16_t counter = 0;

		uint16_t rocket_x = 50;
		uint16_t rocket_y = 50;
		uint16_t old_rocket_x = rocket_x;
		uint16_t old_rocket_y = rocket_y;

		initAsteroids();

		while(game_running == true)
		{
			hmoved = vmoved = 0;

			drawAsteroids();
			counter = updateAsteroids(counter);

			// This is a potential way to increase amount of asteroids after 3 cycles of asteroids
			if ((counter / number_of_asteroids) > 3 && number_of_asteroids < MAX_ASTEROIDS)
			{
				number_of_asteroids++;
			}

			if ((GPIOB->IDR & (1 << 4))==0) // right pressed
			{					
				if (rocket_x < 110)
				{
					rocket_x++;
					hmoved = 1;
				}						
			}
			if ((GPIOB->IDR & (1 << 5))==0) // left pressed
			{			
				
				if (rocket_x > 10)
				{
					rocket_x--;
					hmoved = 1;
				}			
			}
			if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
			{
				if (rocket_y < 140)
				{
					rocket_y++;			
					vmoved = 1;
				}
			}
			if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
			{			
				if (rocket_y > 16)
				{
					rocket_y--;
					vmoved = 1;
				}
			}

			fillRectangle(old_rocket_x,old_rocket_y,12,16,0);
			old_rocket_x = rocket_x;
			old_rocket_y = rocket_y;					
			if (hmoved)
			{
				if (toggle)
					putImage(rocket_x,rocket_y,12,16,rocket,0,0);
				else
					putImage(rocket_x,rocket_y,12,16,rocket,0,0);
				
				toggle = toggle ^ 1;
			}
			else
			{
				putImage(rocket_x,rocket_y,12,16,rocket,0,0);
			}

			for(int i = 0; i < number_of_asteroids; i++)
			{
				if(isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x,rocket_y) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+10,rocket_y) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x,rocket_y+14) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+10,rocket_y+14) )
				{
					// we could use this to display a crash message if the hit an
					// printTextX2("Crashed!", 10, 20, RGBToWord(0xff,0xff,0), 0);
					clear();
					putImage(rocket_x,rocket_y,12,12,explosion,0,0);
					playNote(G7);
					delay(1000);
					printText("You have crashed!", 5, 5, RGBToWord(0xff,0xff,0), 0);
					delay(2000);
					printText("press any button", 5, 25, RGBToWord(0xff,0xff,0), 0);
					printText("to restart!", 5, 35, RGBToWord(0xff,0xff,0), 0);
					game_running = false;					
					break;
				}
			}
			delay(50);
		}
		while(game_running == false)
		{
			if ((GPIOB->IDR & (1 << 4))==0 || (GPIOB->IDR & (1 << 5))==0 || (GPIOA->IDR & (1 << 11)) == 0 || (GPIOA->IDR & (1 << 8)) == 0) // any button pressed
			{
				game_running = true;
			}
		}
	}
	return 0;
}

void initAsteroids() {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].x = 5 + random(1,10) * 12; // Random X position
		for(int j; j < number_of_asteroids; j++)
		{
			if(asteroids[i].x == asteroids[j].x)
			{
				asteroids[i].x = 5 + (random(1,10) * 12);
				j = 0;
			}
		}
        asteroids[i].y = 0; // Start at the top of the screen
        asteroids[i].speed = 1; 
    }
}

uint16_t updateAsteroids(uint16_t counter) {
	uint32_t random_number = random(1,5);
    for (int i = 0; i < number_of_asteroids; i++) {
        asteroids[i].y += asteroids[i].speed;

        // If asteroid goes off the screen, respawn it at the top
        if (asteroids[i].y > 150) {
			counter += 1;
			fillRectangle(asteroids[i].x, asteroids[i].y - asteroids[i].speed, 10, 10, 0);
			asteroids[i].x = 5 + (random(1,10) * 12);
			for(int j; j < number_of_asteroids; j++)
			{
				if(asteroids[i].x == asteroids[j].x)
				{
					asteroids[i].x = 5 + (random(1,10) * 12);
					j = 0;
				}
			}
			asteroids[i].y = 0;
			if(asteroids[i].speed < MAX_ASTEROID_SPEED && random_number == 1)
			{
				asteroids[i].speed += 1;
			}
        }
    }
	return counter;
}

// Function to draw an asteroid at a specific position
void drawAsteroids() {
	for(int i = 0; i < number_of_asteroids; i++){
		clearAsteroid(asteroids[i].x, asteroids[i].y - asteroids[i].speed);
		putImage(asteroids[i].x, asteroids[i].y, 10, 10, asteroid, 0, 0); // Adjust the size and image as needed
	}
}

// Function to clear the area where an asteroid was previously drawn
void clearAsteroid(uint16_t x, uint16_t y) {
    fillRectangle(x, y, 10, 10, 0);
}

void clearAsteroids() {
	for(int i = 0; i < MAX_ASTEROIDS; i++){
		clearAsteroid(asteroids[i].x, asteroids[i].y - asteroids[i].speed);
	}
}

void menu(){
	printTextX2("Welcome to", 5, 5, RGBToWord(0xff,0xff,0), 0);
	printTextX2("Space", 30, 30, RGBToWord(0xff,0xff,0), 0);
	printTextX2("Dodger", 25, 50, RGBToWord(0xff,0xff,0), 0);
	printText("Dodge the Meteors", 5, 80, RGBToWord(0xff,0xff,0), 0);
	delay(5000);
}