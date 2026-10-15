#include "raylib.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

// Рисуем человечка
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

    InitWindow(screenWidth, screenHeight, "Виселица");
    SetTargetFPS(60);

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

        // Перезапуск по R
        if (gameOver && IsKeyPressed(KEY_R)) {
            NewGame(word, hidden, usedLetters, errors, gameOver, win, words);
        }

        // Ввод букв
        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                // Преобразуем нажатую клавишу в строку из одного символа
                string letter(1, (char)key);   // для ASCII
                
                // Для кириллицы GetCharPressed возвращает код > 127
                // Мы будем искать эту букву прямо в загаданном слове
                // Самый простой рабочий способ:

                bool alreadyUsed = false;
                for (char c : usedLetters) {
                    if (c == (char)key) {
                        alreadyUsed = true;
                        break;
                    }
                }

                if (!alreadyUsed && key > 32) {
                    usedLetters += (char)key;
                    usedLetters += " ";

                    bool found = false;
                    for (size_t i = 0; i < word.length(); i++) {
                        if (word[i] == (char)key) {
                            hidden[i] = word[i];
                            found = true;
                        }
                    }

                    // Дополнительно проверяем маленькие/большие (на всякий случай)
                    char lowerKey = (char)tolower(key);
                    for (size_t i = 0; i < word.length(); i++) {
                        if (word[i] == lowerKey) {
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

                key = GetCharPressed();
            }
        }

        // Рисование
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("ВИСЕЛИЦА", 350, 20, 40, DARKBLUE);
        DrawHangman(errors);

        DrawText("Слово:", 420, 140, 28, BLACK);
        DrawText(hidden.c_str(), 420, 180, 40, DARKGREEN);

        DrawText("Использованные буквы:", 420, 260, 22, GRAY);
        DrawText(usedLetters.c_str(), 420, 295, 26, BLACK);

        DrawText(TextFormat("Ошибок: %d / 6", errors), 420, 350, 26, RED);

        if (gameOver) {
            if (win) {
                DrawText("ПОБЕДА!", 350, 430, 50, GREEN);
            } else {
                DrawText("ПРОИГРЫШ", 320, 430, 50, RED);
                DrawText(TextFormat("Слово было: %s", word.c_str()), 250, 485, 26, BLACK);
            }
            DrawText("Нажми R чтобы начать заново", 260, 520, 22, DARKGRAY);
        } else {
            DrawText("Вводи буквы с клавиатуры", 280, 480, 26, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
