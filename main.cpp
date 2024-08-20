//main.cpp
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <UbidotsEsp32Mqtt.h>
#include "joystick.h"
#include "menu.h"
#include "tetris.h"
#include "space_invaders.h"
#include "Database.h"

#define PIN_CS 17 //Pin CS para comunicación SPI
#define H_DISPLAYS 4 //displays horizontales
#define V_DISPLAYS 4 //displays verticales

Max72xxPanel matrix = Max72xxPanel(PIN_CS, H_DISPLAYS, V_DISPLAYS); //Inicializar matriz
Joystick joystick; //Inicializar Joystick
GameSelection currentGame = NONE;

void setup() {
    Serial.begin(115200); //Inicializar comunicación serial

    // Configuarar posiciones de los displays
    for (int i = 0; i < V_DISPLAYS; i++) {
        for (int j = 0; j < H_DISPLAYS; j++) {
            matrix.setPosition(i + j * H_DISPLAYS, j, i);
        }
    }
    matrix.setRotation(2); // Asegura que la rotación sea la correcta
    matrix.setIntensity(1); //Ajustar brillo de la matriz
    matrix.fillScreen(LOW); // Limpiar la pantalla inicialmente
    matrix.write(); // Actualizar la pantalla

    Serial.println("Matriz LED inicializada");

    randomSeed(analogRead(14)); // Inicializar generador de números aleatorios
    joystick.begin(); // Inicializar joystick
    Serial.println("Joystick inicializado");

    showMenu(); // Mostrar menú inicial
    Serial.println("Menú inicial mostrado");

    databaseSetup();
}

void loop() {
    if (currentGame == NONE) {
        currentGame = handleMenuInput();
        if (currentGame == TETRIS) {
            setupTetris();
        } else if (currentGame == INVADERS) {
            setupSpaceInvaders();
        }
    } else if (currentGame == TETRIS) {
        loopTetris();
    } else if (currentGame == INVADERS) {
        loopSpaceInvaders();
    }
    databaseLoop();
}

