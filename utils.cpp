#include "utils.h"
#include "Max72xxPanel.h"
#include "charact.h"

extern Max72xxPanel matrix;

void setLED(int x, int y, bool state) {
    matrix.drawPixel(x, y, state);
}

void drawColumn(int x, int y, const uint8_t* column) {
    for (int i = 0; i < 5; i++) {
        setLED(x, y + i, column[i]);
    }
}

void drawBorders() {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 8; j++) {
            matrix.drawPixel(j, i, HIGH); // Borde izquierdo
            matrix.drawPixel(31 - j, i, HIGH); // Borde derecho
        }
    }
}

void drawChar(int x, int y, char c) { //utils.cpp
    int index;
    if (c >= '0' && c <= '9') {
        index = c - '0';
    } else if (c >= 'A' && c <= 'Z') {
        index = c - 'A' + 10;
    } else if (c >= 'a' && c <= 'z') {
        index = c - 'a' + 36;
    } else if (c == '!') {
        index = 62;
    } else {
        index = 63;
    }

    for (int j = 0; j < 3; j++) {
        drawColumn(x + j, y, smallFont[index] + j );
    }
}

void drawDigit(int x, int y, int digit){
    for (int i = 0; i < 3; i++) {
        drawColumn(x + i, y, smallFont[digit] + i );
    }
}

void drawTextOnMatrix(const char* text, int x, int y) {
    while (*text) {
        matrix.drawChar(x, y, *text, HIGH, LOW, 1);
        x += 6; // Ajusta el espacio entre caracteres
        text++;
    }
}

void drawVertTextOnMatrix(const char* text, int x, int y) {
    while (*text) {
        matrix.drawChar(x, y, *text, HIGH, LOW, 1);
        y += 8; // Ajusta el espacio entre caracteres
        text++;
    }
}

void loopText(){
    
}