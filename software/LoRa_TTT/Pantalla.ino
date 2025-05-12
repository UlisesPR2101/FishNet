void pantalla_Mostrar(){
  //Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  //Heltec.display->setFont(ArialMT_Plain_10);
  
  Heltec.display->drawString(1, 0, "Number of pack sent: "+ String(id_msjLoRa-1));
  //Heltec.display->setFont(ArialMT_Plain_11);
  Heltec.display->drawString(7, 10, "* Hum: " + humedad +" %");
  Heltec.display->drawString(7, 20, "* CO: " + Gas_C +" ppm");
  Heltec.display->drawString(7, 30, "* Metano: " + Gas_M +" ppm");
  Heltec.display->drawString(7, 40, "* Temp: " + Tem_Cel +" ºC");
  Heltec.display->drawString(7, 50, "* Batt: " + battNivel +"%");
  Heltec.display ->drawString(80, 50, "| D.L: " + String(dir_local, HEX));
  //Heltec.display->drawString(0, 50, "dir Local: " + String(dir_local, HEX));
  //Heltec.display->drawString(2, 50, "RSSI: " + String(rssi_lora)+ " dBm");
  //Heltec.display->drawString(68, 50, "S_R: " + String(snr_lora)+ " dBm");
  //Heltec.display -> display();
}