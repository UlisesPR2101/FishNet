void recibe_lora(int tamano){
    // Si el tamaño del paquete es 0, se considera como un paquete vacío
  if (tamano == 0){ 
    paqRcb_Estado = 0; //vacio
    return;
    }
    
  // lectura de paquete
  paqueteRcb = "";  // Inicializar la cadena que almacenará el paquete recibido
  dir_envio  = LoRa.read(); //Leer la dirección del dispositivo que envía
  dir_remite = LoRa.read(); //Leer la dirección del dispositivo remitente
  paqRcb_ID  = LoRa.read(); //Leer el identificador único del paquete
  
  // Leer el tamaño esperado del paquete
  byte paqRcb_Tamano = LoRa.read();

  // Leer los datos del paquete mientras estén disponibles
  while(LoRa.available()){
    paqueteRcb += (char)LoRa.read();
  }
  
  // Verificar si la longitud del paquete coincide con el tamaño esperado
  if (paqRcb_Tamano != paqueteRcb.length()){
    paqRcb_Estado = 2; // Tamaño incompleto
    return;
  }

  // Verificar si el paquete fue destinado a otro dispositivo
  if (dir_envio != dir_local){
    paqRcb_Estado = 3; // otro destino
    return;
  }

  // Verificar si el paquete es un mensaje de difusión (broadcast)
  if (dir_envio == 0xFF) {
    paqRcb_Estado = 4; //Broadcast, difusion
    return;
  }

  // Si no se cumple ninguna de las condiciones anteriores, se considera un mensaje nuevo
  paqRcb_Estado = 1;   // mensaje Nuevo
}