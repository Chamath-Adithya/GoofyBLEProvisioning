#pragma once
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <BLEServer.h>

// Inherit from BLECharacteristicCallbacks to handle writes
class ProvisioningManager : public BLECharacteristicCallbacks {
public:
    void init();
    void update();
    void sendBLE(const String& message);
    bool isMotorOn();

    // BLE callback
    void onWrite(BLECharacteristic* pCharacteristic) override;

private:
    void connectWiFi();
    void startBLEProvisioning();
    void saveWiFiCredentials(const String& ssid, const String& pass);
    void handleMotorCommand(const String& jsonStr);

    bool wifiConnected = false;
    bool bleProvisioningActive = false;
    bool motorState = false;

    BLEServer* server = nullptr;
    BLECharacteristic* wifiChar = nullptr;
    BLECharacteristic* sensorChar = nullptr;
    BLECharacteristic* controlChar = nullptr;
};
