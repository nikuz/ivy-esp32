#ifndef Relay_h
#define Relay_h

#include <Arduino.h>

class Relay {
public:
    Relay();

    ~Relay();

    static void parseSerialCommand(const char *command, const char *param);

    // heating

    static bool isHeatingEnabled();

    static void heatingOn();

    static void heatingOff();

    // watering

    static bool isWateringEnabled();

    static void wateringOn();

    static void wateringOff();
};

#endif /* Relay_h */
