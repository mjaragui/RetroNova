#include <WiFi.h>
#include "UbidotsEsp32Mqtt.h"

// Configuraciones de WiFi
const char* SSID = "AndroidAP869D";
const char* PASSWORD = "eenv7549";

// Configuraciones de Ubidots
const char* TOKEN = "BBUS-1fJ8PGNk9KXkdxcl0pSOhzBKfN3lDq";  // Reemplaza con tu token de Ubidots
const char* DEVICE_LABEL = "esp32-arcade";  // Nombre del dispositivo en Ubidots
const char* NEW_SCORE_LABEL = "tempScore";  // Label de la variable temporal para recibir un nuevo puntaje
char* NEW_NAME_LABEL = "tempAlias";  // Label de la variable temporal para recibir un nuevo nombre

// Labels de las variables de puntajes en Ubidots
const char* SCORE_LABEL_1 = "Score1";
const char* SCORE_LABEL_2 = "Score2";
const char* SCORE_LABEL_3 = "Score3";

char* NAME_LABEL_1 = "Alias1";
char* NAME_LABEL_2 = "Alias2";
char* NAME_LABEL_3 = "Alias3";


// Instancia de Ubidots
Ubidots ubidots(TOKEN);

// Variables para los puntajes
String playerNames[3] = {"", "", ""};  // Array para almacenar los nombres de los jugadores
float highScores[3] = {0, 0, 0};  // Inicialmente los puntajes están en 0
bool scoresInitialized = false;  // Bandera para saber si ya se han inicializado los puntajes

// Función callback para manejar la recepción de mensajes de Ubidots
void callback(char* topic, byte* payload, unsigned int length) {

    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    
    payload[length] = '\0';  // Asegurar que el payload esté correctamente terminado en NULL
    String valueStr = String((char*)payload);
    float value = valueStr.toFloat();
    bool newHighScore = false;
    int index = 0;

    // Determinar cuál variable ha sido recibida y actualizar el puntaje correspondiente
    if (strstr(topic, SCORE_LABEL_1)) {
        highScores[0] = value;
    } else if (strstr(topic, SCORE_LABEL_2)) {
        highScores[1] = value;
    } else if (strstr(topic, SCORE_LABEL_3)) {
        highScores[2] = value;
    } else if (strstr(topic, NEW_SCORE_LABEL)) {
        Serial.print("Nuevo puntaje recibido: ");
        Serial.println(value);

        // Insertar el nuevo puntaje en el lugar correcto
        for (int i = 0; i < 3; i++) {
        if (value > highScores[i]) {
            // Desplazar puntajes inferiores hacia abajo
            for (int j = 2; j > i; j--) {
            highScores[j] = highScores[j-1];
            }
            // Insertar el nuevo puntaje en la posición correcta
            highScores[i] = value;
            newHighScore = true;
            index = i;
            break;
        }
        }
    } else if (strstr(topic, NAME_LABEL_1)) {
        playerNames[0] = valueStr;
    } else if (strstr(topic, NAME_LABEL_2)) {
        playerNames[1] = valueStr;
    } else if (strstr(topic, NAME_LABEL_3)) {
        playerNames[2] = valueStr;
    } else if (strstr(topic, NEW_SCORE_LABEL)) {
        if  (newHighScore) {
            playerNames[index] = valueStr;
        }
    }

  // Verificar si todos los puntajes han sido inicializados
    if (highScores[0] != 0 && highScores[1] != 0 && highScores[2] != 0 && highScores[3] != 0 && highScores[4] != 0) {
        scoresInitialized = true;
    }
}

void databaseSetup() {
    // Conexión a WiFi
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");

    // Conexión a Ubidots y suscripción a las variables de puntajes
    ubidots.setCallback(callback);
    ubidots.connectToWifi(SSID, PASSWORD);
    ubidots.setup();
    ubidots.reconnect();


    // Suscribirse a cada variable de puntaje
    ubidots.subscribeLastValue(DEVICE_LABEL, SCORE_LABEL_1);
    ubidots.subscribeLastValue(DEVICE_LABEL, SCORE_LABEL_2);
    ubidots.subscribeLastValue(DEVICE_LABEL, SCORE_LABEL_3);
    ubidots.subscribeLastValue(DEVICE_LABEL, NEW_SCORE_LABEL);  // Suscribirse también a la variable de nuevo puntaje

    // Suscribirse a cada variable de nombre   
    ubidots.subscribeLastValue(DEVICE_LABEL, NAME_LABEL_1);
    ubidots.subscribeLastValue(DEVICE_LABEL, NAME_LABEL_2);
    ubidots.subscribeLastValue(DEVICE_LABEL, NAME_LABEL_3);

    // Suscribirse a la variable temporal para recibir un nuevo nombre
    ubidots.subscribeLastValue(DEVICE_LABEL, NEW_NAME_LABEL);
}

void databaseLoop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, SCORE_LABEL_1);
    ubidots.subscribeLastValue(DEVICE_LABEL, SCORE_LABEL_2);
    ubidots.subscribeLastValue(DEVICE_LABEL, SCORE_LABEL_3);
    ubidots.subscribeLastValue(DEVICE_LABEL, NEW_SCORE_LABEL);

    // Suscribirse a cada variable de nombre
    ubidots.subscribeLastValue(DEVICE_LABEL, NAME_LABEL_1);
    ubidots.subscribeLastValue(DEVICE_LABEL, NAME_LABEL_2);
    ubidots.subscribeLastValue(DEVICE_LABEL, NAME_LABEL_3);
    ubidots.subscribeLastValue(DEVICE_LABEL, NEW_NAME_LABEL); // Suscribirse a la variable temporal para recibir un nuevo nombre
    }

    ubidots.loop();  // Mantenimiento de la conexión MQTT

    // Publicar los puntajes altos actuales si ya se han inicializado
    if (scoresInitialized) {
        ubidots.add(SCORE_LABEL_1, highScores[0]);
        ubidots.add(SCORE_LABEL_2, highScores[1]);
        ubidots.add(SCORE_LABEL_3, highScores[2]);

        ubidots.addContext(NAME_LABEL_1, &playerNames[0][0]);
        ubidots.addContext(NAME_LABEL_2, &playerNames[1][0]);
        ubidots.addContext(NAME_LABEL_3, &playerNames[2][0]);

        ubidots.publish(DEVICE_LABEL);
        scoresInitialized = false;
    }
}

void cloudScore(float player, float score){
    ubidots.add(NEW_SCORE_LABEL, score);
    ubidots.add(NEW_NAME_LABEL, player);

    ubidots.publish(DEVICE_LABEL);
    Serial.println("cloud");
}