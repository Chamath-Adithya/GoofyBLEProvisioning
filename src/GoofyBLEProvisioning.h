#pragma once
#include "core/ProvisioningManager.h"

class GoofyBLEProvisioning {
public:
    void begin();
    void loop();
    void sendBLE(const String& message);
    bool isMotorOn();
private:
    ProvisioningManager provisioningManager;
};
