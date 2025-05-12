import json
import sqlite3
import paho.mqtt.client as mqtt
from flask import Flask, render_template
from flask_socketio import SocketIO

# Configuración MQTT
BROKER_IP = "192.168.43.128"
TOPIC = "GsmClient/Oceano/JSON"

app = Flask(__name__)
socketio = SocketIO(app)

mqtt_connected = False

# Conectar a SQLite y crear tablas
def init_db():
    conn = sqlite3.connect("sensores.db")
    cursor = conn.cursor()
    cursor.execute("CREATE TABLE IF NOT EXISTS MQ135 (id INTEGER PRIMARY KEY, valor REAL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")
    cursor.execute("CREATE TABLE IF NOT EXISTS Temp (id INTEGER PRIMARY KEY, valor REAL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")
    cursor.execute("CREATE TABLE IF NOT EXISTS TDS (id INTEGER PRIMARY KEY, valor REAL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")
    cursor.execute("CREATE TABLE IF NOT EXISTS PH (id INTEGER PRIMARY KEY, valor REAL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")
    cursor.execute("CREATE TABLE IF NOT EXISTS LDR (id INTEGER PRIMARY KEY, valor REAL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")
    conn.commit()
    conn.close()

# Guardar datos en SQLite
def save_data(sensor, value):
    conn = sqlite3.connect("sensores.db")
    cursor = conn.cursor()
    cursor.execute(f"INSERT INTO {sensor} (valor) VALUES (?)", (value,))
    conn.commit()
    conn.close()

# Obtener datos de la base de datos
def get_data(sensor):
    conn = sqlite3.connect("sensores.db")
    cursor = conn.cursor()
    cursor.execute(f"SELECT valor FROM {sensor}")
    data = cursor.fetchall()
    conn.close()
    return [item[0] for item in data]

# Manejo de eventos MQTT
def on_connect(client, userdata, flags, rc):
    global mqtt_connected
    if rc == 0:
        mqtt_connected = True
        print("✅ Conectado a MQTT")
        client.subscribe(TOPIC)
    else:
        mqtt_connected = False
        print("❌ Error al conectar MQTT")

    socketio.emit("mqtt_status", {"status": mqtt_connected})

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode("utf-8"))
        print("📡 Datos recibidos:", data)

        save_data("MQ135", data["MQ135"])
        save_data("Temp", data["Temp"])
        save_data("TDS", data["TDS"])
        save_data("PH", data["PH"])
        save_data("LDR", data["LDR"])

        # Obtener datos actualizados para cada sensor
        mq135_data = get_data("MQ135")
        temp_data = get_data("Temp")
        tds_data = get_data("TDS")
        ph_data = get_data("PH")
        ldr_data = get_data("LDR")

        # Emitir datos a la interfaz
        socketio.emit("sensor_data", {
            "MQ135": mq135_data,
            "Temp": temp_data,
            "TDS": tds_data,
            "PH": ph_data,
            "LDR": ldr_data
        })
    except Exception as e:
        print("❌ Error procesando mensaje MQTT:", e)

# Hilo MQTT
def mqtt_thread():
    global mqtt_connected
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    try:
        client.connect(BROKER_IP, 1883, 60)
        client.loop_forever()
    except Exception as e:
        mqtt_connected = False
        print("❌ No se pudo conectar al broker MQTT:", e)
        socketio.emit("mqtt_status", {"status": mqtt_connected})

@app.route("/")
def index():
    return render_template("index.html", mqtt_connected=mqtt_connected)

if __name__ == "__main__":
    init_db()
    import threading
    threading.Thread(target=mqtt_thread, daemon=True).start()
    socketio.run(app, host="0.0.0.0", port=5000, debug=True)
