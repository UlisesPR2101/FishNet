#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//LIBRERIAS
#include <OneWire.h>
#include <DallasTemperature.h>

// Reemplaza con tus credenciales de Wi-Fi
const char *ssid ="Y9";// "Ubee3150";DELL_1253
const char *password ="12345678";// "DA0C8B3150";X22R010322

// MQTT details
// Reemplaza con la dirección del broker MQTT
const char *mqttServer = "192.168.43.128";//"broker.hivemq.com"; // Ejemplo: puedes usar tu propio broker  192.168.43.128 || 192.168.1.100 || 178.6.8.81
const int mqttPort = 1883;
const char *mqttUser = ""; // Si es necesario
const char *mqttPassword = ""; // Si es necesario

// MQTT sensores Ocenao (estos si se pueden modificar)
const char* topicS1  = "GsmClient/Oceano/JSON";

///////////////////////////////////////Pines  de iniciacion////////////////////////////////////
const int mq135Pin = 15; // Cambia esto si usas otro pin ///Amarilo
#define senTemp 13 // Pin de temperatura en grados Celsius
#define senTDS 33      // Pin del TDS 0 ~ 1000ppm --- Precisión de medición de TDS: ± 10% FS (25 ℃) //Azul
const byte senPH = 32;     // Pin del PH de  0 ~ 14.(acido /base)  //verde
int  LDR = 34;     //Pin de temperatura grados celcius
////////////////////// Instancia las clases de temperatura ////////////////////////////
OneWire oWireTemp(senTemp);            // Se establece el pin para el sensor DS18B20
DallasTemperature DS18B20(&oWireTemp); // Se declara una variable u objeto para el sensor

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    Algoritmos

//Sensor TDS
  #define VREF 3.3              // voltaje de referencia analógica(Volt) del ADC
  #define SCOUNT  50            // suma de puntos de muestreo

  int analogBuffer[SCOUNT];     // almacena el valor analógico en el array, leído del ADC
  int analogBufferTemp[SCOUNT];
  int analogBufferIndex = 0;
  int copyIndex = 0;

  float averageVoltage = 0;
  float tdsValue = 0;
  
// algoritmo de filtrado de mediana
  int getMedianNum(int bArray[], int iFilterLen){
    int bTab[iFilterLen];
    for (byte i = 0; i<iFilterLen; i++)
    bTab[i] = bArray[i];
    int i, j, bTemp;
    //algoritmo de burbuja
    for (j = 0; j < iFilterLen - 1; j++) {
      for (i = 0; i < iFilterLen - j - 1; i++) {
        if (bTab[i] > bTab[i + 1]) {
          bTemp = bTab[i];
          bTab[i] = bTab[i + 1];
          bTab[i + 1] = bTemp;
        }
      }
    }
    if ((iFilterLen & 1) > 0){
      bTemp = bTab[(iFilterLen - 1) / 2];
    }
    else {
      bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
    }
    return bTemp;
  }

//variables de PH
  float Po;
  float buf[20];

//////////////////////////////////// Variables globlales////////////////////////////////////////
String SV135 = "" ;
String Tem_Cel = "";
String TDS = "";
String PH = "";
String Lumenes = "";

String paquete ="";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  pinMode(mq135Pin,INPUT);
  DS18B20.begin();  //Inicia el sensor temperatura
  pinMode(senTDS, INPUT);
  pinMode(LDR, INPUT_PULLUP);

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  Serial.begin(115200);
  setupWiFi();
  client.setServer(mqttServer, mqttPort);
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("Conectado a Wi-Fi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("conectado");
    } else {
      Serial.print("fallo, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(500);
  sensar();
  
  paquete = "{\"MQ135\":" + SV135 + //
             ",\"Temp\":"+ Tem_Cel + // (%)
             ",\"TDS\":" + TDS + // (ppm)
             ",\"PH\":" + PH + //(ºC)
             ",\"LDR\":" + Lumenes + //(V)
             "}";  

  // Publicar el JSON en el tópico
  if (client.publish(topicS1, paquete.c_str())) {
    Serial.print("Mensaje enviado: ");
    Serial.println(paquete); // Imprimir el mensaje enviado
  } else {
    Serial.println("Error al enviar el mensaje");
  }

  // Esperar 5 segundos antes de enviar de nuevo
  delay(100);
}