#include "raylib.h"
#include "hangman_logic.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

const int screenWidth = 900;
const int screenHeight = 600;

// Экранная клавиатура: 2 ряда по 13 букв
const int KEY_SIZE = 50;
const int KEY_GAP = 8;
const int KEYS_PER_ROW = 13;
const int KEYBOARD_X = (screenWidth - (KEYS_PER_ROW * (KEY_SIZE + KEY_GAP) - KEY_GAP)) / 2;
const int KEYBOARD_Y = 450;

Rectangle KeyRect(int i) {
    int row = i / KEYS_PER_ROW;
    int col = i % KEYS_PER_ROW;
    return { (float)(KEYBOARD_X + col * (KEY_SIZE + KEY_GAP)),
             (float)(KEYBOARD_Y + row * (KEY_SIZE + KEY_GAP)),
             (float)KEY_SIZE, (float)KEY_SIZE };
}

void DrawCentered(const char* text, int y, int size, Color color) {
    DrawText(text, (screenWidth - MeasureText(text, size)) / 2, y, size, color);
}

void DrawHangman(int errors) {
    Color wood = { 120, 80, 40, 255 };
    // Виселица
    DrawRectangle(40, 400, 220, 12, wood);
    DrawLineEx({ 90, 400 }, { 90, 70 }, 8, wood);
    DrawLineEx({ 86, 70 }, { 230, 70 }, 8, wood);
    DrawLineEx({ 90, 120 }, { 140, 70 }, 6, wood);
    DrawLineEx({ 220, 70 }, { 220, 120 }, 3, DARKGRAY);

    Color body = errors >= HangmanGame::MAX_ERRORS ? MAROON : BLACK;
    if (errors >= 1) DrawRing({ 220, 150 }, 26, 30, 0, 360, 36, body);
    if (errors >= 2) DrawLineEx({ 220, 180 }, { 220, 280 }, 4, body);
    if (errors >= 3) DrawLineEx({ 220, 205 }, { 175, 255 }, 4, body);
    if (errors >= 4) DrawLineEx({ 220, 205 }, { 265, 255 }, 4, body);
    if (errors >= 5) DrawLineEx({ 220, 278 }, { 180, 350 }, 4, body);
    if (errors >= 6) {
        DrawLineEx({ 220, 278 }, { 260, 350 }, 4, body);
        // крестики вместо глаз
        DrawLine(208, 140, 216, 148, body); DrawLine(216, 140, 208, 148, body);
        DrawLine(224, 140, 232, 148, body); DrawLine(232, 140, 224, 148, body);
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Hangman");
    SetTargetFPS(60);
    srand((unsigned)time(0));

    vector<WordEntry> words = {
        { "programming", "IT" },   { "computer", "IT" },   { "algorithm", "IT" },
        { "variable", "IT" },      { "function", "IT" },   { "array", "IT" },
        { "compiler", "IT" },      { "interface", "IT" },  { "processor", "IT" },
        { "operator", "IT" },      { "memory", "IT" },     { "cycle", "IT" },
        { "condition", "IT" },     { "class", "IT" },      { "object", "IT" },
        { "elephant", "Animals" }, { "giraffe", "Animals" }, { "penguin", "Animals" },
        { "dolphin", "Animals" },  { "kangaroo", "Animals" },
        { "banana", "Food" },      { "pizza", "Food" },    { "chocolate", "Food" },
        { "sandwich", "Food" },    { "london", "Cities" }, { "moscow", "Cities" },
        { "paris", "Cities" },     { "tokyo", "Cities" }
    };

    HangmanGame game;
    int wordIndex = -1;
    int wins = 0, losses = 0;
    string message;
    float messageTimer = 0;
    bool scoreCounted = false;

    auto newGame = [&]() {
        wordIndex = PickWordIndex((int)words.size(), wordIndex);
        game.Start(words[wordIndex].word);
        scoreCounted = false;
        message.clear();
        while (GetCharPressed() > 0) {} // не переносим нажатия в новую игру
    };

    auto handleGuess = [&](int key) {
        GuessResult r = game.Guess(key);
        if (r == GuessResult::AlreadyUsed) {
            message = TextFormat("Letter '%c' was already used", HangmanGame::ToLower((char)key));
            messageTimer = 1.5f;
        } else if (r == GuessResult::Invalid) {
            message = "Only English letters A-Z";
            messageTimer = 1.5f;
        }
    };

    newGame();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (messageTimer > 0) messageTimer -= dt;

        // Ввод
        if (game.IsOver()) {
            if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                newGame();
        } else {
            int key = GetCharPressed();
            while (key > 0) {
                if (key != ' ') handleGuess(key);
                key = GetCharPressed();
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 m = GetMousePosition();
                for (int i = 0; i < 26; i++)
                    if (CheckCollisionPointRec(m, KeyRect(i))) handleGuess('a' + i);
            }
        }

        if (game.IsOver() && !scoreCounted) {
            if (game.IsWin()) wins++; else losses++;
            scoreCounted = true;
        }

        // Отрисовка
        BeginDrawing();
        ClearBackground({ 245, 240, 230, 255 });

        DrawText("HANGMAN", 330, 15, 44, DARKBLUE);
        DrawText(TextFormat("Wins: %d   Losses: %d", wins, losses), 660, 25, 20, DARKGRAY);

        DrawHangman(game.Errors());

        DrawText(TextFormat("Category: %s", words[wordIndex].category.c_str()), 330, 100, 24, DARKPURPLE);

        string shown = game.IsOver() ? "" : game.Display();
        if (game.IsOver()) {
            // после конца игры показываем слово целиком
            for (size_t i = 0; i < game.Word().size(); i++) {
                if (i > 0) shown += ' ';
                shown += game.Word()[i];
            }
        }
        DrawText(shown.c_str(), 330, 160, 40, game.IsOver() && !game.IsWin() ? MAROON : DARKGREEN);
        DrawText(TextFormat("%d letters", (int)game.Word().size()), 330, 210, 18, GRAY);

        // Жизни
        DrawText("Lives:", 330, 260, 24, BLACK);
        for (int i = 0; i < HangmanGame::MAX_ERRORS; i++) {
            Color c = i < game.ErrorsLeft() ? RED : LIGHTGRAY;
            DrawCircle(430 + i * 32, 272, 11, c);
        }
        DrawText(TextFormat("Errors: %d / %d", game.Errors(), HangmanGame::MAX_ERRORS), 330, 300, 22, MAROON);

        if (messageTimer > 0) DrawText(message.c_str(), 330, 345, 22, ORANGE);

        // Экранная клавиатура
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < 26; i++) {
            char c = 'a' + i;
            Rectangle r = KeyRect(i);
            Color bg = WHITE;
            if (game.IsUsed(c)) bg = game.IsInWord(c) ? Color{ 120, 200, 120, 255 } : Color{ 230, 120, 120, 255 };
            else if (!game.IsOver() && CheckCollisionPointRec(mouse, r)) bg = SKYBLUE;
            DrawRectangleRounded(r, 0.2f, 6, bg);
            DrawRectangleRoundedLinesEx(r, 0.2f, 6, 2, DARKGRAY);
            char label[2] = { (char)(c - 'a' + 'A'), 0 };
            DrawText(label, (int)r.x + (KEY_SIZE - MeasureText(label, 28)) / 2, (int)r.y + 11, 28, BLACK);
        }

        if (game.IsOver()) {
            DrawRectangle(0, 380, screenWidth, 220, Fade(BLACK, 0.92f));
            if (game.IsWin()) DrawCentered("YOU WIN!", 410, 56, GREEN);
            else DrawCentered("YOU LOSE", 410, 56, RED);
            DrawCentered(TextFormat("The word was: %s", game.Word().c_str()), 480, 28, RAYWHITE);
            DrawCentered("Press R, ENTER or click to play again", 530, 22, LIGHTGRAY);
        } else {
            DrawCentered("Type letters on the keyboard or click the buttons", 570, 20, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
