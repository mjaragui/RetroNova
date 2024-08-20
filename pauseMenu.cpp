#include <Arduino.h>
#include "pauseMenu.h"
#include "charact.h"
#include "config.h"
#include "utils.h"
#include <Max72xxPanel.h>
#include "joystick.h"
#include "tetris.h"
#include "space_invaders.h"
#include "menu.h"

extern Max72xxPanel matrix;
extern Joystick joystick;

const char* pauseOptions[] = {"CONT", "REIN", "MENU"};
int selectedOption = 0;
bool gamePaused = false;

void initPauseMenu() {
    pinMode(PAUSE_BUTTON_PIN, INPUT_PULLUP);
}

void handlePauseMenu() {
    if (!gamePaused) return;    // El botón de pausa solo funciona cuando el juego está corriendo

    while (true){
        int yValue = joystick.readY();    
        if (joystick.isPressed()) {
            while(joystick.isPressed()){delay(30);}
            switch (selectedOption) {
                case 0:
                    // Continuar
                    gamePaused = false;
                    delay(300);
                    return;
                case 1:
                    // Reiniciar el juego
                    restartGame();
                    setupTetris();
                    gamePaused = false;
                    delay(300);
                    return;
                case 2:
                    // Volver al menú principal
                    goToMainMenu();
                    gamePaused = false;
                    delay(300);
                    return;
            }
            
        }

        if (yValue < 1) {
            selectedOption = (selectedOption - 1 + 3) % 3;
            delay(300);
        } else if (yValue > 4094) {
            selectedOption = (selectedOption + 1) % 3;
            delay(300);
        }

        drawPauseMenu();
        delay(50);
    }
}

void drawArrowPause(int y) {
    // Dibuja la flecha
    matrix.drawPixel(4, y, HIGH); // Posición de la flecha ajustada
    matrix.drawPixel(3, y + 1, HIGH);
    matrix.drawPixel(3, y - 1, HIGH);
}

void drawPauseMenu() {
    matrix.fillScreen(LOW);

    // Dibujar el borde de j leds de grosor
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 2; j++) {
            matrix.drawPixel(j, i, HIGH); // Borde izquierdo
            matrix.drawPixel(31 - j, i, HIGH); // Borde derecho
            matrix.drawPixel(i, j, HIGH); // Borde superior
            matrix.drawPixel(i, 31 - j, HIGH); // Borde inferior
        }
    }

    // Dibujar las opciones del menú de pausa
    for (int i = 0; i < 3; i++) {
        int x = 6;
        int y = 4 + i * 8;
        drawTextOnMatrix(pauseOptions[i], x, y); // Aquí se dibujan "CONTINUAR", "REINICIAR" y "MENU P"
    }

    // Dibujar la flecha para la opción seleccionada
    drawArrowPause(7 + 8 * selectedOption);
    matrix.write();
}

void handlePauseButton() {
    if (!gamePaused) {
        gamePaused = true; // Pausar el juego y mostrar el menú de pausa
    }
}

void restartGame() {
    // Reinicia el juego actual basado en la selección actual
    if (currentGame == TETRIS) {
        setupTetris(); // Re-inicializa el juego de Tetris
    } else if (currentGame == INVADERS) {
        setupSpaceInvaders(); // Re-inicializa el juego de Invaders
    }
}

void goToMainMenu() {
    currentGame = NONE; // Marca que no hay juego actual en curso
    showMenu(); // Llama a la función que muestra el menú principal
}

