import whisper
import paho.mqtt.client as mqtt
import sounddevice as sd
import numpy as np

# MQTT налаштування
MQTT_BROKER = "fb075b16.ala.eu-central-1.emqxsl.com"
MQTT_PORT = 8883
MQTT_TOPIC = "esp/test/wiadomosc"
MQTT_USERNAME = "test"
MQTT_PASSWORD = "test"

# Налаштування Whisper
model = whisper.load_model("base")  # Використовуйте "base", "small", "medium", "large" за потребою

# Налаштування MQTT клієнта
client = mqtt.Client()
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
client.tls_set()

def record_audio(duration=5, samplerate=16000):
    """Запис аудіо з мікрофона"""
    print("Recording...")
    audio = sd.rec(int(duration * samplerate), samplerate=samplerate, channels=1, dtype='float32')
    sd.wait()  # Очікуємо завершення запису
    print("Recording completed.")
    return np.squeeze(audio)

def transcribe_and_send():
    """Записує аудіо, розпізнає текст та надсилає через MQTT"""
    audio = record_audio()
    print("Transcribing...")
    result = model.transcribe(audio, fp16=False)
    text = result["text"]
    print(f"Recognized text: {text}")

    # Відправка через MQTT
    client.connect(MQTT_BROKER, MQTT_PORT)
    client.publish(MQTT_TOPIC, text)
    print("Text sent via MQTT.")

if __name__ == "__main__":
    try:
        print("Starting speech-to-text...")
        while True:
            transcribe_and_send()
    except KeyboardInterrupt:
        print("Program stopped.")
