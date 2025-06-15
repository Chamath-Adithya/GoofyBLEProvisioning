#include "ProvisioningManager.h"
#include "../config/Config.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <BLEServer.h>

void ProvisioningManager::init() {
    EEPROM.begin(EEPROM_SIZE);
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(MOTOR_PIN, LOW);
    connectWiFi();
}

void ProvisioningManager::update() {
    if (!wifiConnected && !bleProvisioningActive) {
        startBLEProvisioning();
    }
    // BLE runs in callback, no periodic tasks needed here
}

void ProvisioningManager::connectWiFi() {
    String ssid = EEPROM.readString(0);
    String pass = EEPROM.readString(32);
    if (ssid.length() == 0) return;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT_MS) {
        delay(100);
    }
    wifiConnected = (WiFi.status() == WL_CONNECTED);
}

void ProvisioningManager::startBLEProvisioning() {
    BLEDevice::init("ESP32_BLE_Provisioner");
    server = BLEDevice::createServer();
    BLEService* service = server->createService(SERVICE_UUID);

    // WiFi credentials characteristic
    wifiChar = service->createCharacteristic(CHAR_UUID_WIFI, BLECharacteristic::PROPERTY_WRITE);
    wifiChar->setCallbacks(this);

    // Sensor data notification characteristic
    sensorChar = service->createCharacteristic(CHAR_UUID_SENSOR, BLECharacteristic::PROPERTY_NOTIFY);
    sensorChar->addDescriptor(new BLE2902());

    // Motor control characteristic
    controlChar = service->createCharacteristic(CHAR_UUID_CONTROL, BLECharacteristic::PROPERTY_WRITE);
    controlChar->setCallbacks(this);

    service->start();
    server->getAdvertising()->start();
    bleProvisioningActive = true;
    Serial.println("📶 BLE Provisioning Mode Started");
}

void ProvisioningManager::onWrite(BLECharacteristic* pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();
    if (pCharacteristic == wifiChar) {
        DynamicJsonDocument doc(256);
        auto err = deserializeJson(doc, value);
        if (!err) {
            String ssid = doc["ssid"].as<String>();
            String pass = doc["pass"].as<String>();
            saveWiFiCredentials(ssid, pass);
            Serial.println("💾 WiFi creds saved, restarting...");
            ESP.restart();
        }
    } else if (pCharacteristic == controlChar) {
        Serial.println("📥 Motor command: " + value);
        handleMotorCommand(value);
    }
}

void ProvisioningManager::saveWiFiCredentials(const String& ssid, const String& pass) {
    EEPROM.writeString(0, ssid);
    EEPROM.writeString(32, pass);
    EEPROM.commit();
}

void ProvisioningManager::handleMotorCommand(const String& jsonStr) {
    DynamicJsonDocument doc(128);
    auto err = deserializeJson(doc, jsonStr);
    if (err) {
        Serial.println("❌ Invalid JSON");
        return;
    }
    if (doc.containsKey("motor")) {
        String cmd = doc["motor"].as<String>();
        if (cmd == "ON") {
            digitalWrite(MOTOR_PIN, HIGH);
            motorState = true;
            Serial.println("⚙️ Motor ON");
        } else if (cmd == "OFF") {
            digitalWrite(MOTOR_PIN, LOW);
            motorState = false;
            Serial.println("🛑 Motor OFF");
        }
    }
}

bool ProvisioningManager::isMotorOn() {
    return motorState;
}

void ProvisioningManager::sendBLE(const String& message) {
    if (sensorChar) {
        sensorChar->setValue(message.c_str());
        sensorChar->notify();
    }
}
