#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>

struct SoilMoistureSensor {
    int sensorId;
    int min;
    int max;
};

class Sensor {
public:
    Sensor();

    ~Sensor();

    static void parseSerialCommand(const char *command, const char *param);

    static int temperatureGet();

    static int humidityGet();

    static int getSoilMoisture(int sensorId);

    static int getSoilTemperature(int sensorId);

    static int getLightIntensity();
};

#endif /* Sensor_h */
