//joystick.h
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>



class Joystick {
public:
    Joystick(int pinX = 34, int pinY = 35, int pinButton = 32)
        : pinX(pinX), pinY(pinY), pinButton(pinButton) {}

    void begin() {
        pinMode(pinButton, INPUT_PULLUP);
    }

    int readX() {
        return analogRead(pinX);
    }

    int readY() {
        return analogRead(pinY);
    }

    bool isPressed() {
        return digitalRead(pinButton) == LOW;
    }

private:
    int pinX;
    int pinY;
    int pinButton;
};

#endif

