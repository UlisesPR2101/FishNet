# **FishNet: Monitoreo de acuiferos**

## Descripción 📦

El proyecto "FishNet" consiste en el diseño, desarrollo e implementación de un prototipo para el monitoreo y control en tiempo real de sistemas de acuicultura destinados a la producción de tilapia. Este innovador sistema integra tecnología avanzada y el Internet de las Cosas (IoT) para abordar los serios desafíos que enfrenta la industria, especialmente en regiones como Tezontepec de Aldama, Hidalgo, donde el monitoreo ineficaz de parámetros críticos del agua pone en riesgo los cultivos y afecta la rentabilidad.

"FishNet" utiliza sensores de alta precisión para medir una variedad de parámetros esenciales, incluyendo temperatura, pH, sólidos disueltos totales, luminescencia y gases evaporados nocivos. Estos datos son transmitidos eficientemente a través del protocolo GSM, permitiendo su procesamiento y visualización en tiempo real mediante la plataforma Node-RED. Esto capacita a los productores para realizar ajustes oportunos y optimizar el entorno de cultivo, mejorando así la calidad del agua y la salud de los peces.

El prototipo combina microcontroladores, sensores y algoritmos diseñados para capturar datos con alta precisión. Utiliza herramientas de desarrollo como C++ para la programación y Python para la visualización, lo que ofrece una solución costo-efectiva especialmente dirigida a pequeños y medianos productores

## Visuales 📷
Cambiar visuales
![Zabbix Dashboard](https://github.com/UlisesPR2101/FishNet/blob/main/Imagenes/zabbix-dashboard.jpg)
![Zabbix Dashboard](https://github.com/UlisesPR2101/FishNet/blob/main/diagramas/conection-Diagram-liygo.png)
![Zabbix Dashboard](https://github.com/UlisesPR2101/FishNet/blob/main/diagramas/conection-diagram.png)

## Empezando 🚀

Por ahora no hay alguna forma de tener nuestro proyecto ya que se encuentra alojado en una maquina virtual, proximamente en un contenedor para el acceso para todos

## Requisitos 📋

### Software
Lista de software y herramientas, incluyendo versiones, que necesitas para instalar y ejecutar este proyecto:

- Sistema Operativo: Linux nativo o de forma virtualizada
- Base de datos: InfluxDB
- //////Dasboar usado: Frontend, server y agente 2 (version 6)
- //////PHP (version 8)
- Arduino IDE
- Librerias de arduino usadas:
  - SoftwareSerial
  - PubSubClient
  - OneWire
  - DallasTemperature
  - TinyGsmClient

### Hardware 

- Placa lilygo t-sim7000g
- Raspberry Placa o de Forma virtualizada
- Sensor Ambiental MQ135
- Sensor de CQRobot TDS
- Sensor PH-4502C Sensor de PH Liquido con electrodo E201-BNC
- Sensor de Temperatura 18B20
- Sensor de Luz LDR


## Instalación 🔧

### Instalación en lilygo t-sim7000g:

Dependencias:
- Sistema operativo Arduino IDE
- Placa lilygo t-sim7000g
- Cable USB

Pasos:
1.- Descargue e instale el Arduino IDE: Descargue e instale la última versión del Arduino IDE desde el sitio web oficial:       
    
    https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE.
  
2.- Conecte la placa lilygo t-sim7000g: Conecte la placa lilygo t-sim7000g a su computadora usando un cable USB.
  
3.- Seleccione la placa y el puerto: En el Arduino IDE, seleccione la placa "ESP32 Dev Module" y el puerto correspondiente a la conexión USB.
  
4.- Descargue el código del proyecto: Descargue el código fuente del proyecto para lilygo t-sim7000g.
  
5.- Abra el código en Arduino IDE: Abra el código descargado en el Arduino IDE.
  
6.- Compile y cargue el código: Compile el código y cárguelo en la placa lilygo t-sim7000g presionando el botón "Subir".

### Instalación en Raspberry Pi:

Dependencias:
- Sistema operativo Raspbian
- Raspberry Pi
- Cable Ethernet o WiFi

Pasos:

1.- Descargue e instale Raspbian: Descargue e instale la última versión de Raspbian en su Raspberry Pi. Puede encontrar instrucciones detalladas en el sitio web oficial de Raspberry Pi:
  
    https://www.raspberrypi.com/software/.

2.- Actualice el sistema: Abra una terminal en su Raspberry Pi y ejecute el siguiente comando para actualizar el sistema:
```bash
sudo apt update && sudo apt upgrade
```

3.- Instale las dependencias necesarias: Instale las dependencias necesarias para ejecutar el software del proyecto usando el siguiente comando:
```bash
sudo apt install zabbix-agent2
```

## Configuracion /////Cambiar configuracion⚙️

Servidor Zabbix:
Agente Zabbix:

1.- Configure el agente Zabbix para comunicarse con el servidor Zabbix. Esto generalmente se realiza editando el archivo de configuración del agente (zabbix_agentd.conf).

Servidor Zabbix:

1.- Habilitar plugin MQTT: En el servidor Zabbix, habilite el plugin MQTT en la configuración del servidor (zabbix_server.conf).

2,- Crear Elementos (Items): Cree elementos en Zabbix para representar los datos que desea monitorear de los dispositivos IoT. Configure la clave del elemento para utilizar la función mqttavailable o mqtt.get.

- mqttavailable: verifica la disponibilidad de un tópico MQTT específico.
- mqtt.get: recupera el valor de un tópico MQTT específico.

```zabbix agent configuration file
# Define el nombre del elemento
Hostname: MqttSensor1

# Define la clave del elemento para usar la función mqtt.get
Type: Zabbix Agent ("mqtt.get")

# Define el ID del host asociado al dispositivo IoT
Key: mqtt.sensor1/temperature

# Opcional: define el nombre del ítem para mostrarse en la interfaz
Name: Sensor 1 Temperature (MQTT)

# Opcional: define las unidades de medida del dato
Units: °C

# Opcional: define el intervalo de recolección de datos
Delay: 30s
Crear Hosts: Cree hosts en Zabbix para representar los dispositivos IoT. Asigne los elementos creados anteriormente a cada host correspondiente.
```

## Inalambrica 🛜
El proyecto utiliza la tecnología GSM (Global System for Mobile Communications) para la comunicación inalámbrica entre los sensores del acuífero FishNet y el servidor Hiven MQ. GSM es un estándar de comunicación móvil que permite la transmisión de datos a través de redes celulares, proporcionando conectividad de largo alcance y un consumo relativamente bajo de energía, ideal para aplicaciones de Internet de las Cosas (IoT). Esta tecnología permite la transmisión eficiente y en tiempo real de los datos críticos del acuífero, asegurando un monitoreo continuo y optimización del proceso de producción de tilapia.

#### Dispositivos de Red Inalámbrica:
- Sensores: Los sensores instalados en el biodigestor estarán equipados con módulos LoRaWAN para transmitir datos al gateway.
- Gateway: Un gateway LoRaWAN actuará como punto de acceso para los sensores y será responsable de la comunicación entre los sensores y el servidor MQTT.
- Servidor MQTT: Un servidor MQTT recibirá los datos de los sensores a través del gateway y los almacenará para su análisis y visualización.
Configuración de la Red:

#### Configuración de la Red:
- Sensores: Cada sensor LoRaWAN tendrá una dirección única que lo identificará en la red. Los sensores se configurarán para transmitir datos a intervalos regulares, por ejemplo, cada minuto.
- Gateway: El gateway LoRaWAN se configurará para recibir transmisiones de los sensores y reenviar los datos al servidor MQTT. El gateway también se configurará para unirse a la red LoRaWAN local.
- Servidor MQTT: El servidor MQTT se configurará para escuchar las transmisiones del gateway y almacenar los datos recibidos en una base de datos.

*El router puede utilizarse como punto de acceso para proporcionar conectividad a Internet al servidor MQTT. Esto permitirá que los usuarios accedan a los datos del biodigestor desde cualquier lugar a través de Internet.*

## Funcionalidades 🗒️

- Monitoreo de Sensores: El prototipo recopilará datos en tiempo real de los sensores instalados en el biodigestor
- Control de Parámetros: Basándose en los datos de los sensores, el software ajustará automáticamente parámetros clave del biodigestor
- Visualización de Datos: El software presentará los datos de los sensores y los parámetros operativos en una interfaz gráfica. Esto permitirá al usuario visualizar el estado del biodigestor en tiempo real y detectar cualquier anomalía o problema potencial.
- Alertas y Notificaciones: El software generará alertas y notificaciones cuando se detecte alguna condición que pueda afectar el funcionamiento del biodigestor, como niveles de temperatura o humedad fuera del rango ideal.
- Registro de Datos: El software registrará históricamente todos los datos de los sensores y los parámetros operativos. Esta información podrá ser utilizada para analizar el rendimiento del biodigestor, identificar patrones y optimizar aún más el proceso.

### Interacción con otros Sistemas
- Actuadores: El software controlará los actuadores del biodigestor, como el sistema de calentamiento, el sistema de mezclado y las válvulas de alimentación, mediante señales de control apropiadas.
- SCADA (Supervisory Control and Data Acquisition): El software podrá integrarse con un sistema SCADA para la supervisión y control remoto del biodigestor.

### Escenarios de uso

### Casos de uso
**Monitoreo de Parámetros Clave**
El sistema recopilará datos en tiempo real de los sensores instalados en el biodigestor, incluyendo:
- Temperatura
- Nivel de humedad
- Concentración de CO2
- Concentración de metano
- El sistema almacenará los datos recopilados en una base de datos.

Respuestas Esperadas:
- El usuario podrá visualizar los datos de los sensores en tiempo real en una interfaz gráfica.
- El sistema generará alertas si alguno de los parámetros supera los límites establecidos.
- El sistema registrará los datos históricos para su análisis posterior.

## Contribuyendo 🖇️

Las contribuciones son lo que hacen a la comunidad de código abierto un lugar increíble para aprender, inspirar y crear. Cualquier contribución que hagas es muy apreciada.

## Roadmap

Posibles implementaciones al proyecto

- Implementar automatizacion al proyecto
- Agregar mas sensores para el monitoreo
- Optimizacion de codigo de la placa
- Implementacion de big data para el analisis de datos
- implementacion de computo en la nube

## Autores ✒️

- **Alejandro Barrientos Escalante** - _Trabajo inicial_ - [Alejandro Barrientos](https://github.com/beofalejandro)
- **Ulises Porras Rosas** - _Trabajo inicial_ - [Ulises Porras](https://github.com/UlisesPR2101)
- **Elvis Jesus Martinez Lugo** - _Trabajo inicial_ - [Elvis Lugo](https://github.com/Elvis-Lugo)
- **Elizabeth Diaz Oropeza** - _Trabajo inicial_ - [Elizabeth Diaz](https://github.com/Elizabeth-Diaz-Oropeza)

Mira también la lista de [contribuidores]() que han participado en este proyecto.

## Expresiones de Gratitud 🎁

Estamos agradecidos por las contribuciones de la comunidad a este proyecto. Si encontraste cualquier valor en este proyecto o quieres contribuir, aquí está lo que puedes hacer:

- Comparte este proyecto con otros
- Muestra tu agradecimiento diciendo gracias en un nuevo problema.
- sigue al tarabjo de nuestros autores

## Licencia y Términos de Uso 📄
Licencia:
El proyecto se licenciará bajo una licencia de código abierto compatible con las directrices de colaboración abierta de Bionova. Esto permitirá a los usuarios y colaboradores utilizar, modificar libremente, sujeto a los términos de la licencia elegida.

Términos de Uso:
Al utilizar o contribuir al proyecto, los usuarios y colaboradores aceptan los siguientes términos:

#### Contribuciones:
- Las contribuciones al proyecto deben hacerse de buena fe y estar libres de plagio o código de terceros no autorizado.
- Las contribuciones deben estar adecuadamente documentadas y revisadas por otros colaboradores antes de ser integradas al proyecto.
- Los colaboradores deben otorgar una licencia de código abierto compatible con la licencia del proyecto para sus contribuciones.
#### Uso del Software:
- Se debe respetar la licencia del software y no se debe eliminar ni modificar los avisos de copyright o licencia.
- El uso del software no debe dañar ni poner en riesgo a terceros.
#### Responsabilidad:
- Los desarrolladores del proyecto no son responsables de ningún daño o pérdida que pueda surgir del uso del software.
- Los usuarios y colaboradores deben utilizar el software de manera responsable y bajo su propio riesgo.

---
Hecha con ❤️ por **ESCALIA Studios**
