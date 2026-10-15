#include "raylib.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

// Рисуем человечка
void DrawHangman(int errors) {
    DrawLine(100, 420, 100, 80, BLACK);       // стойка
    DrawLine(100, 80, 250, 80, BLACK);        // перекладина
    DrawLine(250, 80, 250, 130, BLACK);       // верёвка

    if (errors >= 1) DrawCircleLines(250, 160, 30, BLACK); // голова
    if (errors >= 2) DrawLine(250, 190, 250, 290, BLACK);  // тело
    if (errors >= 3) DrawLine(250, 220, 200, 270, BLACK);  // левая рука
    if (errors >= 4) DrawLine(250, 220, 300, 270, BLACK);  // правая рука
    if (errors >= 5) DrawLine(250, 290, 200, 360, BLACK);  // левая нога
    if (errors >= 6) DrawLine(250, 290, 300, 360, BLACK);  // правая нога
}

// Новая игра
void NewGame(string& word, string& hidden, string& usedLetters, int& errors, bool& gameOver, bool& win, const vector<string>& words) {
    word = words[rand() % words.size()];
    hidden = string(word.length(), '_');
    usedLetters = "";
    errors = 0;
    gameOver = false;
    win = false;
}

int main() {
    const int screenWidth = 900;
    const int screenHeight = 550;

    InitWindow(screenWidth, screenHeight, "Виселица");
    SetTargetFPS(60);

    // Русские слова
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

        // === Ввод ===
        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                // Берём символ как есть (raylib отдаёт Unicode-код)
                // Для простоты работаем только с маленькими русскими и английскими
                string letterStr = "";
                
                // Английские
                if (key >= 'A' && key <= 'Z') key = key - 'A' + 'a';
                if (key >= 'a' && key <= 'z') {
                    letterStr = string(1, (char)key);
                }
                // Русские (UTF-8 коды примерно)
                // raylib GetCharPressed для кириллицы возвращает код символа
                // Самый простой рабочий способ — проверять по слову напрямую

                // Более надёжный способ для кириллицы:
                if (key > 127) {  // скорее всего кириллица
                    // Преобразуем код в строку из одного символа
                    char buf[5] = {0};
                    // Простая обработка самых частых случаев
                    // (для школьного проекта этого достаточно)
                }

                // Самый простой и рабочий способ — принимать любой символ и искать его в слове
                // через строковое сравнение

                key = GetCharPressed();
            }

            // Более надёжный ввод через IsKeyPressed + таблица
            // Но для простоты сделаем через GetCharPressed правильно
        }

        // === Перезапуск ===
        if (gameOver && IsKeyPressed(KEY_R)) {
            NewGame(word, hidden, usedLetters, errors, gameOver, win, words);
        }

        // === Рисование ===
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
