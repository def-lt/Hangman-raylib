#include "raylib.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

// Рисуем человечка в зависимости от количества ошибок
void DrawHangman(int errors) {
    // Стойка
    DrawLine(100, 420, 100, 80, BLACK);       // вертикальная стойка
    DrawLine(100, 80, 250, 80, BLACK);        // горизонтальная перекладина
    DrawLine(250, 80, 250, 130, BLACK);       // верёвка

    // Голова
    if (errors >= 1) DrawCircleLines(250, 160, 30, BLACK);

    // Тело
    if (errors >= 2) DrawLine(250, 190, 250, 290, BLACK);

    // Руки
    if (errors >= 3) DrawLine(250, 220, 200, 270, BLACK); // левая
    if (errors >= 4) DrawLine(250, 220, 300, 270, BLACK); // правая

    // Ноги
    if (errors >= 5) DrawLine(250, 290, 200, 360, BLACK); // левая
    if (errors >= 6) DrawLine(250, 290, 300, 360, BLACK); // правая
}

int main() {
    const int screenWidth = 900;
    const int screenHeight = 550;

    InitWindow(screenWidth, screenHeight, "Виселица / Hangman");
    SetTargetFPS(60);

    // Список слов (можно менять)
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
        "оператор"
    };

    srand((unsigned)time(0));
    string word = words[rand() % words.size()];
    string hidden(word.length(), '_');
    string usedLetters = "";
    int errors = 0;
    bool gameOver = false;
    bool win = false;

    while (!WindowShouldClose()) {
        // === Ввод ===
        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                // Принимаем русские и английские буквы
                if ((key >= 'а' && key <= 'я') || (key >= 'А' && key <= 'Я') ||
                    (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {

                    char letter = (char)tolower(key);

                    // Проверяем, не называли ли уже эту букву
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

        // === Рисование ===
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Заголовок
        DrawText("ВИСЕЛИЦА", 350, 20, 40, DARKBLUE);

        // Человечек
        DrawHangman(errors);

        // Слово
        DrawText("Слово:", 420, 140, 28, BLACK);
        DrawText(hidden.c_str(), 420, 180, 40, DARKGREEN);

        // Использованные буквы
        DrawText("Использованные буквы:", 420, 260, 22, GRAY);
        DrawText(usedLetters.c_str(), 420, 295, 26, BLACK);

        // Счётчик ошибок
        DrawText(TextFormat("Ошибок: %d / 6", errors), 420, 350, 26, RED);

        // Сообщения
        if (gameOver) {
            if (win) {
                DrawText("ПОБЕДА!", 350, 430, 50, GREEN);
            } else {
                DrawText("ПРОИГРЫШ", 320, 430, 50, RED);
                DrawText(TextFormat("Слово было: %s", word.c_str()), 280, 490, 28, BLACK);
            }
        } else {
            DrawText("Вводи буквы с клавиатуры", 280, 480, 26, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
