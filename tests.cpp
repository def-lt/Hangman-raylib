// Автотесты логики "Виселицы" (без raylib).
// Сборка: g++ -o tests.exe tests.cpp && tests.exe
// Номера тестов совпадают с таблицей в TESTING.md.
#include "hangman_logic.h"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

const Language EN = Language::English;
const Language RU = Language::Russian;

int passed = 0, failed = 0;

void Check(const string& id, const string& name, bool ok) {
    if (ok) passed++; else failed++;
    cout << (ok ? "[ПРОЙДЕН] " : "[ПРОВАЛ]  ") << id << "  " << name << "\n";
}

// Вводит буквы строки по очереди
void GuessAll(HangmanGame& g, const u32string& letters) {
    for (char32_t c : letters) g.Guess((int)c);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // чтобы русский текст в консоли не превращался в кракозябры
#endif

    cout << "===== ЧЁРНЫЙ ЯЩИК: английская игра =====\n";
    // --- Начало игры ---
    {
        HangmanGame g; g.Start(U"cat");
        Check("T01", "Новая игра: слово скрыто подчёркиваниями", g.Hidden() == U"___");
    }
    {
        HangmanGame g; g.Start(U"cat");
        Check("T02", "Новая игра: 0 ошибок, игра не окончена", g.Errors() == 0 && !g.IsOver() && g.Used().empty());
    }
    {
        HangmanGame g; g.Start(U"cat");
        Check("T03", "Между буквами на экране стоят пробелы", g.Display() == "_ _ _");
    }
    // --- Классы эквивалентности ввода ---
    {
        HangmanGame g; g.Start(U"cat");
        GuessResult r = g.Guess('a');
        Check("T04", "Верная буква открывается на своём месте", r == GuessResult::Correct && g.Hidden() == U"_a_");
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessResult r = g.Guess('z');
        Check("T05", "Неверная буква добавляет 1 ошибку", r == GuessResult::Wrong && g.Errors() == 1);
    }
    {
        HangmanGame g; g.Start(U"banana");
        g.Guess('a');
        Check("T06", "Повторяющаяся в слове буква открывается везде", g.Hidden() == U"_a_a_a");
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessResult r = g.Guess('A');
        Check("T07", "Заглавная буква работает как строчная", r == GuessResult::Correct && g.Hidden() == U"_a_");
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessResult r = g.Guess('5');
        Check("T08", "Цифра не принимается, ошибки нет", r == GuessResult::Invalid && g.Errors() == 0);
    }
    {
        HangmanGame g; g.Start(U"cat");
        bool ok = g.Guess(' ') == GuessResult::Invalid && g.Guess('!') == GuessResult::Invalid
               && g.Guess('-') == GuessResult::Invalid;
        Check("T09", "Пробел и знаки не принимаются", ok && g.Errors() == 0 && g.Used().empty());
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessResult r = g.Guess(U'ж');
        Check("T10", "Русская буква в английской игре: «не та раскладка»", r == GuessResult::WrongLayout && g.Errors() == 0);
    }
    // --- Повторный ввод ---
    {
        HangmanGame g; g.Start(U"cat");
        g.Guess('z');
        GuessResult r = g.Guess('z');
        Check("T11", "Неверная буква дважды = только 1 ошибка", r == GuessResult::AlreadyUsed && g.Errors() == 1);
    }
    {
        HangmanGame g; g.Start(U"cat");
        g.Guess('a');
        GuessResult r = g.Guess('a');
        Check("T12", "Верная буква дважды -> «уже была»", r == GuessResult::AlreadyUsed);
    }
    {
        HangmanGame g; g.Start(U"cat");
        g.Guess('z');
        GuessResult r = g.Guess('Z');
        Check("T13", "'z', потом 'Z' считается повтором", r == GuessResult::AlreadyUsed && g.Errors() == 1);
    }
    // --- Граничные значения количества ошибок ---
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"zxwvu"); // 5 ошибок
        Check("T14", "5 ошибок (граница - 1): игра продолжается", g.Errors() == 5 && !g.IsOver());
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"zxwvuq"); // 6 ошибок
        Check("T15", "6 ошибок (граница): проигрыш", g.Errors() == 6 && g.IsOver() && !g.IsWin());
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"zxwvuq");
        GuessResult r = g.Guess('y');
        Check("T16", "После проигрыша ввод игнорируется, ошибок остаётся 6", r == GuessResult::GameOver && g.Errors() == 6);
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"zxwvu"); // 5 ошибок
        GuessAll(g, U"cat");
        Check("T17", "Победа с 5 ошибками (на последней жизни)", g.IsOver() && g.IsWin() && g.Errors() == 5);
    }
    // --- Победа ---
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"cat");
        Check("T18", "Все буквы отгаданы -> победа", g.IsOver() && g.IsWin() && g.Hidden() == U"cat");
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"cat");
        GuessResult r = g.Guess('z');
        Check("T19", "После победы ввод игнорируется", r == GuessResult::GameOver && g.Errors() == 0);
    }
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"ca");
        Check("T20", "Осталась одна буква -> ещё не победа", !g.IsOver() && g.Hidden() == U"ca_");
    }
    // --- Граничные значения длины слова ---
    {
        HangmanGame g; g.Start(U"a");
        g.Guess('a');
        Check("T21", "Слово из 1 буквы: победа за 1 ход", g.IsWin() && g.IsOver());
    }
    {
        HangmanGame g; g.Start(U"aaaa");
        g.Guess('a');
        Check("T22", "Слово из одной повторяющейся буквы: победа за 1 ход", g.IsWin());
    }
    {
        HangmanGame g;
        bool ok = g.Start(U"pneumonoultramicroscopicsilicovolcanoconiosis");
        Check("T23", "Очень длинное слово (45 букв) работает", ok && g.Hidden().size() == 45);
    }
    {
        HangmanGame g;
        Check("T24", "Пустое слово не принимается", !g.Start(U""));
    }
    {
        HangmanGame g;
        bool ok = !g.Start(U"ice cream") && !g.Start(U"abc1") && !g.Start(U"x-ray");
        Check("T25", "Слово с пробелом/цифрой/дефисом не принимается", ok);
    }
    {
        HangmanGame g; g.Start(U"CaT");
        g.Guess('c');
        Check("T26", "Слово заглавными приводится к строчным", g.Word() == U"cat" && g.Hidden() == U"c__");
    }
    // --- Новая игра ---
    {
        HangmanGame g; g.Start(U"cat");
        GuessAll(g, U"zxwvuq");
        g.Start(U"dog");
        Check("T27", "Новая игра сбрасывает ошибки, буквы и статус",
              g.Errors() == 0 && g.Used().empty() && !g.IsOver() && g.Hidden() == U"___");
    }

    cout << "\n===== ЧЁРНЫЙ ЯЩИК: русская игра =====\n";
    {
        HangmanGame g; g.Start(U"молоко", RU);
        GuessResult r = g.Guess(U'о');
        Check("T28", "Русская буква открывается во всех местах", r == GuessResult::Correct && g.Hidden() == U"_о_о_о");
    }
    {
        HangmanGame g; g.Start(U"кот", RU);
        GuessResult r = g.Guess(U'К');
        Check("T29", "Заглавная русская 'К' работает как 'к'", r == GuessResult::Correct && g.Hidden() == U"к__");
    }
    {
        HangmanGame g; g.Start(U"кот", RU);
        GuessResult r = g.Guess('k');
        Check("T30", "Латинская буква в русской игре: «не та раскладка», без ошибки",
              r == GuessResult::WrongLayout && g.Errors() == 0);
    }
    {
        HangmanGame g; g.Start(U"ёжик", RU);
        GuessResult r1 = g.Guess(U'е');
        GuessResult r2 = g.Guess(U'ё');
        Check("T31", "'е' и 'ё' - разные буквы", r1 == GuessResult::Wrong && r2 == GuessResult::Correct);
    }
    {
        HangmanGame g; g.Start(U"ёжик", RU);
        GuessResult r = g.Guess(U'Ё');
        Check("T32", "Заглавная 'Ё' работает как 'ё'", r == GuessResult::Correct && g.Hidden() == U"ё___");
    }
    {
        HangmanGame g; g.Start(U"кот", RU);
        GuessAll(g, U"абвгдё");
        Check("T33", "Русская игра: 6 ошибок -> проигрыш", g.IsOver() && !g.IsWin() && g.Errors() == 6);
    }
    {
        HangmanGame g; g.Start(U"кот", RU);
        GuessAll(g, U"кот");
        Check("T34", "Русская игра: все буквы отгаданы -> победа", g.IsOver() && g.IsWin());
    }
    {
        HangmanGame g;
        bool ok = !g.Start(U"cat", RU) && !g.Start(U"кот", EN) && !g.Start(U"кoт", RU); // во 2-й букве латинская o
        Check("T35", "Слово не того языка (или смесь) не принимается", ok);
    }
    {
        HangmanGame g; g.Start(U"кот", RU);
        g.Guess(U'т');
        Check("T36", "Русское слово на экране в UTF-8: \"_ _ т\"", g.Display() == "_ _ т");
    }
    {
        HangmanGame g; g.Start(U"Москва", RU);
        Check("T37", "Русское слово с заглавной приводится к строчным", g.Word() == U"москва");
    }

    cout << "\n===== БЕЛЫЙ ЯЩИК =====\n";
    {
        HangmanGame g; // Start() не вызывался
        Check("T38", "До Start(): over=true, Guess -> «игра окончена»", g.Guess('a') == GuessResult::GameOver);
    }
    {
        HangmanGame g; g.Start(U"cat");
        bool ok = g.Guess(0) == GuessResult::Invalid && g.Guess(-1) == GuessResult::Invalid
               && g.Guess(0x110000) == GuessResult::Invalid && g.Guess('a' + 0x10000) == GuessResult::Invalid;
        Check("T39", "Коды 0, <0 и >0x10FFFF отклоняются и не превращаются в буквы", ok && g.Used().empty());
    }
    {
        // Границы проверки IsLatin: символы до/после 'a','z','A','Z'
        bool ok = !HangmanGame::IsLatin('@') && HangmanGame::IsLatin('A') && HangmanGame::IsLatin('Z')
               && !HangmanGame::IsLatin('[') && !HangmanGame::IsLatin('`') && HangmanGame::IsLatin('a')
               && HangmanGame::IsLatin('z') && !HangmanGame::IsLatin('{');
        Check("T40", "Границы IsLatin: @ A Z [ ` a z {", ok);
    }
    {
        // Границы IsCyrillic: 0x40F (Џ) и 0x450 (ѐ) - уже не русские буквы
        bool ok = !HangmanGame::IsCyrillic(0x40F) && HangmanGame::IsCyrillic(U'А') && HangmanGame::IsCyrillic(U'Я')
               && HangmanGame::IsCyrillic(U'а') && HangmanGame::IsCyrillic(U'я') && !HangmanGame::IsCyrillic(0x450)
               && HangmanGame::IsCyrillic(U'Ё') && HangmanGame::IsCyrillic(U'ё') && !HangmanGame::IsCyrillic(0x452);
        Check("T41", "Границы IsCyrillic: Џ А Я а я ѐ Ё ё ђ", ok);
    }
    {
        // Все ветки ToLower/ToUpper
        bool ok = HangmanGame::ToLower('Q') == 'q' && HangmanGame::ToLower(U'Я') == U'я'
               && HangmanGame::ToLower(U'Ё') == U'ё' && HangmanGame::ToLower('5') == '5'
               && HangmanGame::ToUpper('q') == 'Q' && HangmanGame::ToUpper(U'я') == U'Я'
               && HangmanGame::ToUpper(U'ё') == U'Ё' && HangmanGame::ToUpper('5') == '5';
        Check("T42", "ToLower/ToUpper: латиница, кириллица, Ё, не буква", ok);
    }
    {
        // Все 4 ветки ToUtf8: 1, 2, 3 и 4 байта
        bool ok = ToUtf8(U'a') == "a" && ToUtf8(U'ж') == "\xD0\xB6"
               && ToUtf8(U'€') == "\xE2\x82\xAC" && ToUtf8((char32_t)0x1F600) == "\xF0\x9F\x98\x80";
        Check("T43", "ToUtf8: символы из 1, 2, 3 и 4 байт", ok);
    }
    {
        u32string en = HangmanGame::Alphabet(EN), ru = HangmanGame::Alphabet(RU);
        bool ok = en.size() == 26 && ru.size() == 33 && ru.find(U'ё') == 6;
        Check("T44", "Алфавиты: EN 26 букв, RU 33 буквы, 'ё' после 'е'", ok);
    }
    {
        HangmanGame g; g.Start(U"cat");
        g.Guess('z'); g.Guess('a');
        Check("T45", "Использованные буквы хранятся в порядке ввода", g.Used() == U"za");
    }
    {
        bool ok = PickWordIndex(0, -1) == -1 && PickWordIndex(1, 0) == 0;
        Check("T46", "PickWordIndex: 0 слов -> -1, 1 слово -> 0", ok);
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
        Check("T47", "PickWordIndex никогда не повторяет предыдущее слово", ok);
    }
    {
        HangmanGame g; g.Start(U"ab");
        GuessAll(g, U"zxwvu"); // 5 ошибок
        g.Guess('a');
        GuessResult r = g.Guess('b');
        // ветка Correct + проверка победы после последней буквы
        Check("T48", "Ветка: последняя верная буква даёт победу, а не проигрыш", r == GuessResult::Correct && g.IsWin());
    }
    {
        HangmanGame g; g.Start(U"cat");
        int before = g.ErrorsLeft();
        g.Guess('z');
        Check("T49", "ErrorsLeft = MAX_ERRORS - errors", before == 6 && g.ErrorsLeft() == 5);
    }

    cout << "\nПройдено: " << passed << "  Провалено: " << failed << "  Всего: " << (passed + failed) << "\n";
    return failed == 0 ? 0 : 1;
}
