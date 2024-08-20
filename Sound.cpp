#include <Arduino.h>
#include "Sound.h"

unsigned long previousTime = 0;
const int noteMillis = 1;
int note = 0;

// Melodía del Tetris y duraciones
const int melody[] = { // Line per measure
  NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, // 1
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, // 2
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, // 3
  NOTE_C5, NOTE_A4, NOTE_A4, MUTE, // 4
  MUTE, NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5, // 5
  NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, // 6
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, // 7
  NOTE_C5, NOTE_A4, NOTE_A4, MUTE, // 8
  NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, // 9
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, // 10
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, // 11
  NOTE_C5, NOTE_A4, NOTE_A4, MUTE, // 12
  MUTE, NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5, // 13
  NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, // 14
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, // 15
  NOTE_C5, NOTE_A4, NOTE_A4, MUTE, // 16
  NOTE_E3, NOTE_C3, // 17
  NOTE_D3, NOTE_B2, // 18
  NOTE_C3, NOTE_A2, // 19
  NOTE_GS2, NOTE_B2, // 20
  NOTE_E3, NOTE_C3, // 21
  NOTE_D3, NOTE_B2, // 22
  NOTE_C3, NOTE_E3, NOTE_A3, // 23
  NOTE_GS3, NOTE_GS3, MUTE // 24
};

const int noteDurations[] = {
  0, // Line per measure
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 1
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 2
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 3
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 4
  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 5
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 6
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 7
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 8
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 9
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 10
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 11
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 12
  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 13
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, // 14
  QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 15
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 16
  HALF_NOTE, HALF_NOTE, // 17
  HALF_NOTE, HALF_NOTE, // 18
  HALF_NOTE, HALF_NOTE, // 19
  HALF_NOTE, HALF_NOTE, // 20
  HALF_NOTE, HALF_NOTE, // 21
  HALF_NOTE, HALF_NOTE, // 22
  QUARTER_NOTE, QUARTER_NOTE, HALF_NOTE, // 23
  HALF_NOTE, QUARTER_NOTE, QUARTER_NOTE // 24
};

const int spaceInvadersMelody[] = { 
  NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, // 1
  NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, // 2
  NOTE_D4, NOTE_A3, NOTE_D4, NOTE_A3, // 3
  NOTE_D4, NOTE_A3, NOTE_D4, NOTE_A3, // 4
  NOTE_E4, NOTE_B3, NOTE_E4, NOTE_B3, // 5
  NOTE_E4, NOTE_B3, NOTE_E4, NOTE_B3, // 6
  NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, // 7
  NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, // 8
};

const int spaceInvadersNoteDurations[] = {
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 1
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 2
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 3
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 4
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 5
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 6
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 7
  QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, // 8
};

// Sonido para las balas de las naves enemigas
const int enemyBulletSound[] = { NOTE_B3 };
const int enemyBulletDurations[] = { SIXTEENTH_NOTE };

// Sonido para las balas del jugador
const int playerBulletSound[] = { NOTE_C5 };
const int playerBulletDurations[] = { SIXTEENTH_NOTE };

// Sonido cuando se destruye el jugador
const int playerDeathSound[] = { NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4 };
const int playerDeathDurations[] = { QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE };

void setupSound() {
    // Configuración del canal LEDC para la generación de tonos
    ledcSetup(0, 1000, 8);  // Canal 0, frecuencia base 1kHz, resolución 8 bits
    ledcAttachPin(BUZZER_PIN, 0);  // Asigna el pin al canal 0
}

void playSound(const int *melody, const int *durations, int length) {
    for (int i = 0; i < length; i++) {
        ledcWriteTone(0, melody[i]);
        delay(noteMillis * durations[i]);
    }
    //MuteSound(); // Apaga el sonido después de la reproducción
}

void playSpaceInvadersMelody() {
    // playSound(spaceInvadersMelody, spaceInvadersNoteDurations, sizeof(spaceInvadersMelody)/sizeof(spaceInvadersMelody[0]));
    unsigned long currentTime = millis();

    if ((currentTime - previousTime >= noteMillis*spaceInvadersNoteDurations[note]) && (note < 32)) { 
      
      ledcWriteTone(0,spaceInvadersMelody[note]); // Se asigna la nota al canal 0
      
      note++; // Se prepara la proxima nota

      previousTime = currentTime; 
    }
    else  if (currentTime - previousTime >= 400*noteMillis) { // pausa final
        ledcWriteTone(0, 0);
        if (currentTime - previousTime >= 800*noteMillis){
          note = 0;
          previousTime = currentTime;
        }
    }
}

void playEnemyBulletSound() {
    playSound(enemyBulletSound, enemyBulletDurations, sizeof(enemyBulletSound)/sizeof(enemyBulletSound[0]));
}

void playPlayerBulletSound() {
    playSound(playerBulletSound, playerBulletDurations, sizeof(playerBulletSound)/sizeof(playerBulletSound[0]));
}

void playPlayerDeathSound() {
    playSound(playerDeathSound, playerDeathDurations, sizeof(playerDeathSound)/sizeof(playerDeathSound[0]));
}

void playMelodyTet() {

  unsigned long currentTime = millis();

  if ((currentTime - previousTime >= noteMillis*noteDurations[note]) && (note < 102)) { 
    
    ledcWriteTone(0,melody[note]); // Se asigna la nota al canal 0
    
    note++; // Se prepara la proxima nota

    previousTime = currentTime; 
  }
  else  if (currentTime - previousTime >= 400*noteMillis) { // pausa final
      ledcWriteTone(0, 0);
      if (currentTime - previousTime >= 800*noteMillis){
        note = 0;
        previousTime = currentTime;
      }
  }

}

void MuteSound(){
  ledcWriteTone(0,MUTE);
}
