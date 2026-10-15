# Виселица (Hangman) на raylib

Простая графическая игра «Виселица» на C++ с использованием библиотеки **raylib**.

## Как запустить

1. Установи [raylib](https://github.com/raysan5/raylib/releases) и компилятор [w64devkit](https://github.com/skeeto/w64devkit/releases).
2. Распакуй raylib (папка `raylib-6.0_win64_mingw-w64`) в `C:/raylib-6.0_win64_mingw-w64`.
3. Распакуй w64devkit в `C:/w64devkit`.
4. Добавь `C:/w64devkit/bin` в переменную среды Path.
5. Открой проект в VS Code.
6. Нажми `Ctrl + Shift + B` чтобы скомпилировать.
7. Запусти появившийся файл `hangman.exe`.

## Управление

- Вводи буквы с клавиатуры (русские или английские).
- У тебя есть 6 попыток.

## Структура проекта

- `main.cpp` — основной код игры
- `.vscode/tasks.json` — настройки сборки
- `.vscode/c_cpp_properties.json` — настройки IntelliSense

## Автор

Сделано для учёбы.
