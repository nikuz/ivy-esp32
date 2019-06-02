#include <Arduino.h>

#include "def.h"
#include "Relay.h"
#include "AppSerial.h"
#include "Sensor.h"
#include "AppTime.h"

const char relayOnSerialCommand[] = "rOn";
const char relayOffSerialCommand[] = "rOf";

// watering
bool wateringEnabled = false;
bool wateringOpenedValve = false;

Relay::Relay() {}

Relay::~Relay() {}

void Relay::parseSerialCommand(const char *command, const char *param) {
    if (strcmp(command, "rOn") == 0) {
        if (strcmp(param, "valve") == 0) {
            wateringOpenedValve = true;
            DEBUG_PRINTLN("Open valve s1.");
        }
    } else if (strcmp(command, "rOf") == 0) {
        if (strcmp(param, "valve") == 0) {
            wateringOpenedValve = false;
            DEBUG_PRINTLN("Close valve s1.");
        }
    }
}

// watering

bool Relay::isWateringOn() {
    return wateringEnabled;
}

bool Relay::wateringValveIsOpen() {
    return wateringOpenedValve;
}

void Relay::wateringOpenValve() {
    SerialFrame openValveFrame = SerialFrame(relayOnSerialCommand, "1");
    AppSerial::sendFrame(&openValveFrame);
}

void Relay::wateringCloseValve() {
    SerialFrame closeValveFrame = SerialFrame(relayOffSerialCommand, "1");
    AppSerial::sendFrame(&closeValveFrame);
}
