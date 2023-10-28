#include <stm32f031x6.h>
#include <stdlib.h>
#include <stdbool.h>
#include "display.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160
#define MAX_ASTEROIDS 10
#define MAX_ASTEROID_SPEED 5

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void initAsteroids();
uint16_t updateAsteroids(uint16_t number_of_asteroids, uint16_t counter);
void drawAsteroids(uint16_t number_of_asteroids);
void clearAsteroid(uint16_t x,uint16_t y);
void clearAsteroids();


volatile uint32_t milliseconds;

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

int main()
{
	initClock();
	initSysTick();
	setupIO();
	while(1)
	{
		clear();
		int hinverted = 0;
		int vinverted = 0;
		int toggle = 0;
		int hmoved = 0;
		int vmoved = 0;

		bool game_running = true;

		uint16_t number_of_asteroids = 3;
		uint16_t counter = 0;

		uint16_t rocket_x = 50;
		uint16_t rocket_y = 50;
		uint16_t old_rocket_x = rocket_x;
		uint16_t old_rocket_y = rocket_y;

		initAsteroids();

		while(game_running == true)
		{
			hmoved = vmoved = 0;
			hinverted = vinverted = 0;

			drawAsteroids(number_of_asteroids);
			counter = updateAsteroids(number_of_asteroids, counter);

			// This is a potential way to increase amount of asteroids after 3 cycles of asteroids
			// if ((counter / number_of_asteroids) > 3 && number_of_asteroids < MAX_ASTEROIDS)
			// {
			// 	number_of_asteroids++;
			// }

			if ((GPIOB->IDR & (1 << 4))==0) // right pressed
			{					
				if (rocket_x < 110)
				{
					rocket_x++;
					hmoved = 1;
					hinverted=0;
				}						
			}
			if ((GPIOB->IDR & (1 << 5))==0) // left pressed
			{			
				
				if (rocket_x > 10)
				{
					rocket_x--;
					hmoved = 1;
					hinverted=0;
				}			
			}
			if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
			{
				if (rocket_y < 140)
				{
					rocket_y++;			
					vmoved = 1;
					vinverted = 0;
				}
			}
			if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
			{			
				if (rocket_y > 16)
				{
					rocket_y--;
					vmoved = 1;
					vinverted = 0;
				}
			}

			fillRectangle(old_rocket_x,old_rocket_y,12,16,0);
			old_rocket_x = rocket_x;
			old_rocket_y = rocket_y;					
			if (hmoved)
			{
				if (toggle)
					putImage(rocket_x,rocket_y,12,16,rocket,hinverted,0);
				else
					putImage(rocket_x,rocket_y,12,16,rocket,hinverted,0);
				
				toggle = toggle ^ 1;
			}
			else
			{
				putImage(rocket_x,rocket_y,12,16,rocket,0,vinverted);
			}

			for(int i = 0; i < number_of_asteroids; i++)
			{
				if (isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x,rocket_y) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+10,rocket_y) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x,rocket_y+14) || isInside(asteroids[i].x,asteroids[i].y,10,10,rocket_x+10,rocket_y+14) )
				{
					// we could use this to display a crash message if the hit an
					// printTextX2("Crashed!", 10, 20, RGBToWord(0xff,0xff,0), 0);
					clearAsteroids();
					printText("You have crashed!", 5, 5, RGBToWord(0xff,0xff,0), 0);
					printText("press any button", 5, 25, RGBToWord(0xff,0xff,0), 0);
					printText("to restart!", 5, 35, RGBToWord(0xff,0xff,0), 0);
					delay(200);
					game_running = false;					
					break;
				}
			}
			delay(35);
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
void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	// checks to see if point px,py is within the rectange defined by x,y,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		// ok, x constraint met
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}

void initAsteroids() {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].x = (rand() %13) * 10; // Random X position
        asteroids[i].y = 0; // Start at the top of the screen
        asteroids[i].speed = 1; 
    }
}

uint16_t updateAsteroids(uint16_t number_of_asteroids, uint16_t counter) {
    for (int i = 0; i < number_of_asteroids; i++) {
        asteroids[i].y += asteroids[i].speed;

        // If asteroid goes off the screen, respawn it at the top
        if (asteroids[i].y > 150) {
			counter += 1;
			fillRectangle(asteroids[i].x, asteroids[i].y - asteroids[i].speed, 10, 10, 0);
			asteroids[i].x = (rand() %14) * 10;
			asteroids[i].y = 0;
			if(asteroids[i].speed < 10)
			{
				asteroids[i].speed += 1;
			}
        }
    }
	return counter;
}

// Function to draw an asteroid at a specific position
void drawAsteroids(uint16_t number_of_asteroids) {
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