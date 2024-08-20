#ifndef NAVES_H
#define NAVES_H

#include <stdint.h>

const uint8_t NAVES[3][3]={
    //NAVES 20 PTS
    {
        0b00011000,
        0b00011000,
        0b00100100
    },
    //NAVES 10 PTS
    {
        0B00100100,
        0B00111100,
        0B00011000
    },
    //NAVE BOSS
    {
        0B00111100,
        0B01111110,
        0B11011011
    }
};

const uint8_t shield[1][3]={
    //ESCUDO
    {
        0b00011100,
        0b00111110,
        0b00100010
    }
};

#endif