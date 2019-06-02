#ifndef Screen_h
#define Screen_h

#include <Arduino.h>

class Screen {
public:
    Screen();

    ~Screen();

    static void initiate();

    static void clearBuffer();

    static void sendBuffer();

    static void printTemperature(int currentTemperature, int currentHumidity);

    static void printLightIntensity(int lightIntensity);

    static void printSoilMoisture(int value1, int value2, int value3);

    static void printSoilTemperature(int value1, int value2, int value3);

    static void printAppVersion();

    static void printTime(struct tm localtime);

    static void printUptime();

    static void refresh();
};

#endif /* Screen_h */
