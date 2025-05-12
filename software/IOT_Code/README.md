# IoT Dashboard with Flask, SocketIO, and MQTT

This project creates a dashboard using Flask and SocketIO to display sensor data received from an MQTT topic. The data includes pH, temperature, salinity, and humidity readings, which are stored in an SQLite database and then displayed on the dashboard.

## Features

- **MQTT Integration**: Subscribes to an MQTT topic to receive sensor data.
- **Real-time Updates**: Uses SocketIO to update the dashboard in real-time.
- **Data Storage**: Stores sensor data in an SQLite database.
- **Dashboard**: Displays sensor data on a web-based dashboard.

## Requirements

- Python 3.10 or higher
- Flask
- Flask-SocketIO
- Paho-MQTT
- SQLite

## Installation

1. Clone the repository:

    ```sh
    git clone https://github.com/yourusername/IOT_Codes.git
    cd IOT_Codes
    ```

2. Install the required Python packages:

    ```sh
    pip install flask flask-socketio paho-mqtt sqlite3
    ```

3. Run the Flask application:

    ```sh
    python app.py
    ```

## Usage

1. Ensure your MQTT broker is running and publishing sensor data to the specified topic.
2. Start the Flask application as described in the installation steps.
3. Open your web browser and navigate to `http://localhost:5000` to view the dashboard.

## Project Structure

- `app.py`: Main application file that sets up Flask, SocketIO, and MQTT.
- `templates/`: Contains HTML templates for the dashboard.
- `static/`: Contains static files like CSS and JavaScript.
- `database.db`: SQLite database file where sensor data is stored.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgements

- Flask: [Flask Documentation](https://flask.palletsprojects.com/)
- Flask-SocketIO: [Flask-SocketIO Documentation](https://flask-socketio.readthedocs.io/)
- Paho-MQTT: [Paho-MQTT Documentation](https://www.eclipse.org/paho/)

---

Made with ❤️ by Alejandro Barrientos Escalante and ESCALIA Corporation
