/**************************************************************

   For this example, you need to install PubSubClient library:
     https://github.com/knolleary/pubsubclient
     or from http://librarymanager/all#PubSubClient

   TinyGSM Getting Started guide:
     https://tiny.cc/tinygsm-readme

   For more MQTT examples, see PubSubClient library

 **************************************************************
   This example connects to HiveMQ's showcase broker.

   You can quickly test sending and receiving messages from the HiveMQ webclient
   available at http://www.hivemq.com/demos/websocket-client/.

   Subscribe to the topic GsmClientTest/ledStatus
   Publish "toggle" to the topic GsmClientTest/led and the LED on your board
   should toggle and you should see a new message published to
   GsmClientTest/ledStatus with the newest LED status.

 **************************************************************/

 /*
Este programa envia los Datos de los sensores por un broker publico, leeido  atravez de algoritmos para determinar el mejor valor

Created by Ulises Porras Rosas
Febrero, 2025
*/

//LIBRERIAS
#include <OneWire.h>
#include <DallasTemperature.h>

// Select your modem:
#define TINY_GSM_MODEM_SIM7000

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#ifndef _AVR_ATmega328P_
#define SerialAT Serial1

// or Software Serial on Uno, Nano
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3);  // RX, TX
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet.
// This is only needed for this example, not in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]  = "internet.itelcel.com";     //SET TO YOUR APN
const char gprsUser[] = "itelcel";
const char gprsPass[] = "itelcel";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "Y9";
const char wifiPass[] = "12345678";

// MQTT details
const char* broker = "broker.hivemq.com";

const char* topicLed       = "GsmClientTest/led";
const char* topicInit      = "GsmClientTest/init";
const char* topicLedStatus = "GsmClientTest/ledStatus";

// MQTT sensores Ocenao (estos si se pueden modificar)
const char* topicS1  = "GsmClient/Oceano/JSON";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient  mqtt(client);

#define LED_PIN 12
int ledStatus = LOW;

///////////////////////////////////////Pines  de iniciacion////////////////////////////////////
const int mq135Pin = 15; // Cambia esto si usas otro pin
#define senTemp 13 // Pin de temperatura en grados Celsius
#define senTDS 33      // Pin del TDS 0 ~ 1000ppm --- Precisión de medición de TDS: ± 10% FS (25 ℃)
const byte senPH = 32;     // Pin del PH de  0 ~ 14.(acido /base)
int  LDR = 34;    //Pin de temperatura grados celcius
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

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();

  // Only proceed if incoming message's topic matches
  if (String(topic) == topicLed) {
    ledStatus = !ledStatus;
    digitalWrite(LED_PIN, ledStatus);
    mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
   delay(500);
   sensar();
  
  paquete = "{\"MQ135\":" + SV135 + //
             ",\"Temp\":"+ Tem_Cel + // (%)
             ",\"TDS\":" + TDS + // (ppm)
             ",\"PH\":" + PH + //(ºC)
             ",\"LDR\":" + Lumenes + //(V)
             "}";  

   mqtt.publish(topicS1, paquete.c_str());
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker
  boolean status = mqtt.connect("GsmClientTest");

  // Or, if you want to authenticate MQTT:
  // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.publish(topicInit, "GsmClientTest started");
  mqtt.subscribe(topicLed);
  return mqtt.connected();
}


void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  pinMode(mq135Pin,INPUT);
  DS18B20.begin();  //Inicia el sensor temperatura
  pinMode(senTDS, INPUT);
  pinMode(LDR, INPUT_PULLUP);

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  // Starting the machine requires at least 1 second of low level, and with a level conversion, the levels are opposite
  delay(1000);
  digitalWrite(4, LOW);

  SerialMon.println("Wait...");

  // Set GSM module baud rate
  //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.begin(9600, SERIAL_8N1, 26, 27);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }
#endif

#if TINY_GSM_USE_WIFI
  // Wifi connection parameters must be set before waiting for the network
  SerialMon.print(F("Setting SSID/password..."));
  if (!modem.networkConnect(wifiSSID, wifiPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");
#endif

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }
#endif

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }

#if TINY_GSM_USE_GPRS
    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected()) {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) {
        SerialMon.println("GPRS reconnected");
      }
    }
#endif
  }

  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  mqtt.loop();
}