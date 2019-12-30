#include <Arduino.h>

#include "def.h"
#include "Heating.h"
#include "Sensor.h"
#include "Relay.h"
#include "Tools.h"
#include "AppTime.h"
#include "AppStorage.h"
#include "AppBlynk.h"

static HeatingIntVariable intVariables[10];
static HeatingStringVariable stringVariables[10];
static int blankIntVariable = -1;
static String blankStringVariable = "";

bool heatingStarted = false;

Heating::Heating() {}

Heating::~Heating() {}

bool isHeatingEnabled() {
    return Heating::getIntVariable("autoHeating") == 1;
}

void start() {
    Relay::heatingOn();

    heatingStarted = true;
    AppBlynk::print("Ivy low temperature: ");
    AppBlynk::println(AppTime::getTimeString(AppTime::getCurrentTime()));
}

// public

void Heating::setVariable(int *var, const char *key) {
    int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (!intVariables[i].key) {
            intVariables[i] = HeatingIntVariable(var, key);
            break;
        }
    }
}

void Heating::setVariable(String *var, const char *key) {
    int varsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsLen; i++) {
        if (!stringVariables[i].key) {
            stringVariables[i] = HeatingStringVariable(var, key);
            break;
        }
    }
}

int &Heating::getIntVariable(const char *key) {
    const int varsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsLen; i++) {
        if (intVariables[i].key == key) {
            return *intVariables[i].var;
        }
    }

    return blankIntVariable;
}

String &Heating::getStringVariable(const char *key) {
    const int varsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsLen; i++) {
        if (stringVariables[i].key == key) {
            return *stringVariables[i].var;
        }
    }

    return blankStringVariable;
}

void Heating::stop() {
    Relay::heatingOff();

    heatingStarted = false;
    AppBlynk::print("Heating stop: ");
    AppBlynk::println(AppTime::getTimeString(AppTime::getCurrentTime()));
    String &lastTimeHeating = Heating::getStringVariable("lastHeating");
    lastTimeHeating = AppTime::getTimeString(AppTime::getCurrentTime());
    AppStorage::putString("lastHeating", lastTimeHeating);
}

void Heating::check() {
    const int soilTemperature1 = Sensor::getSoilTemperature(SOIL_SENSOR_1);
    const int soilTemperature2 = Sensor::getSoilTemperature(SOIL_SENSOR_2);
    const int soilTemperature3 = Sensor::getSoilTemperature(SOIL_SENSOR_3);
    const int min = Heating::getIntVariable("hSoilTmpMin");
    const int max = Heating::getIntVariable("hSoilTmpMax");

    if (!Tools::millisOverflowIsClose() && isHeatingEnabled()) {
        if (
            !heatingStarted
            && (
                soilTemperature1 <= min
                || soilTemperature2 <= min
                || soilTemperature3 <= min
            )
        ) {
            start();
        } else if (
            heatingStarted
            && (
                soilTemperature1 >= max
                && soilTemperature2 >= max
                && soilTemperature3 >= max
            )
        ) {
            Heating::stop();
        }
    }
}

