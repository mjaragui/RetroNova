//charact.h
#ifndef CHARACT_H
#define CHARACT_H

#include <stdint.h>

const uint8_t smallFont[][3] = {
    // Digits
    {0x1F, 0x11, 0x1F}, // 0
    {0x09, 0x1F, 0x01}, // 1
    {0x17, 0x15, 0x1D}, // 2
    {0x15, 0x15, 0x1F}, // 3
    {0x1C, 0x04, 0x1F}, // 4
    {0x1D, 0x15, 0x17}, // 5
    {0x1F, 0x15, 0x1D}, // 6
    {0x10, 0x13, 0x1C}, // 7
    {0x1F, 0x15, 0x1F}, // 8
    {0x1C, 0x14, 0x1F}, // 9
    // Uppercase letters
    {0x0F, 0x14, 0x0F}, // A
    {0x1F, 0x15, 0x1A}, // B
    {0x0E, 0x11, 0x11}, // C
    {0x1F, 0x11, 0x0E}, // D
    {0x1F, 0x15, 0x15}, // E
    {0x1F, 0x14, 0x10}, // F
    {0x0E, 0x11, 0x17}, // G
    {0x1F, 0x04, 0x1F}, // H
    {0x11, 0x1F, 0x11}, // I
    {0x02, 0x01, 0x1F}, // J
    {0x1F, 0x04, 0x1B}, // K
    {0x1F, 0x01, 0x01}, // L
    {0x1F, 0x08, 0x1F}, // M
    {0x0E, 0x04, 0x0E}, // N
    {0x0E, 0x11, 0x0E}, // O
    {0x1F, 0x14, 0x14}, // P
    {0x0E, 0x13, 0x0F}, // Q
    {0x1F, 0x16, 0x1D}, // R
    {0x09, 0x15, 0x12}, // S
    {0x10, 0x1F, 0x10}, // T
    {0x0F, 0x01, 0x0F}, // U
    {0x0E, 0x01, 0x0E}, // V
    {0x1F, 0x02, 0x1F}, // W
    {0x1B, 0x04, 0x1B}, // X
    {0x18, 0x07, 0x18}, // Y
    {0x13, 0x15, 0x19}, // Z

    {0x00, 0x00, 0x00}, // Space
};

#endif

