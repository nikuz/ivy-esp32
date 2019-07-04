#include <Arduino.h>

#include "def.h"
#include "Watering.h"
#include "AppTime.h"
#include "AppStorage.h"
#include "AppBlynk.h"
#include "Sensor.h"
#include "Relay.h"
#include "Tools.h"

static WateringIntVariable intVariables[10];
static WateringStringVariable stringVariables[10];
static int blankIntVariable = -1;
static String blankStringVariable = "";

bool wateringStarted = false;
struct tm wateringStartedAt = {0};
const int wateringDuration = 30;                    // 30 sec
bool wateringPassed = false;

bool valveIsOpen = false;

bool lowMoistureNotificationSent = false;
struct tm lowMoistureNotificationSentAt = {0};

Watering::Watering() {}

Watering::~Watering() {}

int &Watering::getIntVariable(const char *key) {
    const int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (intVariables[i].key == key) {
            return *intVariables[i].var;
        }
    }

    return blankIntVariable;
}

String &Watering::getStringVariable(const char *key) {
    const int varsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsLen; i++) {
        if (stringVariables[i].key == key) {
            return *stringVariables[i].var;
        }
    }

    return blankStringVariable;
}

bool isWateringEnabled() {
    return (
        Watering::getIntVariable("autoWatering") == 1
        || Watering::getIntVariable("manualWatering") == 1
    );
}

int getWateringInterval() {
    int &wateringInterval = Watering::getIntVariable("wInterval");
    return wateringInterval * 60;
}

void printLastWateringTime(double prevWateringSec) {
    Serial.print("Prev watering was only ");
    Serial.print(prevWateringSec);
    Serial.println(" sec ago.");
    Serial.print("Must be at least ");
    Serial.print(getWateringInterval());
    Serial.println(" sec.");
}

void sendNotification(int potSoilMoisture) {
    int wateringInterval = getWateringInterval();
    if (
        !lowMoistureNotificationSent
        || AppTime::compareDates(lowMoistureNotificationSentAt, AppTime::getCurrentTime()) >= wateringInterval
    ) {
        lowMoistureNotificationSent = true;
        lowMoistureNotificationSentAt = AppTime::getCurrentTime();
        AppBlynk::notify("Ivy low soil moisture: " + String(potSoilMoisture));
    }
}

// stopping (3 stage)

void stopping() {
    if (!wateringStarted) {
        return;
    }

    if (wateringPassed) {
        Watering::stop();
    }
}

// open valve (2 stage)

void valve() {
    if (
        valveIsOpen
        && AppTime::compareDates(wateringStartedAt, AppTime::getCurrentTime()) >= wateringDuration
    ) {
        AppBlynk::println("Stop watering");
        wateringPassed = true;
        return;
    }

    if (!valveIsOpen && Relay::isWateringEnabled()) {
        valveIsOpen = true;
        wateringStartedAt = AppTime::getCurrentTime();
        AppBlynk::println("Valve was open");
        return;
    }

    if (!valveIsOpen) {
        Relay::wateringOn();
        AppBlynk::println("Try to open valve");
    }
}

// soil moisture check (1 stage)

void soilMoisture() {
    int &wSoilMstrMin = Watering::getIntVariable("wSoilMstrMin");
    int wateringInterval = getWateringInterval();
    int potSoilMoisture = 0;

    if (!wateringStarted) {
        String &lastWateringTime = Watering::getStringVariable("lastWatering");
        double lastWateringSec = AppTime::compareDates(lastWateringTime, AppTime::getCurrentTime());
        if (lastWateringTime == "") {
            lastWateringSec = getWateringInterval();
        }
        if (lastWateringSec < 0) {
            lastWateringSec = 0;
        }

        potSoilMoisture = max(Sensor::getSoilMoisture(SOIL_SENSOR_1), Sensor::getSoilMoisture(SOIL_SENSOR_2));
        potSoilMoisture = max(potSoilMoisture, Sensor::getSoilMoisture(SOIL_SENSOR_3));
        if (potSoilMoisture < wSoilMstrMin) {
            if (isWateringEnabled()) {
                if (lastWateringSec >= wateringInterval) {
                    wateringStarted = true;
                } else {
                    printLastWateringTime(lastWateringSec);
                }
            } else {
                sendNotification(potSoilMoisture);
            }
        }
    }

    if (wateringStarted) {
        AppBlynk::println("Watering started");
        AppBlynk::print("Initial moisture: ");
        AppBlynk::println(potSoilMoisture);
        AppBlynk::print("Minimum must be: ");
        AppBlynk::println(wSoilMstrMin);
    }
}

// public

void Watering::setVariable(int *var, const char *key) {
    int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (!intVariables[i].key) {
            intVariables[i] = WateringIntVariable(var, key);
            break;
        }
    }
}

void Watering::setVariable(String *var, const char *key) {
    int varsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsLen; i++) {
        if (!stringVariables[i].key) {
            stringVariables[i] = WateringStringVariable(var, key);
            break;
        }
    }
}

void Watering::stop() {
    if (wateringStarted) {
        Relay::wateringOff();
        String &lastTimeWatering = Watering::getStringVariable("lastWatering");
        lastTimeWatering = AppTime::getTimeString(AppTime::getCurrentTime());
        AppStorage::putString("lastWatering", lastTimeWatering);

        // disable manual watering to do the watering only once
        int &manualWateringEnabled = Watering::getIntVariable("manualWatering");
        if (manualWateringEnabled) {
            manualWateringEnabled = 0;
            AppBlynk::postDataNoCache("manualWatering", 0);
        }

        AppBlynk::println("Watering stopped!");
    }

    valveIsOpen = false;
    wateringStarted = false;
    wateringPassed = false;
    wateringStartedAt = {0};
}

void Watering::checkProgress() {
    if ((wateringStarted && Tools::millisOverflowIsClose())) {
        Watering::stop();
        return;
    }
    if (!isWateringEnabled() || !wateringStarted) {
        return;
    }
    valve();
    stopping();
}

void Watering::check() {
    if (
        !Tools::millisOverflowIsClose()
        && !wateringStarted
    ) {
        soilMoisture();
    }
}

