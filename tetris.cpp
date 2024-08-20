#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <UbidotsEsp32Mqtt.h>
#include "Database.h"
#include "tetris.h"
#include "blocks.h"
#include "joystick.h"
#include "pauseMenu.h"
#include "menu.h"
#include "config.h"
#include "Sound.h"
#include "charact.h"
#include "utils.h"
#include "saveMenu.h"


extern Max72xxPanel matrix;
extern Joystick joystick;

unsigned long timeTetris = 0; 

struct Block {
    int type;
    int rotation;
    int x;
    int y;
};

Block currentBlock;
Block nextBlock;
bool gameBoard[32][32] = {0};
bool holdButton = false;

int scoreTet = 0; // Variable para almacenar el puntaje
float gameSpeed = 1-0.05*(scoreTet / 6);

void spawnBlock();
void handleInputTet();
void rotateBlock();
void updateGame();
bool checkCollisionTet(int dx, int dy);
void moveBlock(int dx, int dy);
void fixBlock();
void clearLines();
void drawGameTet();
void drawNext();
void drawBorders();
void playMelodyTet();
void handlePauseMenu();
void handlePauseButton();
void clearBoard();
void drawScore();
void drawDigit(int x, int y, int digit);
void MuteSound();
void endScreen();
void drawSaveMenu();
void handleSaveMenu(int score);
void playPlayerDeathSound();


void setupTetris() {
    scoreTet = 0;
    gameSpeed = 1-0.05*(scoreTet / 12);
    clearBoard();
    randomSeed(analogRead(14)); // Inicializa la semilla para la generación de números aleatorios
    spawnBlock(); // Genera el primer bloque
    initPauseMenu(); // Inicializa el menú de pausa
    setupSound(); // Inicializa el sonido
}

void loopTetris() {
    if (digitalRead(PAUSE_BUTTON_PIN) == LOW) { // Si se presiona el botón de pausa
        MuteSound();
        handlePauseButton();
        handlePauseMenu(); // Maneja el menú de pausa
    } else {
        playMelodyTet(); // Reproduce la melodía continuamente
        handleInputTet(); // Maneja la entrada del usuario
        drawGameTet(); // Dibuja el juego en la pantalla
        updateGame(); // Actualiza el estado del juego
        delay(50*gameSpeed); // Retraso para el control del juego
    }
}

void spawnBlock() {
    // Genera un nuevo bloque aleatorio y lo posiciona al inicio del área de juego
    currentBlock = nextBlock;
    nextBlock.type = random(0, 7);
    nextBlock.rotation = random(0, 4);
    nextBlock.x = 15;
    nextBlock.y = 0;
}

void handleInputTet() {
    // Maneja la entrada del joystick y rota el bloque si se presiona el botón
    int xValue = joystick.readX();
    int yValue = joystick.readY();
    bool pressed = joystick.isPressed();


    if (millis() - timeTetris > 100*gameSpeed){
        timeTetris = millis();
        if (xValue < 1) {
            moveBlock(-1, 0);
        } else if (xValue > 4094) {
            moveBlock(1, 0);
        }

        if (yValue > 4094) {
            moveBlock(0, 1);
        }

        if (pressed) {
            if (!holdButton){
                rotateBlock();
                holdButton = true;
            }
        }
        else {
            holdButton = false;
        }
    }
}

void rotateBlock() {
    // Rota el bloque en sentido horario y verifica colisiones
    int originalRotation = currentBlock.rotation;
    currentBlock.rotation = (currentBlock.rotation + 1) % 4;

    if (checkCollisionTet(0, 0)) {
        currentBlock.rotation = originalRotation;
    }
}

void updateGame() {
    // Actualiza la posición del bloque y verifica si hay colisiones con el suelo o con otros bloques
    if (!checkCollisionTet(0, 1)) {
        moveBlock(0, 1);
    } else {
        fixBlock(); // Fija el bloque en su posición actual
        clearLines(); // Limpia las líneas completas
        spawnBlock(); // Genera un nuevo bloque
        if (checkCollisionTet(0, 0)) {
            // Si el nuevo bloque colisiona al ser generado, el juego termina
            // Aquí puedes agregar lógica para finalizar el juego
            playPlayerDeathSound();
            MuteSound();
            delay(2000);
            clearBoard();
            endScreen();
            // Falta la pantalla para registrar puntos
        }
    }
}

bool checkCollisionTet(int dx, int dy) {
    // Verifica si el bloque colisiona con el borde del área de juego o con otros bloques
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (BLOCKS[currentBlock.type][currentBlock.rotation][i][j] == 1) {
                int x = currentBlock.x + j + dx;
                int y = currentBlock.y + i + dy;
                if (x < 8 || x >= 24 || y >= 32 || (y >= 0 && gameBoard[y][x])) {
                    return true;
                }
            }
        }
    }
    return false;
}

void moveBlock(int dx, int dy) {
    // Mueve el bloque si no hay colisión
    if (!checkCollisionTet(dx, dy)) {
        currentBlock.x += dx;
        currentBlock.y += dy;
    }
}

void fixBlock() {
    // Fija el bloque en su posición actual en el tablero de juego
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (BLOCKS[currentBlock.type][currentBlock.rotation][i][j] == 1) {
                int x = currentBlock.x + j;
                int y = currentBlock.y + i;
                if (y >= 0) {
                    gameBoard[y][x] = true;
                }
            }
        }
    }
}

void clearLines() {
    // Limpia las líneas completas y actualiza el puntaje
    for (int i = 0; i < 32; i++) {
        bool lineComplete = true;
        for (int j = 8; j < 24; j++) {
            if (!gameBoard[i][j]) {
                lineComplete = false;
                break;
            }
        }
        if (lineComplete) {
            for (int k = i; k > 0; k--) {
                for (int j = 8; j < 24; j++) {
                    gameBoard[k][j] = gameBoard[k - 1][j];
                }
            }
            for (int j = 8; j < 24; j++) {
                gameBoard[0][j] = false;
            }
            scoreTet += 10; // Incrementa el puntaje en 100 puntos por línea eliminada
        }
    }
}

void drawGameTet() {
    // Dibuja el estado actual del juego en la pantalla
    matrix.fillScreen(LOW);
    drawBorders(); // Dibuja los bordes laterales
    drawScore(); // Dibuja el puntaje a la izquierda del juego
    drawNext();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (BLOCKS[currentBlock.type][currentBlock.rotation][i][j] == 1) {
                int x = currentBlock.x + j;
                int y = currentBlock.y + i;
                if (x >= 0 && x < 32 && y >= 0 && y < 32) {
                    matrix.drawPixel(x, y, HIGH);
                }
            }
        }
    }

    for (int i = 0; i < 32; i++) {
        for (int j = 7; j < 24; j++) {
            if (gameBoard[i][j]) {
                matrix.drawPixel(j, i, HIGH);
            }
        }
    }

    matrix.write();
}

void clearBoard(){
    for (int y = 0; y < 32; y++){
        for (int x = 8; x < 24; x++){
            gameBoard[y][x] = false;
        }
    }
}

void drawScore(){
    for (int i = 1; i < 28; i++){
        for (int j = 1; j < 7; j++){
            matrix.drawPixel(j, i, LOW); // Espacio para el puntaje
        }
    }

    char scoreStr[10];
    sprintf(scoreStr, "%d", scoreTet); // Convierte el puntaje a cadena de texto

    drawVertTextOnMatrix(scoreStr, 1, 2);
}

void drawNext(){
    for (int i = 1; i < 7; i++){
        for (int j = 25; j < 31; j++){
            matrix.drawPixel(j, i, LOW); // Espacio para el siguiente bloque
        }
    }

    int x = 26; // Posición en el borde izquierdo
    int y = 2;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (BLOCKS[nextBlock.type][nextBlock.rotation][i][j] == 1) {
                matrix.drawPixel(x + j, y + i, HIGH);
            }
        }
    }

    
}

void endScreen(){
    matrix.fillScreen(LOW);
    drawTextOnMatrix("GAME",6,8);
    drawTextOnMatrix("OVER",6,16);
    matrix.write();
    delay(1000);
    bool loseTetris = true;
    while(loseTetris){
        delay(100);
        matrix.fillScreen(LOW);
        drawTextOnMatrix("SCORE",2,8);
        char scoreStr[10];
        sprintf(scoreStr, "%d", scoreTet); // Convierte el puntaje a cadena de texto
        drawTextOnMatrix(scoreStr, 12, 16); // Dibuja el puntaje 
        matrix.write();
        delay(2000);

        handleSaveMenu(scoreTet);
        loseTetris = false;
        MuteSound();
        currentGame = NONE;
        showMenu();
    }
}

