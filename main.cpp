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

    InitWindow(screenWidth, screenHeight, "Viselica");
    SetTargetFPS(60);

    // Загружаем системный шрифт Windows, который поддерживает русский
    Font font = LoadFont("C:/Windows/Fonts/arial.ttf");
    if (font.texture.id == 0) {
        // Если Arial не найден — пробуем другой
        font = LoadFont("C:/Windows/Fonts/segoeui.ttf");
    }

    vector<string> words = {
        "программирование",
        "компьютер",
        "алгоритм",
        "переменная",
        "функция",
        "массив",
        "компилятор",
        "интерфейс",
        "процессор",
        "оператор",
        "память",
        "цикл",
        "условие",
        "класс",
        "объект"
    };

    srand((unsigned)time(0));

    string word, hidden, usedLetters;
    int errors = 0;
    bool gameOver = false;
    bool win = false;

    NewGame(word, hidden, usedLetters, errors, gameOver, win, words);

    while (!WindowShouldClose()) {

        if (gameOver && IsKeyPressed(KEY_R)) {
            NewGame(word, hidden, usedLetters, errors, gameOver, win, words);
        }

        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key > 32) {
                    char ch = (char)key;
                    bool alreadyUsed = (usedLetters.find(ch) != string::npos);

                    if (!alreadyUsed) {
                        usedLetters += ch;
                        usedLetters += " ";

                        bool found = false;
                        for (size_t i = 0; i < word.size(); i++) {
                            if (word[i] == ch) {
                                hidden[i] = word[i];
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

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Используем загруженный шрифт
        DrawTextEx(font, "ВИСЕЛИЦА", Vector2{350, 20}, 40, 2, DARKBLUE);
        DrawHangman(errors);

        DrawTextEx(font, "Слово:", Vector2{420, 140}, 28, 1, BLACK);
        DrawTextEx(font, hidden.c_str(), Vector2{420, 180}, 40, 2, DARKGREEN);

        DrawTextEx(font, "Использованные буквы:", Vector2{420, 260}, 22, 1, GRAY);
        DrawTextEx(font, usedLetters.c_str(), Vector2{420, 295}, 26, 1, BLACK);

        DrawTextEx(font, TextFormat("Ошибок: %d / 6", errors), Vector2{420, 350}, 26, 1, RED);

        if (gameOver) {
            if (win) {
                DrawTextEx(font, "ПОБЕДА!", Vector2{350, 430}, 50, 2, GREEN);
            } else {
                DrawTextEx(font, "ПРОИГРЫШ", Vector2{320, 430}, 50, 2, RED);
                DrawTextEx(font, TextFormat("Слово было: %s", word.c_str()), Vector2{250, 485}, 26, 1, BLACK);
            }
            DrawTextEx(font, "Нажми R чтобы начать заново", Vector2{260, 520}, 22, 1, DARKGRAY);
        } else {
            DrawTextEx(font, "Вводи буквы с клавиатуры", Vector2{280, 480}, 26, 1, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}
