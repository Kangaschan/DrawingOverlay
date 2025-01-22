#include "colors.h"

// Определяем массив цветов
D3DCOLOR colorsPull[COLORS_AMOUNT] = {
    D3DCOLOR_XRGB(255, 255, 255), // Белый
    D3DCOLOR_XRGB(1, 1, 1),       // Черный
    D3DCOLOR_XRGB(128, 128, 128), // Серый

    D3DCOLOR_XRGB(255, 0, 0),     // Красный
    D3DCOLOR_XRGB(0, 255, 0),     // Зеленый
    D3DCOLOR_XRGB(0, 0, 255),     // Синий

    D3DCOLOR_XRGB(255, 255, 0),   // Желтый
    D3DCOLOR_XRGB(0, 255, 255),   // Голубой
    D3DCOLOR_XRGB(255, 0, 255),   // Пурпурный

    D3DCOLOR_XRGB(255, 165, 0),   // Оранжевый
    D3DCOLOR_XRGB(139, 69, 19),   // Коричневый
    D3DCOLOR_XRGB(255, 192, 203)  // Розовый
};

const D3DCOLOR eraser = D3DCOLOR_XRGB(0, 0, 0);

// Определяем массив с именами цветов
const char* colorNamesPull[COLORS_AMOUNT] = {
    "White",      // Белый
    "Black",      // Черный
    "Gray",       // Серый

    "Red",        // Красный
    "Green",      // Зеленый
    "Blue",       // Синий

    "Yellow",     // Желтый
    "Light blue", // Голубой
    "Magenta",    // Пурпурный

    "Orange",     // Оранжевый
    "Brown",      // Коричневый
    "Pink"        // Розовый
};
