#include "raylib.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

// Draw the hangman based on number of errors
void DrawHangman(int errors) {
    // Gallows
    DrawLine(100, 420, 100, 80, BLACK);       // vertical pole
    DrawLine(100, 80, 250, 80, BLACK);        // horizontal beam
    DrawLine(250, 80, 250, 130, BLACK);       // rope

    // Head
    if (errors >= 1) DrawCircleLines(250, 160, 30, BLACK);

    // Body
    if (errors >= 2) DrawLine(250, 190, 250, 290, BLACK);

    // Arms
    if (errors >= 3) DrawLine(250, 220, 200, 270, BLACK); // left
    if (errors >= 4) DrawLine(250, 220, 300, 270, BLACK); // right

    // Legs
    if (errors >= 5) DrawLine(250, 290, 200, 360, BLACK); // left
    if (errors >= 6) DrawLine(250, 290, 300, 360, BLACK); // right
}

int main() {
    const int screenWidth = 900;
    const int screenHeight = 550;

    InitWindow(screenWidth, screenHeight, "Hangman");
    SetTargetFPS(60);

    // List of words
    vector<string> words = {
        "programming",
        "computer",
        "algorithm",
        "variable",
        "function",
        "array",
        "compiler",
        "interface",
        "processor",
        "operator"
    };

    srand((unsigned)time(0));
    string word = words[rand() % words.size()];
    string hidden(word.length(), '_');
    string usedLetters = "";
    int errors = 0;
    bool gameOver = false;
    bool win = false;

    while (!WindowShouldClose()) {
        // === Input ===
        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                // Accept only English letters
                if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {

                    char letter = (char)tolower(key);

                    // Check if letter was already used
                    if (usedLetters.find(letter) == string::npos) {
                        usedLetters += letter;
                        usedLetters += " ";

                        bool found = false;
                        for (size_t i = 0; i < word.length(); i++) {
                            if (word[i] == letter) {
                                hidden[i] = letter;
                                found = true;
                            }
                        }

                        if (!found) {
                            errors++;
                            if (errors >= 6) {
                                gameOver = true;
                                win = false;
                            }
                        }

                        if (hidden == word) {
                            gameOver = true;
                            win = true;
                        }
                    }
                }
                key = GetCharPressed();
            }
        }

        // === Drawing ===
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Title
        DrawText("HANGMAN", 350, 20, 40, DARKBLUE);

        // Hangman figure
        DrawHangman(errors);

        // Word
        DrawText("Word:", 420, 140, 28, BLACK);
        DrawText(hidden.c_str(), 420, 180, 40, DARKGREEN);

        // Used letters
        DrawText("Used letters:", 420, 260, 22, GRAY);
        DrawText(usedLetters.c_str(), 420, 295, 26, BLACK);

        // Error counter
        DrawText(TextFormat("Errors: %d / 6", errors), 420, 350, 26, RED);

        // End messages
        if (gameOver) {
            if (win) {
                DrawText("YOU WIN!", 350, 430, 50, GREEN);
            } else {
                DrawText("YOU LOSE", 340, 430, 50, RED);
                DrawText(TextFormat("The word was: %s", word.c_str()), 280, 490, 28, BLACK);
            }
        } else {
            DrawText("Type letters on the keyboard", 260, 480, 26, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
