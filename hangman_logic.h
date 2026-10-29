// Логика игры "Виселица" без зависимости от raylib.
// Вынесена отдельно, чтобы её можно было тестировать (tests.cpp).
// Слова и буквы хранятся как коды Unicode (char32_t), чтобы работала кириллица.
#pragma once

#include <string>
#include <vector>
#include <cstdlib>

enum class Language { English, Russian };

enum class GuessResult {
    Invalid,      // не буква (цифра, пробел, символ...)
    WrongLayout,  // буква другого языка (например, латиница в русской игре)
    AlreadyUsed,  // эту букву уже вводили
    Correct,      // буква есть в слове
    Wrong,        // буквы нет в слове
    GameOver      // игра уже закончена, ввод игнорируется
};

struct WordEntry {
    std::u32string word;
    std::string category; // UTF-8, для вывода на экран
};

class HangmanGame {
public:
    static const int MAX_ERRORS = 6;

    // Начинает новую игру. Возвращает false, если слово некорректное
    // (пустое или содержит что-то кроме букв выбранного языка).
    bool Start(const std::u32string& newWord, Language lang = Language::English) {
        if (newWord.empty()) return false;
        std::u32string w;
        for (char32_t c : newWord) {
            if (!IsLetter(c, lang)) return false;
            w += ToLower(c);
        }
        language = lang;
        word = w;
        hidden.assign(word.length(), U'_');
        used.clear();
        errors = 0;
        over = false;
        win = false;
        return true;
    }

    GuessResult Guess(int key) {
        if (over) return GuessResult::GameOver;
        if (key <= 0 || key > 0x10FFFF) return GuessResult::Invalid;

        char32_t c = (char32_t)key;
        if (!IsLetter(c, language)) {
            Language other = language == Language::English ? Language::Russian : Language::English;
            return IsLetter(c, other) ? GuessResult::WrongLayout : GuessResult::Invalid;
        }

        char32_t letter = ToLower(c);
        if (IsUsed(letter)) return GuessResult::AlreadyUsed;
        used += letter;

        bool found = false;
        for (size_t i = 0; i < word.length(); i++) {
            if (word[i] == letter) {
                hidden[i] = letter;
                found = true;
            }
        }

        if (!found) {
            errors++;
            if (errors >= MAX_ERRORS) {
                over = true;
                win = false;
            }
            return GuessResult::Wrong;
        }

        if (hidden == word) {
            over = true;
            win = true;
        }
        return GuessResult::Correct;
    }

    bool IsUsed(char32_t c) const { return used.find(ToLower(c)) != std::u32string::npos; }
    bool IsInWord(char32_t c) const { return word.find(ToLower(c)) != std::u32string::npos; }

    // Слово для показа в UTF-8 с пробелами: "п _ о _ _ а м"
    std::string Display() const { return Spaced(hidden); }
    std::string DisplayWord() const { return Spaced(word); }

    const std::u32string& Word() const { return word; }
    const std::u32string& Hidden() const { return hidden; }
    const std::u32string& Used() const { return used; }
    Language Lang() const { return language; }
    int Errors() const { return errors; }
    int ErrorsLeft() const { return MAX_ERRORS - errors; }
    bool IsOver() const { return over; }
    bool IsWin() const { return win; }

    static bool IsLatin(char32_t c) { return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z'); }
    // А-Я (0x410-0x42F), а-я (0x430-0x44F), Ё (0x401), ё (0x451)
    static bool IsCyrillic(char32_t c) { return (c >= 0x410 && c <= 0x44F) || c == 0x401 || c == 0x451; }

    static bool IsLetter(char32_t c, Language lang) {
        return lang == Language::English ? IsLatin(c) : IsCyrillic(c);
    }

    static char32_t ToLower(char32_t c) {
        if (c >= U'A' && c <= U'Z') return c - U'A' + U'a';
        if (c >= 0x410 && c <= 0x42F) return c + 0x20;
        if (c == 0x401) return 0x451;
        return c;
    }

    static char32_t ToUpper(char32_t c) {
        if (c >= U'a' && c <= U'z') return c - U'a' + U'A';
        if (c >= 0x430 && c <= 0x44F) return c - 0x20;
        if (c == 0x451) return 0x401;
        return c;
    }

    // Строчные буквы алфавита в обычном порядке (ё после е)
    static std::u32string Alphabet(Language lang) {
        if (lang == Language::English) return U"abcdefghijklmnopqrstuvwxyz";
        return U"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    }

private:
    static std::string Spaced(const std::u32string& s);

    Language language = Language::English;
    std::u32string word, hidden, used;
    int errors = 0;
    bool over = true;
    bool win = false;
};

// Перевод одного символа Unicode в UTF-8 (raylib рисует текст в UTF-8)
inline std::string ToUtf8(char32_t c) {
    std::string s;
    if (c < 0x80) {
        s += (char)c;
    } else if (c < 0x800) {
        s += (char)(0xC0 | (c >> 6));
        s += (char)(0x80 | (c & 0x3F));
    } else if (c < 0x10000) {
        s += (char)(0xE0 | (c >> 12));
        s += (char)(0x80 | ((c >> 6) & 0x3F));
        s += (char)(0x80 | (c & 0x3F));
    } else {
        s += (char)(0xF0 | (c >> 18));
        s += (char)(0x80 | ((c >> 12) & 0x3F));
        s += (char)(0x80 | ((c >> 6) & 0x3F));
        s += (char)(0x80 | (c & 0x3F));
    }
    return s;
}

inline std::string ToUtf8(const std::u32string& str) {
    std::string s;
    for (char32_t c : str) s += ToUtf8(c);
    return s;
}

inline std::string HangmanGame::Spaced(const std::u32string& s) {
    std::string out;
    for (size_t i = 0; i < s.length(); i++) {
        if (i > 0) out += ' ';
        out += ToUtf8(s[i]);
    }
    return out;
}

// Случайный индекс слова, отличный от предыдущего (если слов больше одного).
inline int PickWordIndex(int count, int previous) {
    if (count <= 0) return -1;
    if (count == 1) return 0;
    int idx;
    do {
        idx = rand() % count;
    } while (idx == previous);
    return idx;
}
