#ifndef Relay_h
#define Relay_h

#include <Arduino.h>

class Relay {
public:
    Relay();

    ~Relay();

    static void parseSerialCommand(const char *command, const char *param);

    // watering
    static bool isWateringOn();

    static bool wateringValveIsOpen();

    static void wateringOpenValve();

    static void wateringCloseValve();
};

#endif /* Relay_h */
