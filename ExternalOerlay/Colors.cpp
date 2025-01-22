#include "colors.h"

// ���������� ������ ������
D3DCOLOR colorsPull[COLORS_AMOUNT] = {
    D3DCOLOR_XRGB(255, 255, 255), // �����
    D3DCOLOR_XRGB(1, 1, 1),       // ������
    D3DCOLOR_XRGB(128, 128, 128), // �����

    D3DCOLOR_XRGB(255, 0, 0),     // �������
    D3DCOLOR_XRGB(0, 255, 0),     // �������
    D3DCOLOR_XRGB(0, 0, 255),     // �����

    D3DCOLOR_XRGB(255, 255, 0),   // ������
    D3DCOLOR_XRGB(0, 255, 255),   // �������
    D3DCOLOR_XRGB(255, 0, 255),   // ���������

    D3DCOLOR_XRGB(255, 165, 0),   // ���������
    D3DCOLOR_XRGB(139, 69, 19),   // ����������
    D3DCOLOR_XRGB(255, 192, 203)  // �������
};

const D3DCOLOR eraser = D3DCOLOR_XRGB(0, 0, 0);

// ���������� ������ � ������� ������
const char* colorNamesPull[COLORS_AMOUNT] = {
    "White",      // �����
    "Black",      // ������
    "Gray",       // �����

    "Red",        // �������
    "Green",      // �������
    "Blue",       // �����

    "Yellow",     // ������
    "Light blue", // �������
    "Magenta",    // ���������

    "Orange",     // ���������
    "Brown",      // ����������
    "Pink"        // �������
};
