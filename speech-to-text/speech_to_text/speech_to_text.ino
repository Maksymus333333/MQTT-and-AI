#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_PCF8574.h>
#include <OneWire.h>
#include <time.h> 
#include <Wire.h>

//  Wi-Fi
const char *ssid = "Max";                     //  SSID
const char *password = "1234567890";      

// Налаштування MQTT
const int mqtt_port = 8883;                    // port SSL/TLS
const char *mqtt_broker = "fb075b16.ala.eu-central-1.emqxsl.com";  // address EMQX
const char *mqtt_topic = "esp/test/wiadomosc";  // topic MQTT
const char *mqtt_username = "test";          // user EMQX
const char *mqtt_password = "test";          

//   NTP
const char *ntp_server = "pool.ntp.org";       // Serwer NTP
const long gmt_offset_sec = 0;                 
const int daylight_offset_sec = 0;              

//   LCD 
LiquidCrystal_PCF8574 lcd(0x27);  
 

 
BearSSL::WiFiClientSecure espClient;
PubSubClient mqtt_client(espClient);

 
static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";

 
void connectToWiFi();
void connectToMQTT();
void syncTime();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
    Serial.begin(9600); 
   
    lcd.begin(16, 2);  
    lcd.setBacklight(255);
    lcd.print("Initializing...");

    connectToWiFi();
    syncTime(); 
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTT();

    lcd.clear();
    lcd.print("Ready.");
    Serial.println("Setup complete.");
}

void syncTime() {
    configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
    Serial.print("Czekam na synchronizację czasu NTP: ");
    while (time(nullptr) < 8 * 3600 * 2) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Czas zsynchronizowany");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.print("Aktualny czas: ");
        Serial.println(asctime(&timeinfo));
    } else {
        Serial.println("Nie udało się uzyskać lokalnego czasu");
    }
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Connected to Wi-Fi");
}

void connectToMQTT() {
    BearSSL::X509List serverTrustedCA(ca_cert);
    espClient.setTrustAnchors(&serverTrustedCA);
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT broker as %s...\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
        } else {
            char err_buf[128];
            espClient.getLastSSLError(err_buf, sizeof(err_buf));
            Serial.printf("Failed to connect, rc=%d\nSSL Error: %s\n", mqtt_client.state(), err_buf);
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);

  
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("Message: " + message);

    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Message:");
    lcd.setCursor(0, 1);
    lcd.print(message);
}

void loop() {
    if (!mqtt_client.connected()) {
        connectToMQTT();
    }
    mqtt_client.loop();
}
