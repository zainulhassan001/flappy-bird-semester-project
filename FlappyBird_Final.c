#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Define screen dimensions and game settings
#define SCREEN_WIDTH  32
#define SCREEN_HEIGHT 16
#define PIPE_COUNT    3
#define QUIT_KEY      0x51                // Windows virtual key code for 'Q'
#define GREEN         "\e[32m"
#define YELLOW        "\e[33m"
#define RED           "\e[31m"
#define RESET         "\e[0m"
#define SCORES_FILE   "scores.txt"

// Structure to hold coordinates
typedef struct {
    int x;
    int y;
} Coordinate;

// Structure to hold score entries
typedef struct {
    char name[50];
    int score;
} ScoreEntry;

Coordinate bird;                    // Bird's position
Coordinate pipes[PIPE_COUNT];       // Pipes' positions
Coordinate powerUp;                 // Power-up's position
int score = 0;                      // Player's score
int powerUpActive = 0;              // Power-up state
int powerUpCounter = 0;             // Power-up timer
int gameSpeed = 100;                // Game speed

// Function to draw the game title
void Title() {
        printf("\t\t\t\t   _____  _                              ______  _           _ \n");    
        printf("\t\t\t\t  |  ___|| |                             | ___ \\(_)         | |\n");
        printf("\t\t\t\t  | |_   | |  __ _  _ __   _ __   _   _  | |_/ / _  _ __  __| |\n");
        printf("\t\t\t\t  |  _|  | | / _` || '_ \\ | '_ \\ | | | | | ___ \\| || '__|/ _` |\n");
        printf("\t\t\t\t  | |    | || (_| || |_) || |_) || |_| | | |_/ /| || |  | (_| |\n");
        printf("\t\t\t\t  \\_|    |_| \\__,_|| .__/ | .__/  \\__, | \\____/ |_||_|   \\__,_|\n");
        printf("\t\t\t\t                   | |    | |      __/ |                       \n");
        printf("\t\t\t\t                   |_|    |_|     |___/                        \n");
}
// Function to draw the game screen
void Draw() {
    char buffer[5000] = "\e[17A"; // Move cursor up 17 lines

    for (int y = 0; y <= SCREEN_HEIGHT; y++) {
        for (int x = 0; x <= SCREEN_WIDTH; x++) {
            if (y == 0 || y == SCREEN_HEIGHT || x == 0 || x == SCREEN_WIDTH) {
                strcat(buffer, RESET "[]"); // Screen edge
                continue;
            }

            int isPipe = 0;
            for (int i = 0; i < PIPE_COUNT; i++) {
                if (pipes[i].x == x && (y < pipes[i].y - 2 || y > pipes[i].y + 2)) {
                    strcat(buffer, GREEN "[]"); // Pipe
                    isPipe = 1;
                    break;
                }
            }

            if (!isPipe) {
                if (bird.y == y && bird.x == x) {
                    strcat(buffer, YELLOW ")>"); // Bird
                } else if (powerUp.x == x && powerUp.y == y) {
                    strcat(buffer, RED "@@"); // Power-up
                } else {
                    strcat(buffer, RESET "  "); // Empty space
                }
            }
        }
        strcat(buffer, "\n");
    }

    printf("%s", buffer); // Print buffer
}

// Function to update pipes' positions
void UpdatePipes() {
    for (int i = 0; i < PIPE_COUNT; i++) {
        if (pipes[i].x == -1) {
            pipes[i].x = (i == 0 ? pipes[PIPE_COUNT - 1].x : pipes[i - 1].x) + 15;
            pipes[i].y = (rand() % 7) + 5;
            score++; // Increase score when a pipe is reset
        }
    }
}

// Function to check for collisions
int CheckCollisions() {
    if (bird.y >= SCREEN_HEIGHT - 1 || bird.y <= 0) { // Check for collision with ground or ceiling
        return 1; // Bird hit the floor or ceiling
    }

    for (int i = 0; i < PIPE_COUNT; i++) {
        if (bird.x == pipes[i].x && (bird.y < pipes[i].y - 2 || bird.y > pipes[i].y + 2)) {
            return 1; // Bird hit a pipe
        }
    }
    return 0;
}

// Function to save the score to a file
void SaveScore(const char* name, int score) {
    FILE *file = fopen(SCORES_FILE, "a");
    if (file != NULL) {
        fprintf(file, "%s %d\n", name, score);
        fclose(file);
    }
}

// Function to play a sound
void PlayGameSound(LPCSTR sound, DWORD flags) {
    PlaySound(NULL, 0, 0); // Stop any playing sound
    PlaySound(sound, NULL, flags);
}

// Function to start the game
void StartGame() {
    PlayGameSound(TEXT("assets/quack.wav"), SND_LOOP | SND_ASYNC); // Play continuous quacking sound

    while (1) {
        // Initialize game state
        bird.x = 10;
        bird.y = 10;
        score = 0;
        powerUp.x = -1;
        powerUp.y = -1;
        powerUpActive = 0;
        powerUpCounter = 0;

        // Initialize pipes' positions
        for (int i = 0; i < PIPE_COUNT; i++) {
            pipes[i].x = 25 + 15 * i;
            pipes[i].y = (rand() % 7) + 5;
        }

        system("cls");
        printf("Press UP to jump and Q to quit.\n");
        for (int i = 0; i <= SCREEN_HEIGHT; i++) {
            printf("\n");
        }

        Draw();
        system("pause>nul");


        int frameCounter = 0;
        while (1) {
            if (GetAsyncKeyState(VK_UP)) {
                bird.y -= 2; // Move the bird up
            }

            if (GetAsyncKeyState(QUIT_KEY)) {
                PlaySound(NULL, 0, 0); // Stop quacking sound
                printf("Final Score: %d\n", score);
                char name[50];
                printf("Enter your name: ");
                scanf("%s", name);
                SaveScore(name, score);
                return;
            }

            if (frameCounter == 2) {
                bird.y++; // Gravity effect
                for (int i = 0; i < PIPE_COUNT; i++) {
                    pipes[i].x--; // Move pipes left
                }
                frameCounter = 0;

                if (rand() % 100 < 5 && powerUp.x == -1) { // 5% chance to spawn power-up
                    powerUp.x = SCREEN_WIDTH;
                    powerUp.y = (rand() % (SCREEN_HEIGHT - 2)) + 1;
                }
            }

            if (powerUp.x != -1) {
                powerUp.x--; // Move power-up left
                if (powerUp.x == bird.x && powerUp.y == bird.y) {
                    powerUpActive = 1;
                    powerUpCounter = 10; // Power-up active for 10 frames
                    score += 10; // Add 10 to score
                    PlayGameSound(TEXT("assets/powerup.wav"), SND_ASYNC);
                    powerUp.x = -1; // Reset power-up position
                    powerUp.y = -1;
                } else if (powerUp.x < 0) {
                    powerUp.x = -1; // Remove power-up if it goes off-screen
                }
            }

            if (powerUpActive) {
                powerUpCounter--;
                if (powerUpCounter <= 0) {
                    powerUpActive = 0;
                }
            } else {
                if (CheckCollisions()) {
                    PlaySound(NULL, 0, 0); // Stop quacking sound
                    PlayGameSound(TEXT("assets/hit.wav"), SND_ASYNC); // Play hit sound
                    printf("Game Over! Your score: %d\n", score);
                    char name[50];
                    printf("Enter your name: ");
                    scanf("%s", name);
                    SaveScore(name, score);
                    break;
                }
            }

            Draw();
            UpdatePipes();

            frameCounter++;
            Sleep(gameSpeed); // Delay based on game speed
        }

        printf("Do you want to play again? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            PlaySound(NULL, 0, 0); // Stop quacking sound
            return;
        } else {
            PlayGameSound(TEXT("assets/quack.wav"), SND_LOOP | SND_ASYNC); // Restart quacking sound when playing again
        }
    }
}

// Comparison function for qsort to sort scores in descending order
int compareScores(const void *a, const void *b) {
    ScoreEntry *entryA = (ScoreEntry *)a;
    ScoreEntry *entryB = (ScoreEntry *)b;
    return entryB->score - entryA->score;
}

// Function to display the scores
void ShowScores() {
    FILE *file = fopen(SCORES_FILE, "r");
    if (file == NULL) {
        printf("No scores available.\n");
        return;
    }

    ScoreEntry scores[100];
    int count = 0;

    while (fscanf(file, "%s %d", scores[count].name, &scores[count].score) != EOF && count < 100) {
        count++;
    }
    fclose(file);

    qsort(scores, count, sizeof(ScoreEntry), compareScores);

    system("cls");
    Title();
    for (int i = 0; i < count; i++) {
        printf("%s: %d\n", scores[i].name, scores[i].score);
    }
    system("pause");
}

// Function to show instructions
void Instructions() {
    system("cls");
    Title();
    printf("Instructions:\n");
    printf("Press any button to start the game. Press 'Q' to quit the game.\n");
    printf("Avoid hitting the floor, ceiling, and the pipes to survive in the game.\n");
    printf("As you pass pipes, your score will increase.\n");
    printf("Passing 1 pipe gives one point to the player.\n");
    printf("It is an endless game which ends when the user hits an obstacle.\n");
    printf("Consume a power-up to add 10 points.\n");
    system("pause");
}

// Function to show credits
void Credits() {
    system("cls");
    Title();
    printf("Credits:\n");
    printf("Flappy Bird Game Developed by:\n");
    printf("1. Zain Ul Hassan\n");
    printf("2. Hira Tauseef\n");
    printf("3. Arslan Khan\n");
    system("pause");
}

// Function to select difficulty level
void DifficultyLevel() {
    system("cls");
    Title();
    printf("Select Difficulty Level:\n");
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");
    printf("Select an option: ");
    char choice;
    scanf(" %c", &choice);

    switch (choice) {
        case '1':
            gameSpeed = 100; // Easy
            break;
        case '2':
            gameSpeed = 75; // Medium
            break;
        case '3':
            gameSpeed = 50; // Hard
            break;
        default:
            gameSpeed = 100; 
    }
}

// Main menu function
void Menu() {
    PlayGameSound(TEXT("assets/title_song.wav"), SND_LOOP | SND_ASYNC); // Play title song
    while (1) {
        system("cls");
        Title();
        printf("1. Start Game\n");
        printf("2. Score Board\n");
        printf("3. Instructions\n");
        printf("4. Credits\n");
        printf("5. Quit\n");
        printf("Select an option: ");
        char choice;
        scanf(" %c", &choice);
        
        switch (choice) {
            case '1':
                PlaySound(NULL, 0, 0); // Stop title song
                DifficultyLevel();
                StartGame();
                PlayGameSound(TEXT("assets/title_song.wav"), SND_LOOP | SND_ASYNC); // Restart title song after game
                break;
            case '2':
                ShowScores();
                break;
            case '3':
                Instructions();
                break;
            case '4':
                Credits();
                break;
            case '5':
                PlaySound(NULL, 0, 0); // Stop title song
                exit(0);
            default:
                Sleep(0); // Clear the screen
        }
    }
}

// Main function
int main() {
    srand(time(0)); // Seed the random number generator
    Menu(); // Display the menu
    return 0;
}