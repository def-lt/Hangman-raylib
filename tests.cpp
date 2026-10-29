// Автотесты логики "Виселицы" (без raylib).
// Сборка: g++ -o tests.exe tests.cpp && tests.exe
// Номера тестов совпадают с таблицей в TESTING.md.
#include "hangman_logic.h"
#include <iostream>
#include <string>

using namespace std;

int passed = 0, failed = 0;

void Check(const string& id, const string& name, bool ok) {
    if (ok) passed++; else failed++;
    cout << (ok ? "[PASS] " : "[FAIL] ") << id << "  " << name << "\n";
}

// Вводит буквы строки по очереди
void GuessAll(HangmanGame& g, const string& letters) {
    for (char c : letters) g.Guess(c);
}

int main() {
    // ===================== ЧЁРНЫЙ ЯЩИК =====================
    // --- Начало игры ---
    {
        HangmanGame g; g.Start("cat");
        Check("T01", "New game: hidden word is all underscores", g.Hidden() == "___");
    }
    {
        HangmanGame g; g.Start("cat");
        Check("T02", "New game: 0 errors, game not over", g.Errors() == 0 && !g.IsOver() && g.Used().empty());
    }
    {
        HangmanGame g; g.Start("cat");
        Check("T03", "Display puts spaces between letters", g.Display() == "_ _ _");
    }
    // --- Классы эквивалентности ввода ---
    {
        HangmanGame g; g.Start("cat");
        GuessResult r = g.Guess('a');
        Check("T04", "Correct letter opens its position", r == GuessResult::Correct && g.Hidden() == "_a_");
    }
    {
        HangmanGame g; g.Start("cat");
        GuessResult r = g.Guess('z');
        Check("T05", "Wrong letter adds 1 error", r == GuessResult::Wrong && g.Errors() == 1);
    }
    {
        HangmanGame g; g.Start("banana");
        g.Guess('a');
        Check("T06", "Repeated letter in word opens all positions", g.Hidden() == "_a_a_a");
    }
    {
        HangmanGame g; g.Start("cat");
        GuessResult r = g.Guess('A');
        Check("T07", "Uppercase letter works like lowercase", r == GuessResult::Correct && g.Hidden() == "_a_");
    }
    {
        HangmanGame g; g.Start("cat");
        GuessResult r = g.Guess('5');
        Check("T08", "Digit is rejected, no error", r == GuessResult::Invalid && g.Errors() == 0);
    }
    {
        HangmanGame g; g.Start("cat");
        bool ok = g.Guess(' ') == GuessResult::Invalid && g.Guess('!') == GuessResult::Invalid
               && g.Guess('-') == GuessResult::Invalid;
        Check("T09", "Space and symbols are rejected", ok && g.Errors() == 0 && g.Used().empty());
    }
    {
        HangmanGame g; g.Start("cat");
        GuessResult r = g.Guess(0x0436); // 'ж' (кириллица, Unicode)
        Check("T10", "Cyrillic letter is rejected", r == GuessResult::Invalid && g.Errors() == 0);
    }
    // --- Повторный ввод ---
    {
        HangmanGame g; g.Start("cat");
        g.Guess('z');
        GuessResult r = g.Guess('z');
        Check("T11", "Same wrong letter twice = only 1 error", r == GuessResult::AlreadyUsed && g.Errors() == 1);
    }
    {
        HangmanGame g; g.Start("cat");
        g.Guess('a');
        GuessResult r = g.Guess('a');
        Check("T12", "Same correct letter twice -> AlreadyUsed", r == GuessResult::AlreadyUsed);
    }
    {
        HangmanGame g; g.Start("cat");
        g.Guess('z');
        GuessResult r = g.Guess('Z');
        Check("T13", "'z' then 'Z' is treated as repeat", r == GuessResult::AlreadyUsed && g.Errors() == 1);
    }
    // --- Граничные значения количества ошибок ---
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "zxwvu"); // 5 ошибок
        Check("T14", "5 errors (MAX-1): game continues", g.Errors() == 5 && !g.IsOver());
    }
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "zxwvuq"); // 6 ошибок
        Check("T15", "6 errors (MAX): game lost", g.Errors() == 6 && g.IsOver() && !g.IsWin());
    }
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "zxwvuq");
        GuessResult r = g.Guess('y');
        Check("T16", "After loss input ignored, errors stay 6", r == GuessResult::GameOver && g.Errors() == 6);
    }
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "zxwvu"); // 5 ошибок
        GuessAll(g, "cat");
        Check("T17", "Win with 5 errors (last chance)", g.IsOver() && g.IsWin() && g.Errors() == 5);
    }
    // --- Победа ---
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "cat");
        Check("T18", "All letters guessed -> win", g.IsOver() && g.IsWin() && g.Hidden() == "cat");
    }
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "cat");
        GuessResult r = g.Guess('z');
        Check("T19", "After win input ignored", r == GuessResult::GameOver && g.Errors() == 0);
    }
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "ca");
        Check("T20", "One letter left -> not win yet", !g.IsOver() && g.Hidden() == "ca_");
    }
    // --- Граничные значения длины слова ---
    {
        HangmanGame g; g.Start("a");
        g.Guess('a');
        Check("T21", "Word of 1 letter: win in one move", g.IsWin() && g.IsOver());
    }
    {
        HangmanGame g; g.Start("aaaa");
        g.Guess('a');
        Check("T22", "Word of one repeated letter: win in one move", g.IsWin());
    }
    {
        HangmanGame g;
        bool ok = g.Start("pneumonoultramicroscopicsilicovolcanoconiosis");
        Check("T23", "Very long word (45 letters) works", ok && g.Hidden().size() == 45);
    }
    {
        HangmanGame g;
        Check("T24", "Empty word is rejected", !g.Start(""));
    }
    {
        HangmanGame g;
        bool ok = !g.Start("ice cream") && !g.Start("abc1") && !g.Start("x-ray");
        Check("T25", "Word with space/digit/symbol is rejected", ok);
    }
    {
        HangmanGame g; g.Start("CaT");
        g.Guess('c');
        Check("T26", "Word in uppercase is normalized", g.Word() == "cat" && g.Hidden() == "c__");
    }
    // --- Новая игра ---
    {
        HangmanGame g; g.Start("cat");
        GuessAll(g, "zxwvuq");
        g.Start("dog");
        Check("T27", "Restart resets errors, used letters and state",
              g.Errors() == 0 && g.Used().empty() && !g.IsOver() && g.Hidden() == "___");
    }

    // ===================== БЕЛЫЙ ЯЩИК =====================
    {
        HangmanGame g; // Start() не вызывался
        Check("T28", "Before Start(): over=true, Guess -> GameOver", g.Guess('a') == GuessResult::GameOver);
    }
    {
        HangmanGame g; g.Start("cat");
        bool ok = g.Guess(-1) == GuessResult::Invalid && g.Guess(128) == GuessResult::Invalid
               && g.Guess('a' + 256) == GuessResult::Invalid; // char переполнился бы в 'a'
        Check("T29", "Codes <0 and >127 do not overflow into letters", ok && g.Used().empty());
    }
    {
        // Границы проверки IsLatin: символы до/после 'a','z','A','Z'
        bool ok = !HangmanGame::IsLatin('@') && HangmanGame::IsLatin('A') && HangmanGame::IsLatin('Z')
               && !HangmanGame::IsLatin('[') && !HangmanGame::IsLatin('`') && HangmanGame::IsLatin('a')
               && HangmanGame::IsLatin('z') && !HangmanGame::IsLatin('{');
        Check("T30", "IsLatin boundaries @ A Z [ ` a z {", ok);
    }
    {
        HangmanGame g; g.Start("cat");
        g.Guess('z'); g.Guess('a');
        Check("T31", "Used letters stored in input order", g.Used() == "za");
    }
    {
        bool ok = PickWordIndex(0, -1) == -1 && PickWordIndex(1, 0) == 0;
        Check("T32", "PickWordIndex: 0 words -> -1, 1 word -> 0", ok);
    }
    {
        srand(1);
        bool ok = true;
        int prev = 0;
        for (int i = 0; i < 1000; i++) {
            int idx = PickWordIndex(2, prev);
            if (idx == prev || idx < 0 || idx > 1) ok = false;
            prev = idx;
        }
        Check("T33", "PickWordIndex never repeats previous word", ok);
    }
    {
        HangmanGame g; g.Start("ab");
        GuessAll(g, "zxwvu"); // 5 ошибок
        g.Guess('a');
        GuessResult r = g.Guess('b');
        // ветка Correct + проверка победы после последней буквы
        Check("T34", "Branch: last correct letter sets win, not loss", r == GuessResult::Correct && g.IsWin());
    }
    {
        HangmanGame g; g.Start("cat");
        Check("T35", "ErrorsLeft = MAX - errors", g.ErrorsLeft() == 6 && (g.Guess('z'), g.ErrorsLeft() == 5));
    }

    cout << "\nPassed: " << passed << "  Failed: " << failed << "  Total: " << (passed + failed) << "\n";
    return failed == 0 ? 0 : 1;
}
