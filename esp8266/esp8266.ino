#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <OneWire.h>
#include <DallasTemperature.h>

 
LiquidCrystal_PCF8574 lcd(0x27);

 
#define ONE_WIRE_BUS 2  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  lcd.begin(16, 2);  
  lcd.setBacklight(255); 
  sensors.begin();  
  Serial.begin(9600); 
}

void loop() { 
  lcd.setCursor(0, 0);  
  
 
  sensors.requestTemperatures();  
  float temperature = sensors.getTempCByIndex(0); 
  
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print(temperature);   
  lcd.print(" C");  

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  delay(2000); 
}