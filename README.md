# Виселица (Hangman) на raylib

Графическая игра «Виселица» на C++ с использованием библиотеки **raylib**.

## Как запустить

1. Установи [raylib](https://github.com/raysan5/raylib/releases) и компилятор [w64devkit](https://github.com/skeeto/w64devkit/releases).
2. Распакуй raylib (папка `raylib-6.0_win64_mingw-w64`) в `C:/raylib-6.0_win64_mingw-w64`.
3. Распакуй w64devkit в `C:/w64devkit`.
4. Добавь `C:/w64devkit/bin` в переменную среды Path.
5. Открой проект в VS Code.
6. Нажми `Ctrl + Shift + B` чтобы скомпилировать.
7. Запусти появившийся файл `hangman.exe`.

## Управление

- Вводи английские буквы с клавиатуры или кликай мышью по экранной клавиатуре.
- Угаданные буквы подсвечиваются зелёным, неверные — красным.
- У тебя 6 жизней. Над словом показана категория-подсказка.
- После конца игры — `R`, `Enter` или клик мышью для новой игры.

## Тесты

Логика игры вынесена в `hangman_logic.h` и проверяется 35 автотестами (чёрный и белый ящик):

```
g++ -o tests.exe tests.cpp
tests.exe
```

Тест-кейсы и теория тестирования описаны в [TESTING.md](TESTING.md).

## Структура проекта

- `main.cpp` — графика и управление (raylib)
- `hangman_logic.h` — логика игры (класс `HangmanGame`)
- `tests.cpp` — автотесты
- `TESTING.md` — теория и таблица тест-кейсов
- `.vscode/tasks.json` — настройки сборки
- `.vscode/c_cpp_properties.json` — настройки IntelliSense

## Автор

Сделано для учёбы.
