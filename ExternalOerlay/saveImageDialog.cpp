#pragma once
#include "saveImageDialog.h"
int saveOption = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void ChooseSaveOption(HWND hwndParent) {
    
    
    // Регистрация класса окна
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"MyWindowClass";
    wc.hInstance = GetModuleHandle(nullptr);

    if (!RegisterClass(&wc)) {
        MessageBox(nullptr, L"Window class registration failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Создаем окно
    HWND hwnd = CreateWindow(
        L"STATIC", L"Window with Buttons", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, nullptr, nullptr, wc.hInstance, nullptr
    );

    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Создаем кнопки
    CreateWindowW(L"BUTTON", L"Button 1", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        50, 50, 80, 30, hwnd, (HMENU)1, nullptr, nullptr);
    CreateWindowW(L"BUTTON", L"Button 2", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 50, 80, 30, hwnd, (HMENU)2, nullptr, nullptr);

    // Показать окно
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Главный цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_COMMAND) {
            // Обработка нажатия кнопок
            switch (LOWORD(msg.wParam)) {
            case 1:
                
                saveOption = 1;
                PostQuitMessage(0); // Возвращаем идентификатор кнопки 1
                PostMessage(hwnd, WM_CLOSE, 0, 0); // Закрываем окно, если кнопка 1 нажата
                break;
            case 2:
                
                saveOption = 2;
                PostQuitMessage(0); // Возвращаем идентификатор кнопки 2
                PostMessage(hwnd, WM_CLOSE, 0, 0); // Закрываем окно, если кнопка 2 нажата
                break;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

   
}

// Окно процессор (WinProc)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_COMMAND:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}