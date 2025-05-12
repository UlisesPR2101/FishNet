from flask import Flask as fk, render_template as rt, request as rq
from flask_socketio import SocketIO
import threading as threating
from time import strftime, sleep
import paho.mqtt.client as mqtt
import sqlite3
import time
import json
import datetime

conexion = sqlite3.connect('datos.db')
app = fk(__name__)
socketio = SocketIO(app)

cursor = conexion.cursor()

cursor.execute('''
    CREATE TABLE IF NOT EXISTS mediciones (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        fecha TEXT,
        valor REAL
    )
''')

@app.route('/')
def index():
    return rt('index.html')


@app.route('/iraprueba')
def prueba():
    return rt('prueba.html')


@app.route('/activador')
def activador():
    dtnum = rt.form('device')

    return rt('index.html', salida=dtnum)



def guardar_medicion(valor):
    conexion = sqlite3.connect('datos.db')
    cursor = conexion.cursor()
    fecha = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    cursor.execute('INSERT INTO mediciones (fecha, valor) VALUES (?, ?)', (fecha, valor))
    conexion.commit()
    conexion.close()

# Ejemplo de uso
guardar_medicion(25.5)

def generate_data():
    return


if __name__ == '__main__':
    thread = threating.Thread(target=generate_data)
    thread.daemon = True
    thread.start()
    socketio.run(app, host="0.0.0.0", port=5001, debug=True, allow_unsafe_werkzeug=True)
