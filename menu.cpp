#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include "joystick.h"
#include "menu.h"
#include "charact.h"
#include "utils.h"

extern Max72xxPanel matrix;
extern Joystick joystick;

GameSelection currentSelection = TETRIS;
const int borderThickness = 2;

void drawTextOnMatrix(const char* text, int x, int y);
void drawMenuBorder();
void drawArrow(int y);

void showMenu() {
    matrix.fillScreen(LOW);
    drawMenuBorder();
    drawTextOnMatrix("TETR", 4, 7); // Ajusta las coordenadas para centrar el texto
    drawTextOnMatrix("INVD", 4, 18); // Ajusta las coordenadas para centrar el texto
    matrix.write();
}

GameSelection handleMenuInput() {
    int yValue = joystick.readY();
    bool pressed = joystick.isPressed();

    if (yValue < 1000) {
        currentSelection = TETRIS;
        showMenu();
        drawArrow(11); // Flecha en "1: Tetris"
        matrix.write();
    } else if (yValue > 3000) {
        currentSelection = INVADERS;
        showMenu();
        drawArrow(21); // Flecha en "2: Invaders"
        matrix.write();
    }

    if (pressed) {
        delay(200); // Debounce delay
        return currentSelection;
    }

    return NONE;
}

void drawMenuBorder() {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < borderThickness; j++) {
            matrix.drawPixel(i, j, HIGH); // Superior
            matrix.drawPixel(i, 31 - j, HIGH); // Inferior
            matrix.drawPixel(j, i, HIGH); // Izquierda
            matrix.drawPixel(31 - j, i, HIGH); // Derecha
        }
    }
}

void drawArrow(int y) {
    // Dibuja la flecha
    matrix.drawPixel(4, y, HIGH); // Posición de la flecha ajustada
    matrix.drawPixel(3, y + 1, HIGH);
    matrix.drawPixel(3, y - 1, HIGH);
}
