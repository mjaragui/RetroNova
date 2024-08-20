#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

void setLED(int x, int y, bool state);
void drawChar(int x, int y, char c);
void drawTextOnMatrix(const char* str, int x, int y);
void drawVertTextOnMatrix(const char* text, int x, int y);
void drawColumn(int x, int y, const uint8_t* column);
void drawBorders();
void drawDigit(int x, int y, int digit);

#endif
