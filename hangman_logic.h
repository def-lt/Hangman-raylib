// Логика игры "Виселица" без зависимости от raylib.
// Вынесена отдельно, чтобы её можно было тестировать (tests.cpp).
#pragma once

#include <string>
#include <vector>
#include <cstdlib>

enum class GuessResult {
    Invalid,      // не латинская буква (цифра, пробел, символ...)
    AlreadyUsed,  // эту букву уже вводили
    Correct,      // буква есть в слове
    Wrong,        // буквы нет в слове
    GameOver      // игра уже закончена, ввод игнорируется
};

struct WordEntry {
    std::string word;
    std::string category;
};

class HangmanGame {
public:
    static const int MAX_ERRORS = 6;

    // Начинает новую игру. Возвращает false, если слово некорректное
    // (пустое или содержит что-то кроме латинских букв).
    bool Start(const std::string& newWord) {
        if (newWord.empty()) return false;
        std::string w;
        for (char c : newWord) {
            if (!IsLatin(c)) return false;
            w += ToLower(c);
        }
        word = w;
        hidden.assign(word.length(), '_');
        used.clear();
        errors = 0;
        over = false;
        win = false;
        return true;
    }

    GuessResult Guess(int key) {
        if (over) return GuessResult::GameOver;
        if (key < 0 || key > 127 || !IsLatin((char)key)) return GuessResult::Invalid;

        char letter = ToLower((char)key);
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

    bool IsUsed(char c) const { return used.find(ToLower(c)) != std::string::npos; }
    bool IsInWord(char c) const { return word.find(ToLower(c)) != std::string::npos; }

    // Слово для показа: "p _ o _ _ a m"
    std::string Display() const {
        std::string s;
        for (size_t i = 0; i < hidden.length(); i++) {
            if (i > 0) s += ' ';
            s += hidden[i];
        }
        return s;
    }

    const std::string& Word() const { return word; }
    const std::string& Hidden() const { return hidden; }
    const std::string& Used() const { return used; }
    int Errors() const { return errors; }
    int ErrorsLeft() const { return MAX_ERRORS - errors; }
    bool IsOver() const { return over; }
    bool IsWin() const { return win; }

    static bool IsLatin(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
    static char ToLower(char c) { return (c >= 'A' && c <= 'Z') ? (char)(c - 'A' + 'a') : c; }

private:
    std::string word, hidden, used;
    int errors = 0;
    bool over = true;
    bool win = false;
};

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
