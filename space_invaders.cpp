#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <UbidotsEsp32Mqtt.h>
#include "Database.h"
#include "space_invaders.h"
#include "joystick.h"
#include "pauseMenu.h"
#include "config.h"
#include "charact.h"
#include "Sound.h"
#include "utils.h"
#include "menu.h"
#include "naves.h"
#include "saveMenu.h"
#include "Database.h"

extern Max72xxPanel matrix;
extern Joystick joystick;

const int bordeIzquierdo = 0;
const int bordeDerecho = 0;

const int playerStartX = 16;
const int playerY = 28;
int playerX = playerStartX;
int scoreInv = 0;

// Variables para el disparo del jugador
const int maxShots = 3; // Máximo número de disparos simultáneos permitidos
bool shotActive[maxShots] = {false};
int shotX[maxShots];
int shotY[maxShots];
unsigned long lastShotTime = 0;

const int numRows = 3; // Número de filas de naves
const int numCols = 4; // Número de naves por fila
int invaderX[numRows][numCols];
int invaderY[numRows][numCols];
bool invaderDirection = true; // true = derecha, false = izquierda
bool invaderBorder = false;
unsigned long lastInvaderMoveTime = 0; // Para controlar la velocidad de movimiento de las naves
const unsigned long minInvaderMoveDelay = 50; // Retardo mínimo permitido para evitar que el juego sea demasiado rápido
unsigned long invaderMoveDelay = 400; // Retraso inicial entre movimientos de las naves (en ms)
const int shotProbability = 80;
int speedIncrease = 50; // Decremento en el retardo para cada nueva horda


// Variables para los disparos de las naves
bool invaderShotActive[numCols] = {false};
int invaderShotX[numCols], invaderShotY[numCols];
unsigned long lastInvaderShotTime = 0;
bool allInvadersDestroyed = true; // Indica si todas las naves del grupo actual han sido destruidas

void setupSpaceInvaders();
void handleInputInv();
void updateInvaders();
void handlePauseMenu();
void handlePauseButton();
void drawGameInv();
void drawScoreInv();
void updateShot(); // función para actualizar el disparo del jugador
void handleShot(); // función para manejar el disparo del jugador
void updateInvaderShot(); // función para actualizar el disparo de las naves
void spawnInvaders();
void MuteSound();
void playSpaceInvadersMelody();
void playPlayerDeathSound();
bool checkCollisionInv(int x, int y);
void drawNave(int tipo, int x, int y);
void drawPlayer(int x, int y);
bool checkCollisionWithNave(int shotX, int shotY, int invaderX, int invaderY, int naveWidth, int naveHeight); 
bool checkCollisionWithPlayer(int invaderShotX, int invaderShotY, int playerX, int playerY, int playerWidth, int playerHeight);
void endScreenInv();
void goToMainMenu();
int getTopInvaderInColumn(int col);
void drawSaveMenu();
void handleSaveMenu(int score);
void resetShot();
void resetInvaderShot();

void setupSpaceInvaders() {     
    randomSeed(analogRead(14));
    scoreInv = 0;
    invaderMoveDelay = 400;
    resetShot();
    resetInvaderShot();
    playerX = playerStartX;
    spawnInvaders();
    initPauseMenu(); // Inicializa el menú de pausa
    setupSound();

}

void loopSpaceInvaders() {
    if (digitalRead(PAUSE_BUTTON_PIN) == LOW) {
        MuteSound(); 
        handlePauseButton();
        handlePauseMenu(); // Maneja el menú de pausa
    } else {
        playSpaceInvadersMelody();
        drawGameInv();
        handleInputInv();
        handleShot(); 
        updateInvaders();
        updateShot();
        updateInvaderShot(); // Actualiza el disparo de las naves
        delay(100); // Disminuye el delay para hacer que las naves se muevan más lento
    }
}

void handleInputInv() {
    int xValue = joystick.readX();

    if (xValue < 1000) {
        playerX--;
        if (playerX < bordeIzquierdo + 1) playerX = bordeIzquierdo + 1; // Limita el movimiento a la izquierda dentro del área de juego
    } else if (xValue > 3000) {
        playerX++;
        if (playerX > 30-bordeDerecho) playerX = 30-bordeDerecho; // Limita el movimiento a la derecha dentro del área de juego
    }
}

void handleShot() {
    if (joystick.isPressed()) {
        for (int i = 0; i < maxShots; i++) {
            if (!shotActive[i]) {
                shotActive[i] = true;
                shotX[i] = playerX;
                shotY[i] = playerY - 1; // El disparo se inicia justo encima del jugador
                //playPlayerBulletSound();
                break; // Sal del bucle después de activar un disparo
            }
        }
    }
}

void resetShot(){
    for (int i = 0; i < maxShots; i++){
        shotActive[i] = false;
    }
}

bool checkCollisionWithNave(int shotX, int shotY, int invaderX, int invaderY, int naveWidth, int naveHeight) {
    // Verifica si el disparo está dentro de los límites de la nave
    return (shotX >= invaderX + 2 && shotX < invaderX + 2 + naveWidth &&
            shotY >= invaderY && shotY < invaderY + naveHeight);
}

bool checkCollisionWithPlayer(int invaderShotX, int invaderShotY, int playerX, int playerY) {
    // Verifica la colisión con la parte superior del jugador (un solo LED)
    if (invaderShotX == playerX && invaderShotY == playerY) {
        playPlayerDeathSound();
        MuteSound();
        return true;
    }
    
    // Verifica la colisión con la parte inferior del jugador (3 LEDs de ancho)
    if (invaderShotY == playerY + 1 && (invaderShotX >= playerX - 1 && invaderShotX <= playerX + 1)) {
        playPlayerDeathSound();
        MuteSound();
        return true;
    }

    // Si no hubo colisión
    return false;
}

void updateShot() {
    for (int i = 0; i < maxShots; i++) {
        if (shotActive[i]) {
            shotY[i]--; // El disparo se mueve hacia arriba

            // Verifica colisiones con los invasores
            for (int row = 0; row < numRows; row++) {
                for (int col = 0; col < numCols; col++) {
                    if (invaderX[row][col] != -1 && invaderY[row][col] != -1) {
                        //Ajuste según el tamaño de las naves
                        int naveWidth = 5;
                        int naveHeight = 3;

                        if (checkCollisionWithNave(shotX[i], shotY[i], invaderX[row][col], invaderY[row][col], naveWidth, naveHeight)) {
                            // Actualiza el puntaje según la fila de la nave
                            scoreInv += (row == 0) ? 30 : (row == 1) ? 20 : 10;

                            // Marca la nave como destruida
                            invaderX[row][col] = -1;
                            invaderY[row][col] = -1;
                            // Desactiva el disparo
                            shotActive[i] = false;
                            break;
                        }
                    }
                }
            }

            if (shotY[i] < 0) {
                shotActive[i] = false; // Desactiva el disparo si sale de la pantalla
            }
        }
    }
}

void updateInvaderShot() {
    for (int j = 0; j < numCols; j++) {
        if (invaderShotActive[j]) {
            invaderShotY[j]++; // El disparo se mueve hacia abajo

            // Verifica colisión con el jugador en todo su ancho
            if (checkCollisionWithPlayer(invaderShotX[j], invaderShotY[j], playerX, playerY )) {
                endScreenInv(); // El jugador pierde si es impactado
                return;
            }

            if (invaderShotY[j] >= 32) {
                invaderShotActive[j] = false; // Desactiva el disparo si sale de la pantalla
            }
        } else if (random(0, 100) < shotProbability) { 
            // Posibilidad de disparo aleatorio
            int rowToShoot = getTopInvaderInColumn(j); 
            if (rowToShoot != -1) {
                invaderShotX[j] = invaderX[rowToShoot][j] + 4;
                invaderShotY[j] = invaderY[rowToShoot][j] + 1;
                invaderShotActive[j] = true;
            }
        }
    }

    // Verifica si alguna nave ha pasado el límite inferior máximo para ser destruida
    const int limitY = 26;

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (invaderY[i][j] >= limitY && invaderX[i][j] >= 0) {
                endScreenInv(); // Llama a la función para mostrar "Game Over"
                return;
            }
        }
    }
}

void resetInvaderShot (){
    for (int i = 0; i < numCols; i++){
        invaderShotActive[i] = false;
    }
}

int getTopInvaderInColumn(int col) {
    // Recorre las filas desde la más alta (cerca del jugador) hasta la más baja (cerca de la parte superior de la pantalla)
    for (int row = numRows - 1; row >= 0; row--) {
        if (invaderX[row][col] != -1 && invaderY[row][col] != -1) {
            return row; // Devuelve la fila de la nave superior encontrada en la columna
        }
    }
    return -1; // Si no hay naves en la columna, devuelve -1
}

void updateInvaders() {
    if (allInvadersDestroyed) {
        spawnInvaders(); // Aparece un nuevo grupo de naves
        allInvadersDestroyed = false; // Resetea la bandera
    }

    if (millis() - lastInvaderMoveTime >= invaderMoveDelay) {
        lastInvaderMoveTime = millis(); // Actualiza el tiempo del último movimiento

        // Determina los límites izquierdo y derecho del conjunto de naves
        int leftMostInvader = 31; // Inicializa con un valor mayor al máximo
        int rightMostInvader = 0; // Inicializa con un valor menor al mínimo

        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (invaderX[i][j] != -1 && invaderY[i][j] != -1) {
                    if (invaderX[i][j] < leftMostInvader) {
                        leftMostInvader = invaderX[i][j];
                    }
                    if (invaderX[i][j] > rightMostInvader) {
                        rightMostInvader = invaderX[i][j];
                    }
                }
            }
        }

        // Mover las naves según la dirección
        if (invaderDirection) {
            if (rightMostInvader < 24 - bordeDerecho) {
                for (int i = 0; i < numRows; i++) {
                    for (int j = 0; j < numCols; j++) {
                        if (invaderX[i][j] != -1 && invaderY[i][j] != -1) {
                            invaderX[i][j]++;
                        }
                    }
                }
            } else {
                invaderDirection = false;
                for (int i = 0; i < numRows; i++) {
                    for (int j = 0; j < numCols; j++) {
                        if (invaderX[i][j] != -1 && invaderY[i][j] != -1) {
                            invaderY[i][j]++;
                        }
                    }
                }
            }
        } else {
            if (leftMostInvader > bordeIzquierdo) {
                for (int i = 0; i < numRows; i++) {
                    for (int j = 0; j < numCols; j++) {
                        if (invaderX[i][j] != -1 && invaderY[i][j] != -1) {
                            invaderX[i][j]--;
                        }
                    }
                }
            } else {
                invaderDirection = true;
                for (int i = 0; i < numRows; i++) {
                    for (int j = 0; j < numCols; j++) {
                        if (invaderX[i][j] != -1 && invaderY[i][j] != -1) {
                            invaderY[i][j]++;
                        }
                    }
                }
            }
        }

        // Verifica si todas las naves han sido destruidas
        allInvadersDestroyed = true; // Asume que todas están destruidas
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (invaderX[i][j] != -1 && invaderY[i][j] != -1) {
                    allInvadersDestroyed = false; // Si una nave está presente, no están todas destruidas
                    break;
                }
            }
            if (!allInvadersDestroyed) break; // Sal del bucle si se encontró una nave
        }
    }
}

void drawGameInv() {
    matrix.fillScreen(LOW);

    drawPlayer(playerX, playerY); // Dibuja el jugador

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if (invaderX[i][j] != -1 && invaderY[i][j] != -1) { // Dibuja solo las naves que no están destruidas
                drawNave((i < 2) ? 0 : 1, invaderX[i][j], invaderY[i][j]); // Tipo de nave según la fila
            }
        }
    }
    
    for (int i = 0; i< maxShots; i++){
        if (shotActive[i]) {
            matrix.drawPixel(shotX[i], shotY[i], HIGH); // Dibuja el disparo del jugador
        }
    }

    for (int j = 0; j < numCols; j++)
    {
        if (invaderShotActive[j])
        {
            matrix.drawPixel(invaderShotX[j], invaderShotY[j], HIGH); // Dibuja el disparo de la nave
        }
    }
    matrix.write();
}

void drawScoreInv() {
    char scoreStr[10];
    sprintf(scoreStr, "%d", scoreInv); // Convierte el puntaje a cadena de texto

    drawTextOnMatrix(scoreStr, 25, 2); // Dibuja el puntaje en el área correspondiente (por ejemplo, en el borde derecho)
}

void spawnInvaders() {
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            invaderX[i][j] = 8 + j * 5; // Posiciones iniciales ajustadas
            invaderY[i][j] = i * 4;     // Posiciones iniciales ajustadas
        }
    }
    // Aumentar la velocidad de la nueva horda
    if (invaderMoveDelay > minInvaderMoveDelay) {
        invaderMoveDelay -= speedIncrease;
    }
}

bool checkCollisionInv(int x, int y) {
    if (abs(x - playerX) <= 1 && abs(y - playerY) <= 1) {
        return true;
    }
    return false;
}

void drawNave(int tipo, int x, int y) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            if (NAVES[tipo][i] & (1 << j)) {
                matrix.drawPixel(x + j, y + i, HIGH);
            }
        }
    }
}

void drawPlayer(int x, int y) {
    matrix.drawPixel(x, y, HIGH);
    matrix.drawPixel(x - 1, y + 1, HIGH);
    matrix.drawPixel(x + 1, y + 1, HIGH);
    matrix.drawPixel(x, y + 1, HIGH);
}

void endScreenInv() {
    // Mostrar "GAME OVER"
    matrix.fillScreen(LOW);
    drawTextOnMatrix("GAME", 4, 8);
    drawTextOnMatrix("OVER", 4, 16);
    matrix.write();
    delay(2000);
    matrix.fillScreen(LOW);
    bool loseInvaders = true;
    while(loseInvaders){
        delay(100);
        matrix.fillScreen(LOW);
        drawTextOnMatrix("SCORE",2,8);
        char scoreStr[10];
        sprintf(scoreStr, "%d", scoreInv); // Convierte el puntaje a cadena de texto
        drawTextOnMatrix(scoreStr, 8, 16); // Dibuja el puntaje 
        matrix.write();
        delay(2000);

        handleSaveMenu(scoreInv);
        loseInvaders = false;
        MuteSound();
        currentGame = NONE;
        showMenu();
    }
}

    // // Mostrar "SCORE" y el puntaje
    // drawTextOnMatrix("SCORE", 2, 8);
    // char scoreStr[10];
    // sprintf(scoreStr, "%d", scoreInv); // Convierte el puntaje a cadena de texto
    // drawTextOnMatrix(scoreStr, 8, 16); // Dibuja el puntaje en el área correspondiente
    // matrix.write();
    // delay(2000);

    // // Mostrar el menú de guardado y manejar la lógica de guardado
    // handleSaveMenu(scoreInv);
    // showMenu();


