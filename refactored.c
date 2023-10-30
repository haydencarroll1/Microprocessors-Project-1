//This is a refactored version of the code from src/main.c The code is refactored to improve
//readability and maintainability. The code is refactored by extracting methods and variables
//Sam Curran 30/10/2023 5:15am 

void setupGame() {
    initClock();
    initSysTick();
    setupIO();
}  

void gameLoop() {
    int toggle = 0;
    int hmoved = 0;
    int vmoved = 0;
    char levelText[16];

    score = 0;
    uint16_t previous_score = 1;
    number_of_asteroids = 1;
    bool game_running = true;

    uint16_t rocket_x = 53;
    uint16_t rocket_y = 80;
    uint16_t old_rocket_x = rocket_x;
    uint16_t old_rocket_y = rocket_y;

    initAsteroids();

    while (game_running) {
        hmoved = vmoved = 0;

        // Print the Level and Score
        snprintf(levelText, sizeof(levelText), "Level %d", currentLevel);
        printText(levelText, 74, 5, BLUE, 0);

        drawAsteroids();
        updateAsteroids();

        // This is a way to increase the number of asteroids after the score reaches certain points
        if (score > (previous_score * 3) && number_of_asteroids < MAX_ASTEROIDS) {
            number_of_asteroids++;
            previous_score = score;
        }

        handlePlayerInput(rocket_x, rocket_y, &hmoved, &vmoved);
        updateRocketPosition(rocket_x, rocket_y, old_rocket_x, old_rocket_y, hmoved);
        drawRocket(rocket_x, rocket_y, hmoved, toggle);

        if (checkCollision(rocket_x, rocket_y)) {
            game_running = false;
        }

        delay(35);
    }
}

void gameCrashed(uint16_t rocket_x, uint16_t rocket_y) {
    clear();
    putImage(rocket_x, rocket_y, 12, 12, explosion, 0, 0);
    playNote(G7);
    delay(1000);
    printTextX2("You have", 15, 5, YELLOW, 0);
    printTextX2("CRASHED", 20, 25, RED, 0);
    printText("Final score:", 5, 45, BLUE, 0);
    printNumber(score, 88, 45, BLUE, 0);
    delay(2000);
    fillRectangle(rocket_x, rocket_y, 12, 12, 0);
    printText("To Restart", 28, 80, YELLOW, 0);
    waitForRestart();
}

void waitForRestart() {
    int counter = 0;
    while (1) {
        counter++;
        if (counter < 50) {
            printText("Press < or >", 23, 65, ORANGE, 0);
        } else if (counter < 80) {
            fillRectangle(20, 65, 100, 10, 0);
        } else {
            counter = 0;
        }
        if ((GPIOB->IDR & (1 << 4)) == 0 || (GPIOB->IDR & (1 << 5)) == 0) {
            return; // Restart the game
        }
        delay(12);
    }
}

int main() {
    setupGame();
    menu();
    while (1) {
        clear();
        gameLoop();
        gameCrashed(rocket_x, rocket_y);
    }
    return 0;
}
