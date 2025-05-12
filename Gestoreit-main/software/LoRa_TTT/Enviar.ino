void envia_lora(byte destino, byte remite, byte paqueteID, String paquete) {

  // Espera a que el módulo LoRa esté listo para enviar un paquete
  while (LoRa.beginPacket() == 0) {
    if (serial_msj == true) {
      Serial.println("Esperando radio disponible...");
    }
    yield(); // procesa wifi
    delay(100);
  }

  /*
  LoRa.print() se utiliza para enviar datos en formato de texto. Acepta una cadena de caracteres (String),
  un arreglo de caracteres (char array) o un número como argumento. Los datos se envían en forma de caracteres ASCII.
  LoRa.write() se utiliza para enviar datos en forma de bytes. Acepta un arreglo de bytes (byte array)
  como argumento y envía cada byte individualmente.
  */

  // Iniciar el paquete LoRa
  LoRa.beginPacket();
  // Escribir los datos en el paquete LoRa
  LoRa.write(destino);           //disp de destino
  LoRa.write(remite);            //disp que envia
  LoRa.write(paqueteID);         //Id de paquete unico
  LoRa.write(paquete.length());  //Longitud de datos
  LoRa.print(paquete);           //Escribir en paquete y envia 
  //Serial.println(paquete);

  // Finalizar y enviar el paquete LoRa
  LoRa.endPacket();
}