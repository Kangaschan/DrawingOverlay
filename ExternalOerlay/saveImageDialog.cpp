#pragma once
#include "saveImageDialog.h"
int saveOption = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void ChooseSaveOption(HWND hwndParent) {
    
    
    // ����������� ������ ����
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"MyWindowClass";
    wc.hInstance = GetModuleHandle(nullptr);

    if (!RegisterClass(&wc)) {
        MessageBox(nullptr, L"Window class registration failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // ������� ����
    HWND hwnd = CreateWindow(
        L"STATIC", L"Window with Buttons", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, nullptr, nullptr, wc.hInstance, nullptr
    );

    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // ������� ������
    CreateWindowW(L"BUTTON", L"Button 1", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        50, 50, 80, 30, hwnd, (HMENU)1, nullptr, nullptr);
    CreateWindowW(L"BUTTON", L"Button 2", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 50, 80, 30, hwnd, (HMENU)2, nullptr, nullptr);

    // �������� ����
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // ������� ���� ��������� ���������
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_COMMAND) {
            // ��������� ������� ������
            switch (LOWORD(msg.wParam)) {
            case 1:
                
                saveOption = 1;
                PostQuitMessage(0); // ���������� ������������� ������ 1
                PostMessage(hwnd, WM_CLOSE, 0, 0); // ��������� ����, ���� ������ 1 ������
                break;
            case 2:
                
                saveOption = 2;
                PostQuitMessage(0); // ���������� ������������� ������ 2
                PostMessage(hwnd, WM_CLOSE, 0, 0); // ��������� ����, ���� ������ 2 ������
                break;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

   
}

// ���� ��������� (WinProc)
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