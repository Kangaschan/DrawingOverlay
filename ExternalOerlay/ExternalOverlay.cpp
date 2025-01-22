// ExternalOverlay.cpp : Defines the entry point for the application.
//
#pragma once
#include <Dwmapi.h>
#include <windows.h>
#include <vector>
#include "framework.h"
#include "ExternalOverlay.h"
#include "colors.h"
//#include "saveImageDialog.h"

#define TIMER_NO_DRAWING 1001
#define TIMER_YES_DRAWING 1002
#define TIMER_YES_SHOW 1003
#define TIMER_TIME 150

struct ThreadData {
    int delayMilliseconds;
    int threadIndex;
};


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR overlayWindowName[100] = L"Overlay";
HWND overlayHWND;
int width, height;
Paint paint;
int brushRadius = 5;
int threadInd = 1;
int colorInd = 0;
BOOL isShown = true;
BOOL isDrawing = false;
BOOL isDrawingOld = false;
BOOL isChoosingFile = false;
BOOL isEraser = false;
BOOL DrawFlagChange = false;
HANDLE hMutex;
// Forward declarations of functions included in this code module:
ATOM                registerClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
// delayed func (notification stop)
DWORD WINAPI DelayedFunction(LPVOID lpParameter) {
    ThreadData* data = (ThreadData*)lpParameter;
    

    Sleep(data->delayMilliseconds);  // �������� �� ��������� ���������� �����������
   WaitForSingleObject(hMutex, INFINITE);
   paint.stopMessage();
   ReleaseMutex(hMutex);
   delete data;
   InvalidateRect(overlayHWND, NULL, FALSE);
    return 0;
}
// start thred with deleyed task
void ExecuteWithDelay(int delayMilliseconds,int threadIndex) {
    ThreadData* data = new ThreadData{ delayMilliseconds, threadIndex };
    // ������ ����� ��� ���������� ������� � ���������
    CreateThread(
        NULL,                 // �������� ������������ (NULL � �� ���������)
        0,                    // ������ ����� (0 � �� ���������)
        DelayedFunction,      // ��������� �� ������� ������
        data,   // ��������, ������������ � ������� ������
        0,                    // ����� �������� (0 � ��������� �����)
        NULL                  // ��������� ��� ��������� �������������� ������ (NULL � �� ���������)
    );
}
//screen capture for saving
HBITMAP CaptureScreen(int x, int y, int width, int height) {
    HDC hScreenDC = GetDC(nullptr);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemoryDC, hBitmap);

    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);

    // ����������� �������
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);

    return hBitmap;
}


// win hooks start

HHOOK mouseHook;
HHOOK keyboardHook;
BOOL isMouseDrawing = false;
std::vector<HWND> targetWindows;

void Unhook() {
    if (mouseHook)
        UnhookWindowsHookEx(mouseHook);
    if (keyboardHook)
        UnhookWindowsHookEx(keyboardHook);
}
// ������� ��������� ������� �������
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    HBITMAP tmp;
    if (isDrawing) {
        static POINT lastPoint;
        if (nCode >= 0) {


            MSLLHOOKSTRUCT* mouseInfo = (MSLLHOOKSTRUCT*)lParam;
            // �������� ������� ����
            int mouseX = mouseInfo->pt.x;
            int mouseY = mouseInfo->pt.y;

            switch (wParam) {
            case WM_LBUTTONDOWN:
                if (!isChoosingFile) {
                    isMouseDrawing = true;
                    lastPoint.x = mouseX;
                    lastPoint.y = mouseY;
                    paint.addBrushStroke(lastPoint);
                }
                else
                {
                    if (mouseX > 500 && mouseX < 1060 && mouseY > 565 && mouseY < 615)//canvas
                    {
                        
                        isDrawing = isDrawingOld;
                        isChoosingFile = false;
                        paint.showSaveDialog(false);
                        paint.addMessage("Canvas saved");
                        ExecuteWithDelay(1500, threadInd++);
                        InvalidateRect(overlayHWND, NULL, TRUE);  // ������������� �������������� ����
                        UpdateWindow(overlayHWND);
                        paint.savePaintToFile("E:\\5Sem\\Canvas.png");
                    }
                    if (mouseX > 1060 && mouseX < 1460 && mouseY > 565 && mouseY < 615)//screenshot
                    {
                        isDrawing = isDrawingOld;
                        isChoosingFile = false;
                        paint.showSaveDialog(false);
                        paint.addMessage("Screenshot saved");
                        ExecuteWithDelay(1500, threadInd++);
                        InvalidateRect(overlayHWND, NULL, TRUE);  // ������������� �������������� ����
                        UpdateWindow(overlayHWND);                      
                        tmp = CaptureScreen(0, 0, width, height);
                        paint.saveScreenshotToFile(tmp, "E:\\5Sem\\ScreenshotWithCanvas.png");
                    }
                    return 1;
                }
                // ����� ������ ���� ������
                break;
            case WM_LBUTTONUP:
                if (isMouseDrawing) {
                    isMouseDrawing = false;
                    paint.resetLastPoint();

                    paint.endOfBrushStroke();
                }
                //����� ������ ���� ��������
                break;
            case WM_MOUSEMOVE:
                if (isMouseDrawing) {  // ����������� ����
                    POINT currentPoint;
                    currentPoint.x = mouseX;
                    currentPoint.y = mouseY;

                    paint.addBrushStroke(currentPoint);  // ��������� ����� ��� ����� ��� ��������
                    InvalidateRect(overlayHWND, NULL, FALSE);   // �������������� ����|

                }
                return CallNextHookEx(mouseHook, nCode, wParam, lParam);
                break;

            default:
                break;
            }
        }
        return 1;
    }
    else
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
    
}

// ������� ��������� ������������ �������
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    char buffer[100];
    
    if (nCode >= 0) {

        bool ctrlPressed = GetAsyncKeyState(VK_CONTROL) & 0x8000;
        bool shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;
        KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
        // ���������� ��� ������� �������
        DWORD vkCode = keyInfo->vkCode;

        switch (wParam) {
        case WM_KEYDOWN:
            if (ctrlPressed && shiftPressed) {
                switch (vkCode)
                {
                case 'X':
                    SendMessage(overlayHWND, WM_SETREDRAW, FALSE, 0);
                    Unhook();
                    PostQuitMessage(0);
                    return 1;
                    break;
                case 'D':

                    paint.clearCanvas();    
                    paint.addMessage("Canvas cleared");
                    ExecuteWithDelay(1500, threadInd++);
                    return 1;
                        
                    //������� �������������
                    break;
                case 'H':
                    //������
                    paint.changeShowHelp();
                    InvalidateRect(overlayHWND, NULL, TRUE);
                    return 1;
                    break;
                case 'I':
                    //������������
                    paint.addMessageBrushInfo(colorInd);
                    ExecuteWithDelay(1500, threadInd++);
                    return 1;
                    break;
                
                case VK_RIGHT://�����
                    colorInd = (colorInd + 1) % COLORS_AMOUNT;

                    paint.setBrushColor(colorInd);

                    paint.addMessage(colorNamesPull[colorInd]);
                    ExecuteWithDelay(1500, threadInd++);
                    return 1;
                    break;
                case VK_LEFT:
                    if (colorInd == 0)
                        colorInd = COLORS_AMOUNT;
                    colorInd--;
                    paint.setBrushColor(colorInd);
                    paint.addMessage(colorNamesPull[colorInd]);
                    ExecuteWithDelay(1500, threadInd++);
                    return 1;
                    break;
                case VK_UP://������
                    brushRadius += 3;
                    paint.setBrushRadius(brushRadius);
           
                    sprintf_s(buffer, "Brush radius changed to %d", brushRadius);
                    paint.addMessage(buffer);
                    ExecuteWithDelay(1500, threadInd++);
                    return 1;
                    break;
                case VK_DOWN:
                    brushRadius -= 3;
                    if (brushRadius < 1)
                        brushRadius = 2;
                    paint.setBrushRadius(brushRadius);
                    
                    sprintf_s(buffer, "Brush radius changed to %d", brushRadius);
                    paint.addMessage(buffer);
                    ExecuteWithDelay(1500, threadInd++);
                    return 1;
                    break;
                case 'Q':// �������� ��� ��� ������������
                    if (isShown)
                    {
                        isShown = false;
                        if (isDrawing)
                        {
                            isDrawing = false;
                            DrawFlagChange = true;
                        }
                        SendMessage(overlayHWND, WM_SETREDRAW, FALSE, 0);
                        return 1;
                    }
                    else 
                    {
                        paint.addMessage("Switched to show Drawing mode");
                        //SetTimer(overlayHWND, TIMER_YES_SHOW, TIMER_TIME, NULL);
                        ExecuteWithDelay(1500, threadInd++);
                        if (DrawFlagChange)
                        {

                            DrawFlagChange = false;
                            isDrawing = true;
                        }
                        isShown = true;
                        SendMessage(overlayHWND, WM_SETREDRAW, TRUE, 0);
                        InvalidateRect(overlayHWND, NULL, TRUE);  // ������������� �������������� ����
                        UpdateWindow(overlayHWND);
                        return 1;
                    }
                    break;
                case 'W'://����� ��������� ��/���
                    if (isDrawing)
                    {
                        paint.addMessage("Switched to no Drawing mode");
                        ExecuteWithDelay(1500, threadInd++);
                        isDrawing = false;
                        return 1;
                    }
                    else
                    {
                        paint.addMessage("Switched to Drawing mode");
                        ExecuteWithDelay(1500, threadInd++);
                        isDrawing = true;
                        return 1;
                    }
                    break;
                case 'E':
                    paint.changeEraser();
                    if (isEraser)
                    {
                        isEraser = false;
                        paint.addMessage("Switched brush mode");
                        return 1;

                    }
                    else
                    {
                        isEraser = true;
                        paint.addMessage("Switched eraser mode");
                        return 1;
                    }
                    ExecuteWithDelay(1500, threadInd++);
                    
                    break;
                case 'C':
                    paint.showSaveDialog(true);
                    isDrawingOld = isDrawing;
                    isChoosingFile = true;
                    InvalidateRect(overlayHWND, NULL, TRUE);  // ������������� �������������� ����
                    UpdateWindow(overlayHWND);
                    return 1;

                    break;
                default:
                    break;
                }
                
            }
            
            break;
        default:
            break;
        }
    }
    if (isDrawing)
        return 1;
    else
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
    
}

void SetHooks() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (!mouseHook)
        MessageBoxW(0,L"�� ����� ���������� mousehook",L"������",0);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!keyboardHook)
        MessageBoxW(0, L"�� ����� ���������� keyboardHook", L"������", 0);
}


// win hooks end


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    registerClass(hInstance);

     width = GetSystemMetrics(SM_CXSCREEN); // ������ ������
     height = GetSystemMetrics(SM_CYSCREEN); // ������ ������

     //������������� ����
     SetHooks();
     // ������� ������� ��� ������� � ������� paint 
     hMutex = CreateMutex(NULL, FALSE, NULL);

    // Perform application initialization:
    if (!InitInstance(hInstance, SW_SHOW)){
        return FALSE;
    }

    paint = Paint(overlayHWND,width,height);
    paint.setBrushRadius(brushRadius);

    paint.addMessage("Program started");
    ExecuteWithDelay(3000, threadInd++);
    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM registerClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = 0;
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground  = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName   = overlayWindowName;
    wcex.lpszClassName  = overlayWindowName;
    wcex.hIconSm        = 0;

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   overlayHWND = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, overlayWindowName, overlayWindowName, WS_POPUP,
      1, 1, width, height, nullptr, nullptr, hInstance, nullptr);

   if (!overlayHWND){
      return FALSE;
   }
   SetLayeredWindowAttributes(overlayHWND, RGB(0, 0, 0), 0, LWA_COLORKEY);

   ShowWindow(overlayHWND, nCmdShow);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
    case WM_PAINT:
        paint.render();
        break;
    case WM_DESTROY:
        Unhook();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}