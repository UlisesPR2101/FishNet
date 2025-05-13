void sensar(){
   // Leer el valor del sensor 135
  int S135 = analogRead(mq135Pin);
  SV135=String(S135);
  Serial.print("MQ-135: ");
  Serial.println(SV135);

  // Leer temperatura
  DS18B20.requestTemperatures(); //Se envía el comando para leer la temperatura
  float Temp_S= DS18B20.getTempCByIndex(0); //Se obtiene la temperatura en ºC
  Tem_Cel=String(Temp_S);// transforma a una cadena
  Serial.print("Temperatura: ");
  Serial.println(Tem_Cel);

  // Leer DTS
    static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 60U){    //cada 30 milisegundos, lee el valor analógico del ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(senTDS); //lee el valor analógico y lo almacena en el buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
  }   
  
  static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint > 800U){ // 600 Cada 800 ms, copia analogBuffer a analogBufferTemp.
    printTimepoint = millis(); 
    for(copyIndex=0; copyIndex<SCOUNT; copyIndex++){
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      
      // lee el valor analógico más estable mediante el algoritmo de filtrado de mediana, y lo convierte a valor de voltaje
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 4096.0;
      
      //fórmula de compensación de temperatura: fFinalResult(25^C) = fFinalResult(actual)/(1.0+0.02*(fTP-25.0));
      float compensationCoefficient = 1.0+0.02*(Temp_S-25.0);
      //compensación de temperatura
      float compensationVoltage=averageVoltage/compensationCoefficient;
      
      //convierte el valor de voltaje a valor de tds
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;// el -250 es algo para la placa exclusiva
      //Serial.println(tdsValue,0);
      TDS =String(tdsValue);//transforma una cadena
    }
    Serial.print("TDS: ");
      Serial.println(TDS);
  }

  // Leer PH
  // Realizar 20 lecturas y almacenarlas en el buffer
  for (int i = 0; i < 20; i++){
    buf[i] = (4095 - analogRead(senPH)) / 73.07;
    delay(10); }
  // Encontrar el valor más repetido en el buffer
  float mostRepeatedValue = 0;
  int maxCount = 0;
  for (int i = 0; i < 20; i++){ int count = 0;
    for (int j = 0; j < 20; j++){
      if (buf[j] == buf[i]) count++;  }
    if (count > maxCount){
      maxCount = count;mostRepeatedValue = buf[i];}
  }
  // Asignar el valor más repetido a Po
  Po = mostRepeatedValue;
  // Imprimir el valor más repetido de pH
  PH =String(Po);//transforma una cadena
  Serial.print("PH: ");
  Serial.println(PH);

  //Leer Lumenes
  // Lee el valor del LDR
    int ldrValue = analogRead(LDR);
    //float lumenValue = map(ldrValue, 0, 4095, 0, 100);
    Lumenes=String(ldrValue);// transforma a una cadena
    Serial.print("Lúmenes: ");
    Serial.println(ldrValue);

}