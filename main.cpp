#include "raylib.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

void DrawHangman(int errors) {
    DrawLine(100, 420, 100, 80, BLACK);
    DrawLine(100, 80, 250, 80, BLACK);
    DrawLine(250, 80, 250, 130, BLACK);

    if (errors >= 1) DrawCircleLines(250, 160, 30, BLACK);
    if (errors >= 2) DrawLine(250, 190, 250, 290, BLACK);
    if (errors >= 3) DrawLine(250, 220, 200, 270, BLACK);
    if (errors >= 4) DrawLine(250, 220, 300, 270, BLACK);
    if (errors >= 5) DrawLine(250, 290, 200, 360, BLACK);
    if (errors >= 6) DrawLine(250, 290, 300, 360, BLACK);
}

void NewGame(string& word, string& hidden, string& usedLetters, int& errors, bool& gameOver, bool& win, const vector<string>& words) {
    word = words[rand() % words.size()];
    hidden.assign(word.length(), '_');
    usedLetters.clear();
    errors = 0;
    gameOver = false;
    win = false;
}

int main() {
    const int screenWidth = 900;
    const int screenHeight = 550;

    InitWindow(screenWidth, screenHeight, "Hangman");
    SetTargetFPS(60);

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
        "operator",
        "memory",
        "cycle",
        "condition",
        "class",
        "object"
    };

    srand((unsigned)time(0));

    string word, hidden, usedLetters;
    int errors = 0;
    bool gameOver = false;
    bool win = false;

    NewGame(word, hidden, usedLetters, errors, gameOver, win, words);

    while (!WindowShouldClose()) {

        // Restart on R
        if (gameOver && IsKeyPressed(KEY_R)) {
            NewGame(word, hidden, usedLetters, errors, gameOver, win, words);
        }

        // Input
        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z')) {
                    char letter = (char)tolower(key);

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

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("HANGMAN", 350, 20, 40, DARKBLUE);
        DrawHangman(errors);

        DrawText("Word:", 420, 140, 28, BLACK);
        DrawText(hidden.c_str(), 420, 180, 40, DARKGREEN);

        DrawText("Used letters:", 420, 260, 22, GRAY);
        DrawText(usedLetters.c_str(), 420, 295, 26, BLACK);

        DrawText(TextFormat("Errors: %d / 6", errors), 420, 350, 26, RED);

        if (gameOver) {
            if (win) {
                DrawText("YOU WIN!", 350, 430, 50, GREEN);
            } else {
                DrawText("YOU LOSE", 340, 430, 50, RED);
                DrawText(TextFormat("The word was: %s", word.c_str()), 280, 485, 26, BLACK);
            }
            DrawText("Press R to play again", 300, 520, 22, DARKGRAY);
        } else {
            DrawText("Type letters on the keyboard", 260, 480, 26, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
