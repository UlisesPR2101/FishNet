void sensor_revisa(){

  // Leer Humedad
  int hume_S = analogRead(senHumSuelo); // Leemos el valor que envía el sensor
  humedad = map(hume_S, 600 * 4, 700*4, 100/4, 0/4); // Mapeamos el valor de lectura a un rango de humedad de 0 a 100
  humedad =String(humedad).c_str();// transforma a una cadena

  /*/ Controlar el módulo relé utilizando la variable humedad
  if (humedad.toInt() <= 10) {
    valorM = "1";// Encender motor
  } else {
    valorM = "0";// Apagar motor
  }*/
  
  // Leer Gas CO
  int Gas_C_S = analogRead(senGasC); // Leemos el valor que envía el sensor
  Gas_C = map(Gas_C_S, 0, 4095, 20, 2000); // Mapeamos el valor de lectura a un rango de gas co de 0 a 100
  Gas_C =String(Gas_C).c_str();//transforma una cadena

  /*/ Leer Gas LP
  int Gas_LP_S = analogRead(senGasLP); // Leemos el valor que envía el sensor
  Gas_LP = map(Gas_LP_S, 0, 4095, 200, 10000); // Mapeamos el valor de lectura a un rango de gas lp de 0 a 100
  Gas_LP =String(Gas_LP).c_str();// transforma a una cadena
*/
   // Leer Gas Metano
  int Gas_M_S = analogRead(senGasM); // Leemos el valor que envía el sensor
  Gas_M = map(Gas_M_S, 0, 4095, 200, 10000); // Mapeamos el valor de lectura a un rango de gas metano de 0 a 100
  Gas_M =String(Gas_M).c_str();// transforma a una cadena

  // Leer temperatura
  DS18B20.requestTemperatures(); //Se envía el comando para leer la temperatura
  float Temp_S= DS18B20.getTempCByIndex(0); //Se obtiene la temperatura en ºC
  Tem_Cel=String(Temp_S).c_str();// transforma a una cadena
 
}