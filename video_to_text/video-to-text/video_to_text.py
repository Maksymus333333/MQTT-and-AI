import time
import cv2
from ultralytics import YOLO
import paho.mqtt.client as mqtt

# MQTT
MQTT_BROKER = "fb075b16.ala.eu-central-1.emqxsl.com"
MQTT_PORT = 8883
MQTT_TOPIC = "esp/test/wiadomosc"
MQTT_USERNAME = "test"
MQTT_PASSWORD = "test"

 
mqtt_client = mqtt.Client()
mqtt_client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
mqtt_client.tls_set()   
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

 
model = YOLO("yolov8n.pt")  

 
def publish_mqtt(label):
    mqtt_client.publish(MQTT_TOPIC, label)
    print(f"Wysłano do MQTT: {label}")

 
cap = cv2.VideoCapture(0)   

if not cap.isOpened():
    print("Не вдалося відкрити камеру")
    exit()

print("Починаємо обробку відео...")

last_sent_time = 0   
last_label = None   
SEND_INTERVAL = 5   

while True:
    ret, frame = cap.read()
    if not ret:
        break

  
    results = model(frame)
    detected_labels = []  

    for result in results:
        for box in result.boxes:
            cls = int(box.cls[0])   
            label = model.names[cls]   
            detected_labels.append(label)

         
            x1, y1, x2, y2 = map(int, box.xyxy[0])   
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(frame, label, (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

   
    if detected_labels:
        current_time = time.time()
        if detected_labels[0] != last_label or (current_time - last_sent_time) > SEND_INTERVAL:
            last_label = detected_labels[0]  
            last_sent_time = current_time   
            publish_mqtt(last_label)

   
    cv2.imshow("YOLO Object Detection", frame)

     
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()



 