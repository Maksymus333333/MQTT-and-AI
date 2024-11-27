import paho.mqtt.client as mqtt
import json
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS
import time

# Налаштування InfluxDB
INFLUXDB_URL = "http://localhost:8086"
INFLUXDB_TOKEN = "your-super-secret-token"  # Використовуйте той самий токен, що в docker-compose.yml
INFLUXDB_ORG = "myorg"
INFLUXDB_BUCKET = "temperature_data"

# Налаштування MQTT
MQTT_BROKER = "fb075b16.ala.eu-central-1.emqxsl.com"
MQTT_PORT = 8883
MQTT_TOPIC = "esp/test/wiadomosc"
MQTT_USERNAME = "test"
MQTT_PASSWORD = "test"

# Ініціалізація клієнта InfluxDB
influx_client = InfluxDBClient(
    url=INFLUXDB_URL,
    token=INFLUXDB_TOKEN,
    org=INFLUXDB_ORG
)
write_api = influx_client.write_api(write_options=SYNCHRONOUS)

def on_connect(client, userdata, flags, rc):
    print(f"Підключено до MQTT брокера з кодом: {rc}")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        # Парсинг JSON повідомлення
        payload = json.loads(msg.payload.decode())
        temperature = payload.get('temperature')
        
        if temperature is not None:
            # Створення точки даних для InfluxDB з тегами
            point = Point("temperature_measurement") \
                .tag("sensor_id", "ESP8266_1") \
                .tag("location", "room") \
                .field("temperature", float(temperature)) \
                .time(time.time_ns())
            
            # Запис даних в InfluxDB
            write_api.write(bucket=INFLUXDB_BUCKET, record=point)
            print(f"Записано температуру: {temperature}°C")
            
            # Перевірка запису даних
            query = f'from(bucket:"{INFLUXDB_BUCKET}") |> range(start: -1m)'
            result = influx_client.query_api().query(query)
            print(f"Дані в базі: {result}")
            
    except Exception as e:
        print(f"Помилка обробки повідомлення: {e}")

# Налаштування MQTT клієнта
client = mqtt.Client()
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
client.tls_set()  # Додаємо SSL/TLS підключення
client.on_connect = on_connect
client.on_message = on_message

try:
    # Підключення до MQTT брокера
    print("Підключення до MQTT брокера...")
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    print("Підключено до MQTT брокера")
    
    # Запуск головного циклу
    client.loop_forever()
except Exception as e:
    print(f"Помилка підключення: {e}")