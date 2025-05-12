// Incluir bibliotecas
#include <heltec.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "images.h"

// configuracion LoRa Banda ISM en Región 915Mhz
  #define BAND 915E6       // 433E6,868E6,915E6
  byte spread_factor = 8;  // rango 6-12,default 7

// Mensaje LoRa a enviar por direcciones//////////////////
  byte dir_local = 0xD3;    // Dispositivo 1 dirección local del dispositivo
  byte dir_destino = 0xc1;  // Dispositivo 2 dirección local del dispositivo
  byte id_msjLoRa = 0;      // ID del mensaje (contador)

// Mensaje LoRa recibido////////////////////////////////
  byte dir_envio  = 0xd3; // receptor
  byte dir_remite = 0xc1; // emisor
  String paqueteRcb = ""; // mensaje LoRa
  byte   paqRcb_ID  = 0;
  byte   paqRcb_Estado = 0;
  // 0:vacio, 1: nuevo, 2:incompleto
  // 3:otro destinatario, 4:Broadcast*/

// variables para mensajes por Puerto Serial activa///////
  volatile boolean serial_msj = true;  // Indicador para activar los mensajes por el puerto serial

// SENSORES
  #define senHumSuelo 39 // Pin de Humedad del suelo %
  #define senGasC 37      // Pin del monoxido-Carbono MQ-9 20 hasta 2000 ppm
  #define senGasM 38  // Pin del gas-Metano MQ-4 200 y 10000 ppm-
  #define senTemp 13  //Pin de temperatura grados celcius
  #define pinTransis 25   // Pin donde estara conectada el motor
  // Sensor de Bateria
  # define BattPIN 36
  String battNivel;

// Instancia las clases
OneWire oWireTemp(senTemp);             //Se establece el pin 39 temperatura DS18B20
DallasTemperature DS18B20(&oWireTemp);  //Se declara una variable u objeto para el sensor

// Variables ara almacenar la lectura
String humedad = "";
String Gas_C = "";
//String Gas_LP="";
String Gas_M = "";
String Tem_Cel = "";
String valorM="";
String estANT_M = "0";

// tiempo entre lecturas
long tiempo_antes = 0;         // Tiempo antes de la última lectura
long tiempo_intervalo = 5000;  // Intervalo de tiempo entre lecturas
//long tiempo_espera = tiempo_intervalo + random(2000);

int rssi_lora = 0;  // nivel de señal LoRa
int snr_lora = 0;   // Relación señal-ruido LoRa

void logo(){
	Heltec.display -> clear();
	Heltec.display -> drawXbm(0,5,logo_width,logo_height,(const unsigned char *)logo_bits);
	Heltec.display -> display();
}

void setup() {

  DS18B20.begin();  //Inicia el sensor temperatura

  // Inicializar la comunicación LoRa y configurar la banda y el factor de propagación
  Heltec.begin(true /*Habilita el uso del display integrado en la placa*/,
               true /*Deshabilita la inicialización de la comunicación LoRa*/,
               serial_msj /*Habilita la comunicación serial para la depuración y la visualización de mensajes en el monitor serial*/,
               true /*Habilita el amplificador de potencia (PA_BOOST) para la transmisión LoRa*/, 
               BAND /*Configura la banda de frecuencia para la comunicación LoRa (por ejemplo, 915E6 para América).*/
               );
               
  LoRa.setSpreadingFactor(spread_factor);
  
  //LoRa.onReceive(cbk);
  LoRa.receive();

  // Display
  Heltec.display->init(); 
  logo();
  delay(1500);

  Heltec.display -> clear();
  Heltec.display -> drawString(0, 0, "Conexion con pantalla OK");
  Heltec.display ->drawString(0, 20, "Direccion local: " + String(dir_local, HEX));
  Heltec.display ->drawString(0, 30, "Estableciendo pines");
  Heltec.display -> display();
  Heltec.display -> clear();

  //Serial.print("Sensores activos");

  //Inicializa los pines de los sensores
  pinMode(senHumSuelo, INPUT);
  pinMode(senGasC, INPUT);
  //pinMode(senGasLP,INPUT); sensor no usuado
  pinMode(senGasM, INPUT);
  //inicializa sensor BATERIA
  pinMode(BattPIN, INPUT);
  //inicializa pin de motor
  pinMode(pinTransis,OUTPUT);
    delay(1000);
}

void loop() {/*
  if(valorM != estANT_M ){
    digitalWrite(MotorPin, 1);  // Enciende el módulo relé si la humedad es igual o mayor a 50  
  } else {
  digitalWrite(MotorPin, 0);  // Apaga el módulo relé si la humedad es menor a 50
  }*/

  ////////////////////////////////////////// Enviar mensajes entre intervalos/////////////////////////////////////////////////////////////

  // Verificar si es el momento de enviar un mensaje
  long tiempo_ahora = millis();
  long t_transcurrido = tiempo_ahora - tiempo_antes;


  if (t_transcurrido >= tiempo_intervalo) {

    // Actualizar el estado del sensor
    sensor_revisa();
    // actualiza estado de bateria
    sensorBateria(); 
    // construccion y guaradado de valores en mensaje JSON
    String paqueteEnv = "{\"Humedad\":" + humedad + //(%)
                        ",\"CO\":"+ Gas_C + //(ppm)
                        ",\"Metano\":" + Gas_M + // (ppm)
                        ",\"Temperatura\":" + Tem_Cel + //(ºC)
                        ",\"Volts\":" + battNivel + //(V)
                        "}";  
    /*paqueteEnv = paqueteEnv + "PL:" + Gas_LP;
    paqueteEnv = paqueteEnv + "ppm|";*/

    // Enviar mensaje LoRa al dispositivo de destino
    envia_lora(dir_destino, dir_local, id_msjLoRa, paqueteEnv);
    id_msjLoRa = id_msjLoRa + 1;

    yield(); // Procesa eventos pendientes
    
    // Imprimir mensaje a serial monitor
    if (serial_msj == true) {
      Serial.print("Enviado:  ");
      Serial.print(String(dir_destino, HEX));
      Serial.print(",");
      Serial.print(String(dir_local, HEX));
      Serial.print(",");
      Serial.print(id_msjLoRa - 1);
      Serial.print(",");
      Serial.println(paqueteEnv);
    }

    // Actualizar el tiempo para el próximo envío y generar un nuevo tiempo de espera
    tiempo_antes = millis();
    tiempo_intervalo = 3000 + random(2000);

    // LED parpadea. Indicador de envio LoRa
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(50);
    //Mostrar dato en pantalla
        pantalla_Mostrar();
        Heltec.display -> display();
  }

  //////////////////////////////////////////////// Revisar mensajes LoRa entrantes////////////////////////////////////////
  int msjRcbLoRa = LoRa.parsePacket();
  if (msjRcbLoRa !=0){
      // Procesar y recibir mensajes LoRa
    recibe_lora(msjRcbLoRa);
    rssi_lora = LoRa.packetRssi();
    snr_lora  = LoRa.packetSnr();
    
     // Imprimir información en el monitor serial (si está habilitado)
    if (serial_msj == true){
      if (paqRcb_Estado == 1){
        Serial.print("Recibido: ");
        Serial.print(String(dir_remite,HEX)); 
        Serial.print(",");
        Serial.print(String(dir_envio,HEX));
        Serial.print(",");
        Serial.print(paqRcb_ID);  
        Serial.print(",");
        Serial.print(paqueteRcb); 
        Serial.print(",");
        Serial.print(rssi_lora);
        Serial.print(",");
        Serial.print(snr_lora);
        Serial.print(",");
        Serial.println();

      }else{
        Serial.print("Paquete recibido Estado: ");
        Serial.println(paqRcb_Estado);
      }
    }
     yield(); // procesa wifi
    // LED parpadea Rebibido Lora
    digitalWrite(LED, HIGH); 
    delay(50);
    digitalWrite(LED, LOW ); 
    delay(50);
    digitalWrite(LED, HIGH); 
    delay(50);
    digitalWrite(LED, LOW );
  }

  delay(100);
  yield(); // procesa wifi
  Heltec.display->clear();
}

void sensorBateria(){
  //digitalWrite(pinTransis, 1);  // Enciende la lectura de Pila
  //delay(100);

  int lectura = analogRead(BattPIN);
  // convierte a equivalente en voltios
  float voltaje = (float(lectura)/4096.0)*3.7*3.0/2.0;
  // Calcula el porcentaje de batería
  float porcentaje = (voltaje / 1.06) * 100.0;
  // Asegura que el porcentaje esté en el rango de 0 a 100
  porcentaje = constrain(porcentaje, 0, 100);

  battNivel = String(porcentaje).c_str();

  //digitalWrite(pinTransis, 0);  // Apaga la lectura de la Pila}
  //delay(10);
}