# Flappy Bird in C

A console-based Flappy Bird game written in C as a Programming Fundamentals semester project.
Includes a menu system, difficulty modes, sound effects, scoring, and a power-up feature.

## Features

- Console-based gameplay
- Random pipe generation
- Easy, Medium, and Hard difficulty modes
- Score tracking and leaderboard
- Sound effects and background music
- Power-up for bonus points
- Replay option after game over

## Screenshots

![Main Menu](screenshots/menu.png)
![Gameplay](screenshots/gameplay.png)
![Scoreboard](screenshots/scoreboard.png)

## Project Structure

```text
flappy-bird-c/
  README.md
  .gitignore
  scores.txt
  FlappyBird_Final.c
  assets/
    hit.wav
    powerup.wav
    quack.wav
    title_song.wav
```

## Requirements

- Windows OS
- GCC or MinGW compiler
- Terminal with ANSI escape support

## Build

Run from the project root:

```bash
gcc FlappyBird_Final.c -o FlappyBird.exe -lwinmm
```

## Run

```bash
FlappyBird.exe
```

## Controls

- Up Arrow - Jump
- Q - Quit during gameplay


## Credits

- Zain Ul Hassan
- Hira Tauseef
- Arslan Khan
