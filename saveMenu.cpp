#include <Arduino.h>
#include <UbidotsEsp32Mqtt.h>
#include "Database.h"
#include "saveMenu.h"
#include "charact.h"
#include "utils.h"
#include <Max72xxPanel.h>
#include "joystick.h"
#include "menu.h"

extern Max72xxPanel matrix;
extern Joystick joystick;

const char* saveOptions[] = {"SI", "NO"};
int selectedSaveOption = 0;
char playerName[4] = "AAA"; // Nombre de 3 letras, inicializado en 'AAA'
int currentLetterIndex = 0;
int currentLineIndex = 0;
bool isSelectingLetter = false;

void drawSaveMenu();
void selectionLine(int selectedoOption);
void saveScore(int score);
void handleNameInput(int score);
void handleSaveMenu(int score);
void cloudScore(char *label, float score);



void handleSaveMenu(int score) {
    drawSaveMenu();  //dibujar el menú al menos una vez al principio
    selectionLine(selectedSaveOption);

    while (true) {
        if (joystick.isPressed()) {
            while (joystick.isPressed()) { delay(30); }
            switch (selectedSaveOption) {
                case 0:
                    // Guardar el puntaje
                    handleNameInput(score);
                    showMenu();
                    return;
                case 1:
                    // No guardar el puntaje, ir al menú principal
                    showMenu();
                    return;
            }
        }
        int xValue = joystick.readX();    
            if (xValue < 1000) {
            selectedSaveOption = (selectedSaveOption - 1 + 2) % 2;
            drawSaveMenu();
            selectionLine(selectedSaveOption);
            matrix.write();
            delay(300);

        } else if (xValue > 3000) {
            selectedSaveOption = (selectedSaveOption + 1) % 2;
            drawSaveMenu();
            selectionLine(selectedSaveOption);
            matrix.write();
            delay(300);
        }
    }
}

void drawSaveMenu() {
    matrix.fillScreen(LOW);

    // Dibujar el borde de 2 LEDs de grosor
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 2; j++) {
            matrix.drawPixel(j, i, HIGH); // Borde izquierdo
            matrix.drawPixel(31 - j, i, HIGH); // Borde derecho
            matrix.drawPixel(i, j, HIGH); // Borde superior
            matrix.drawPixel(i, 31 - j, HIGH); // Borde inferior
        }
    }

    // Dibujar las opciones del menú de guardar
    drawTextOnMatrix("SAVE", 5, 4);
    for (int i = 0; i < 2; i++) {
        int x = 3 + i * 15; // Separación horizontal entre las opciones
        drawTextOnMatrix(saveOptions[i], x, 16); // Dibuja "SI" y "NO"
    }
    matrix.write();
}

void selectionLine(int  selectedOption) {
    int xStart = 5 + selectedOption * 15;  // Ajustar la posición en función de la opción seleccionada
    int y = 24;  // Línea horizontal debajo de "SI" y "NO"

    // Dibuja una línea horizontal de 6 LEDs debajo de la opción seleccionada
    for (int i = 0; i < 6; i++) {
        matrix.drawPixel(xStart + i, y, HIGH); // Dibuja la línea en la posición correcta
    }
}

void handleNameInput(int score) {
    matrix.fillScreen(LOW);

    // Dibujar bordes
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 2; j++) {
            matrix.drawPixel(j, i, HIGH); // Borde izquierdo
            matrix.drawPixel(31 - j, i, HIGH); // Borde derecho
            matrix.drawPixel(i, j, HIGH); // Borde superior
            matrix.drawPixel(i, 31 - j, HIGH); // Borde inferior
        }
    }

    drawTextOnMatrix("NAME", 4, 6);  
    matrix.write();

    while (currentLineIndex < 3) {
        Serial.println(currentLineIndex);
        int yPosition = 18; // Mantener la misma posición Y para todas las letras
        int xPosition = 8 + currentLineIndex * 8; // Posición X de la letra actual, desplazada a la derecha

        // Parpadeo de la línea actual
        if (isSelectingLetter) {
            selectionLine(xPosition); // Pasar la posición X
            matrix.write();
        }
        
        matrix.write();

        int yValue = joystick.readY();
        if (yValue < 1000) {
            // Cambiar letra hacia abajo en el alfabeto
            playerName[currentLineIndex] = (playerName[currentLineIndex] == 'A') ? 'Z' : playerName[currentLineIndex] - 1;
            delay(300);
        } else if (yValue > 3000) {
            // Cambiar letra hacia arriba en el alfabeto
            playerName[currentLineIndex] = (playerName[currentLineIndex] == 'Z') ? 'A' : playerName[currentLineIndex] + 1;
            delay(300);
        }

        if (joystick.isPressed()) {
            while (joystick.isPressed()) { delay(30); }
            isSelectingLetter = false;
            drawTextOnMatrix(&playerName[currentLineIndex], xPosition, yPosition); // Congela la letra seleccionada
            matrix.write();
            currentLineIndex++;
            isSelectingLetter = true; // Preparar para seleccionar la siguiente letra
        }

        // Actualiza solo la letra seleccionada en la posición correcta
        drawTextOnMatrix(playerName, 8, yPosition);
    }
    currentLineIndex = 0;

    // Guardar el nombre y puntaje
    saveScore(score);
    showMenu();
}

void saveScore(int score) {
    // Guardar puntaje y nombre
    Serial.print("Player Name: ");
    Serial.println(playerName);  // Imprime el nombre del jugador

    Serial.print("Score to save: ");
    Serial.println(score);  // Imprime el puntaje del jugador

    cloudScore(1, score);

    // *** ESPACIO RESERVADO PARA IMPLEMENTAR EL GUARDADO EN LA BASE DE DATOS ***
    // Aquí es donde se implementará la lógica para almacenar el puntaje y el nombre en una base de datos
    // en el futuro, o bien en una memoria externa, EEPROM, archivo, etc.
    
    // Feedback visual o confirmación opcional
    // Aquí podrías agregar una función que muestre en la pantalla que el nombre y puntaje han sido guardados.
}
