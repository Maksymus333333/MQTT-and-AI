# IoT System with ESP8266, MQTT, Speech-to-Text, and Video-to-Text

This project combines an IoT system with ESP8266 microcontrollers and AI-driven functionalities like speech-to-text and video-to-text. The system uses MQTT for communication between components. Below is a detailed explanation of each module.

 ## 1. ESP8266 - Displaying Temperature on LCD
Description
This project uses ESP8266 to read temperature data from a DS18B20 sensor and displays it on an LCD screen.

Key Components:
- Microcontroller: ESP8266.
- Temperature Sensor: DS18B20.
- Display: LCD (I2C).
  
How to Use:
1) Upload the Firmware:

- Open the code in Arduino IDE.
- Connect the ESP8266 to your computer.
- Upload the firmware to the microcontroller.
2) Connect Hardware:

- Connect the DS18B20 to GPIO 2 on the ESP8266.
- Connect the LCD screen via the I2C interface.
3) Run the Device:

- Power on the device.
- The current temperature will be displayed on the LCD.
  
Key Features:
- Reads the temperature every 10 seconds.
- Displays the temperature in real-time on the LCD.



## 2) Subscriber - Sending Temperature via MQTT to InfluxDB and Grafana
     
Description
This project uses ESP8266 to measure temperature using the DS18B20 sensor and sends the data via MQTT to a broker. The data is stored in InfluxDB and visualized in Grafana.

Key Components:
- Microcontroller: ESP8266.
- Temperature Sensor: DS18B20.
- MQTT Broker: EMQX.
- InfluxDB: For data storage.
- Grafana: For data visualization.

How to Use:
1. Setup ESP8266:

- Load the code in Arduino IDE.
- Configure Wi-Fi, MQTT broker credentials, and connect the DS18B20 sensor.
2. Set Up the Server:
- Use the docker-compose.yml file to start InfluxDB and Grafana
- Start the services with: docker-compose up
3. Configure Grafana:
- Add InfluxDB as a data source in Grafana.
- Create a dashboard to visualize temperature data.

Key Features:
- Sends temperature data to MQTT.
- Stores data in InfluxDB.
- Displays temperature trends in Grafana.

## 3) Speech-to-Text

Description

This project uses the Whisper model to recognize spoken Polish text and sends the transcribed text via MQTT to ESP8266, which displays it on the top line of an LCD.

Key Components:
- Model: Whisper (OpenAI).
- MQTT Broker: EMQX.
- Microcontroller: ESP8266.
How to Use:
1. Set Up Python Environment
2. Run the Program
3. Set Up ESP8266

Key Features:
- Recognizes Polish speech in real-time.
- Sends transcribed text via MQTT.
- Displays text on the LCD screen.

## 4) Video-to-Text

Description

This project processes video streams using the YOLO model, identifies objects in the video, and sends their names via MQTT to ESP8266. The names are displayed on the bottom line of an LCD.

Key Components:
- Model: YOLO (object detection).
- MQTT Broker: EMQX.
- Microcontroller: ESP8266.

How to Use:
1. Set Up Python Environment
2. Run the Program
3. Set Up ESP8266

Key Features:
- Detects objects in real-time video streams.
- Sends detected object names via MQTT.
- Displays text on the LCD screen.

## General Instructions:
1. Install Docker:
Use Docker for projects involving InfluxDB and Grafana.

2. Set Up MQTT Broker:
Ensure the MQTT broker (EMQX) is configured and running for all projects.

3. ESP8266:
For ESP8266, you need Arduino IDE and libraries:

- PubSubClient (for MQTT).
- LiquidCrystal_PCF8574 (for LCD).
- DallasTemperature and OneWire (for DS18B20).


