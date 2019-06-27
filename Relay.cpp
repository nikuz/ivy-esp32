#include <Arduino.h>

#include "def.h"
#include "Relay.h"
#include "AppSerial.h"
#include "Sensor.h"
#include "AppTime.h"

const char relayOnSerialCommand[] = "rOn";
const char relayOffSerialCommand[] = "rOf";

// heating
bool heatingEnabled = false;

// watering
bool wateringEnabled = false;

Relay::Relay() {}

Relay::~Relay() {}

void Relay::parseSerialCommand(const char *command, const char *param) {
    if (strcmp(command, "rOn") == 0) {
        if (strcmp(param, "valve") == 0) {
            wateringEnabled = true;
            DEBUG_PRINTLN("Valve opened.");
        }
        if (strcmp(param, "heat") == 0) {
            heatingEnabled = true;
            DEBUG_PRINTLN("Heat enabled.");
        }
    } else if (strcmp(command, "rOf") == 0) {
        if (strcmp(param, "valve") == 0) {
            wateringEnabled = false;
            DEBUG_PRINTLN("Valve closed.");
        }
        if (strcmp(param, "heat") == 0) {
            heatingEnabled = true;
            DEBUG_PRINTLN("Heat disabled.");
        }
    }
}

bool Relay::isHeatingEnabled() {
    return heatingEnabled;
}

void Relay::heatingOn() {
    SerialFrame heatingFrame = SerialFrame(relayOnSerialCommand, "heat");
    AppSerial::sendFrame(&heatingFrame);
}

void Relay::heatingOff() {
    SerialFrame heatingFrame = SerialFrame(relayOffSerialCommand, "heat");
    AppSerial::sendFrame(&heatingFrame);
}

// watering

bool Relay::isWateringEnabled() {
    return wateringEnabled;
}

void Relay::wateringOn() {
    SerialFrame openValveFrame = SerialFrame(relayOnSerialCommand, "valve");
    AppSerial::sendFrame(&openValveFrame);
}

void Relay::wateringOff() {
    SerialFrame closeValveFrame = SerialFrame(relayOffSerialCommand, "valve");
    AppSerial::sendFrame(&closeValveFrame);
}
