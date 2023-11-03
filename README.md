# Microprocessors Project 1

**CMPU2013 Microprocessors Module Group Project**

*Developed by Hayden Carroll and Sam Curran*

# The Space Dodger Game

Space Dodger is a simple 2D game implemented on an STM32 microcontroller. The game involves maneuvering a rocket to dodge falling asteroids and score points. This README provides an overview of the code structure and its components.

## Table of Contents
- [Introduction](#introduction)
- [Code Overview](#code-overview)
- [Functionality](#functionality)
- [Game Logic](#game-logic)
- [How to Play](#how-to-play)
- [Contributing](#contributing)

## Introduction

Space Dodgers is designed as a fun project to explore embedded programming using the STM32 microcontroller. It utilizes C programming and integrates with various hardware components, including an LED display, buttons, and sound for an interactive gaming experience.

## Code Overview

The code for Space Dodger is organized into several functions and components:

- **setupGame:** Initializes the game environment and system components.
- **menu:** Displays the game menu, including the highest completed level, and allows the player to start or load a game.
- **countdown:** Provides a countdown to the game start.
- **gameLoop:** Contains the core game logic, such as asteroid movement and collision detection.
- **initAsteroids:** Initializes the asteroid objects for each level.
- **drawAsteroids:** Displays the asteroids on the screen.
- **updateAsteroids:** Updates the position and speed of asteroids.
- **handlePlayerInput:** Processes player input to move the rocket.
- **updateRocketPosition:** Updates and draws the rocket's position.
- **checkCollision:** Checks for collisions between the rocket and asteroids.
- **levelUp:** Handles level progression.
- **gameCrashed:** Handles game over scenarios.
- **loadGame:** Loads the game at a specific level.
- **saveHighestLevel:** Saves the highest completed level to non-volatile memory.
- **loadHighestLevel:** Loads the highest completed level from non-volatile memory.

## Functionality

Space Dodger provides the following features:

- Multiple levels with increasing difficulty.
- Player rocket that can be moved using buttons.
- Falling asteroids with varying speeds.
- Collision detection and game over screen.
- Level progression

## Game Logic

Space Dodger consists of different game levels, each defined by a structure that includes the number of asteroids and their maximum speed. As the player progresses through the game, the number and speed of asteroids increase. The player's goal is to dodge the falling asteroids using the rocket.

## How to Play

1. Upload the code to your STM32 microcontroller.
2. Connect the required hardware components (display, buttons, and sound).
3. Start the game by pressing a button when prompted in the menu.
4. Control the rocket's movement using the left and right buttons.
5. Dodge the falling asteroids to increase your score.
6. If your rocket collides with an asteroid, the game will display a game over message and allow you to restart.

Enjoy playing Space Dodger!



