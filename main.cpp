#include "raylib.h"
#include "hangman_logic.h"
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

const int screenWidth = 900;
const int screenHeight = 650;

// Экранная клавиатура: EN — 2 ряда по 13, RU — 3 ряда по 11
const int KEY_SIZE = 50;
const int KEY_GAP = 8;
const int KEYBOARD_Y = 440;

// Тексты интерфейса на двух языках
struct UiText {
    const char* category;
    const char* letters;
    const char* lives;
    const char* errors;
    const char* score;
    const char* win;
    const char* lose;
    const char* wordWas;
    const char* playAgain;
    const char* hint;
    const char* alreadyUsed;
    const char* invalid;
    const char* wrongLayout;
};

const UiText TEXT_EN = {
    "Category: %s", "%d letters", "Lives:", "Errors: %d / %d", "Wins: %d   Losses: %d",
    "YOU WIN!", "YOU LOSE", "The word was: %s", "Press R, ENTER or click to play again",
    "Type letters or click the buttons.  F1 - language",
    "Letter '%s' was already used", "Only letters A-Z", "Switch keyboard layout to English"
};

const UiText TEXT_RU = {
    "Категория: %s", "Букв в слове: %d", "Жизни:", "Ошибки: %d / %d", "Победы: %d   Поражения: %d",
    "ПОБЕДА!", "ПОРАЖЕНИЕ", "Загаданное слово: %s", "R, ENTER или клик - новая игра",
    "Вводи буквы или нажимай кнопки.  F1 - язык",
    "Буква '%s' уже была", "Только буквы А-Я", "Переключи раскладку на русскую"
};

Font font;

void Text(const string& s, float x, float y, float size, Color color) {
    DrawTextEx(font, s.c_str(), { x, y }, size, 1, color);
}

float TextWidth(const string& s, float size) {
    return MeasureTextEx(font, s.c_str(), size, 1).x;
}

void TextCentered(const string& s, float y, float size, Color color) {
    Text(s, (screenWidth - TextWidth(s, size)) / 2, y, size, color);
}

// Шрифт из Windows с латиницей и кириллицей (встроенный шрифт raylib не умеет кириллицу)
Font LoadGameFont() {
    vector<int> codepoints;
    for (int c = 32; c < 127; c++) codepoints.push_back(c);
    for (int c = 0x410; c <= 0x44F; c++) codepoints.push_back(c);
    codepoints.push_back(0x401);
    codepoints.push_back(0x451);

    const char* paths[] = { "C:/Windows/Fonts/arial.ttf", "C:/Windows/Fonts/segoeui.ttf",
                            "C:/Windows/Fonts/tahoma.ttf" };
    for (const char* path : paths) {
        if (!FileExists(path)) continue;
        Font f = LoadFontEx(path, 64, codepoints.data(), (int)codepoints.size());
        if (f.texture.id != 0) {
            SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
            return f;
        }
    }
    return GetFontDefault();
}

int KeysPerRow(Language lang) { return lang == Language::English ? 13 : 11; }

Rectangle KeyRect(int i, Language lang) {
    int perRow = KeysPerRow(lang);
    int startX = (screenWidth - (perRow * (KEY_SIZE + KEY_GAP) - KEY_GAP)) / 2;
    int row = i / perRow;
    int col = i % perRow;
    return { (float)(startX + col * (KEY_SIZE + KEY_GAP)),
             (float)(KEYBOARD_Y + row * (KEY_SIZE + KEY_GAP)),
             (float)KEY_SIZE, (float)KEY_SIZE };
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
    InitWindow(screenWidth, screenHeight, "Hangman / Виселица");
    SetTargetFPS(60);
    srand((unsigned)time(0));
    font = LoadGameFont();

    vector<WordEntry> wordsEn = {
        { U"programming", "IT" },   { U"computer", "IT" },   { U"algorithm", "IT" },
        { U"variable", "IT" },      { U"function", "IT" },   { U"array", "IT" },
        { U"compiler", "IT" },      { U"interface", "IT" },  { U"processor", "IT" },
        { U"operator", "IT" },      { U"memory", "IT" },     { U"cycle", "IT" },
        { U"condition", "IT" },     { U"class", "IT" },      { U"object", "IT" },
        { U"elephant", "Animals" }, { U"giraffe", "Animals" }, { U"penguin", "Animals" },
        { U"dolphin", "Animals" },  { U"kangaroo", "Animals" },
        { U"banana", "Food" },      { U"pizza", "Food" },    { U"chocolate", "Food" },
        { U"sandwich", "Food" },    { U"london", "Cities" }, { U"moscow", "Cities" },
        { U"paris", "Cities" },     { U"tokyo", "Cities" }
    };

    vector<WordEntry> wordsRu = {
        { U"программа", "Информатика" }, { U"компьютер", "Информатика" },
        { U"алгоритм", "Информатика" },  { U"переменная", "Информатика" },
        { U"функция", "Информатика" },   { U"массив", "Информатика" },
        { U"компилятор", "Информатика" }, { U"процессор", "Информатика" },
        { U"память", "Информатика" },    { U"цикл", "Информатика" },
        { U"условие", "Информатика" },   { U"объект", "Информатика" },
        { U"слон", "Животные" },   { U"жираф", "Животные" },  { U"пингвин", "Животные" },
        { U"дельфин", "Животные" }, { U"кенгуру", "Животные" }, { U"ёжик", "Животные" },
        { U"банан", "Еда" },  { U"пицца", "Еда" },  { U"шоколад", "Еда" },
        { U"бутерброд", "Еда" }, { U"молоко", "Еда" },
        { U"москва", "Города" }, { U"лондон", "Города" }, { U"париж", "Города" },
        { U"казань", "Города" }, { U"токио", "Города" }
    };

    Language lang = Language::Russian;
    HangmanGame game;
    int wordIndex = -1;
    int wins = 0, losses = 0;
    string message;
    float messageTimer = 0;
    bool scoreCounted = false;

    auto words = [&]() -> const vector<WordEntry>& { return lang == Language::English ? wordsEn : wordsRu; };
    auto text = [&]() -> const UiText& { return lang == Language::English ? TEXT_EN : TEXT_RU; };

    auto newGame = [&]() {
        wordIndex = PickWordIndex((int)words().size(), wordIndex);
        game.Start(words()[wordIndex].word, lang);
        scoreCounted = false;
        message.clear();
        while (GetCharPressed() > 0) {} // не переносим нажатия в новую игру
    };

    auto switchLanguage = [&]() {
        lang = lang == Language::English ? Language::Russian : Language::English;
        wordIndex = -1;
        newGame();
    };

    auto handleGuess = [&](int key) {
        GuessResult r = game.Guess(key);
        if (r == GuessResult::AlreadyUsed) {
            message = TextFormat(text().alreadyUsed, ToUtf8(HangmanGame::ToUpper((char32_t)key)).c_str());
        } else if (r == GuessResult::Invalid) {
            message = text().invalid;
        } else if (r == GuessResult::WrongLayout) {
            message = text().wrongLayout;
        } else {
            return;
        }
        messageTimer = 2.0f;
    };

    Rectangle langButton = { 20, 15, 70, 36 };

    newGame();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (messageTimer > 0) messageTimer -= dt;
        u32string alphabet = HangmanGame::Alphabet(lang);
        Vector2 mouse = GetMousePosition();
        bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // Ввод
        if (IsKeyPressed(KEY_F1) || (clicked && CheckCollisionPointRec(mouse, langButton))) {
            switchLanguage();
            clicked = false;
        } else if (game.IsOver()) {
            if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER) || clicked)
                newGame();
        } else {
            int key = GetCharPressed();
            while (key > 0) {
                if (key != ' ') handleGuess(key);
                key = GetCharPressed();
            }
            if (clicked) {
                for (size_t i = 0; i < alphabet.size(); i++)
                    if (CheckCollisionPointRec(mouse, KeyRect((int)i, lang))) handleGuess(alphabet[i]);
            }
        }

        if (game.IsOver() && !scoreCounted) {
            if (game.IsWin()) wins++; else losses++;
            scoreCounted = true;
        }

        const UiText& t = text();

        // Отрисовка
        BeginDrawing();
        ClearBackground({ 245, 240, 230, 255 });

        TextCentered(lang == Language::English ? "HANGMAN" : "ВИСЕЛИЦА", 12, 48, DARKBLUE);
        string score = TextFormat(t.score, wins, losses);
        Text(score, screenWidth - TextWidth(score, 20) - 20, 22, 20, DARKGRAY);

        // Кнопка языка
        bool langHover = CheckCollisionPointRec(mouse, langButton);
        DrawRectangleRounded(langButton, 0.3f, 6, langHover ? SKYBLUE : WHITE);
        DrawRectangleRoundedLinesEx(langButton, 0.3f, 6, 2, DARKGRAY);
        const char* langLabel = lang == Language::English ? "EN" : "RU";
        Text(langLabel, langButton.x + (langButton.width - TextWidth(langLabel, 24)) / 2, langButton.y + 6, 24, DARKBLUE);

        DrawHangman(game.Errors());

        Text(TextFormat(t.category, words()[wordIndex].category.c_str()), 330, 95, 26, DARKPURPLE);

        string shown = game.IsOver() ? game.DisplayWord() : game.Display();
        Text(shown, 330, 150, 44, game.IsOver() && !game.IsWin() ? MAROON : DARKGREEN);
        Text(TextFormat(t.letters, (int)game.Word().size()), 330, 205, 20, GRAY);

        // Жизни
        Text(t.lives, 330, 255, 26, BLACK);
        for (int i = 0; i < HangmanGame::MAX_ERRORS; i++) {
            Color c = i < game.ErrorsLeft() ? RED : LIGHTGRAY;
            DrawCircle(450 + i * 32, 270, 11, c);
        }
        Text(TextFormat(t.errors, game.Errors(), HangmanGame::MAX_ERRORS), 330, 295, 24, MAROON);

        if (messageTimer > 0) Text(message, 330, 340, 24, ORANGE);

        // Экранная клавиатура
        for (size_t i = 0; i < alphabet.size(); i++) {
            char32_t c = alphabet[i];
            Rectangle r = KeyRect((int)i, lang);
            Color bg = WHITE;
            if (game.IsUsed(c)) bg = game.IsInWord(c) ? Color{ 120, 200, 120, 255 } : Color{ 230, 120, 120, 255 };
            else if (!game.IsOver() && CheckCollisionPointRec(mouse, r)) bg = SKYBLUE;
            DrawRectangleRounded(r, 0.2f, 6, bg);
            DrawRectangleRoundedLinesEx(r, 0.2f, 6, 2, DARKGRAY);
            string label = ToUtf8(HangmanGame::ToUpper(c));
            Text(label, r.x + (KEY_SIZE - TextWidth(label, 30)) / 2, r.y + 10, 30, BLACK);
        }

        if (game.IsOver()) {
            DrawRectangle(0, 380, screenWidth, screenHeight - 380, Fade(BLACK, 0.92f));
            if (game.IsWin()) TextCentered(t.win, 420, 60, GREEN);
            else TextCentered(t.lose, 420, 60, RED);
            TextCentered(TextFormat(t.wordWas, ToUtf8(game.Word()).c_str()), 500, 30, RAYWHITE);
            TextCentered(t.playAgain, 560, 24, LIGHTGRAY);
        } else {
            TextCentered(t.hint, 620, 20, GRAY);
        }

        EndDrawing();
    }

    if (font.texture.id != GetFontDefault().texture.id) UnloadFont(font);
    CloseWindow();
    return 0;
}
