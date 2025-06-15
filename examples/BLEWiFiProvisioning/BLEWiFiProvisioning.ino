#include <GoofyBLEProvisioning.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 27
#define DHTTYPE DHT22

GoofyBLEProvisioning provisioner;
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  provisioner.begin();
}

void loop() {
  provisioner.loop();
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    DynamicJsonDocument doc(256);
    doc["device_id"] = "ESP32_001";
    JsonObject sensors = doc.createNestedObject("sensors");
    sensors["temperature"] = isnan(t) ? -1.0 : t;
    sensors["humidity"] = isnan(h) ? -1.0 : h;
    doc["motor_status"] = provisioner.isMotorOn() ? "ON" : "OFF";
    String json;
    serializeJson(doc, json);
    Serial.println("📤 Sending: " + json);
    provisioner.sendBLE(json);
  }
}
