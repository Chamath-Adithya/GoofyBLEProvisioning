#include "GoofyBLEProvisioning.h"

void GoofyBLEProvisioning::begin() {
    provisioningManager.init();
}

void GoofyBLEProvisioning::loop() {
    provisioningManager.update();
}

void GoofyBLEProvisioning::sendBLE(const String& message) {
    provisioningManager.sendBLE(message);
}

bool GoofyBLEProvisioning::isMotorOn() {
    return provisioningManager.isMotorOn();
}
