#include <Arduino.h>

#include "def.h"
#include "Sensor.h"
#include "Tools.h"

int currentTemperature = 0;
int currentHumidity = 0;
unsigned int soilMoisture[3] = {
    0,
    0,
    0,
};
unsigned int soilTemperature[3] = {
    0,
    0,
    0,
};
static SoilMoistureSensor soilMoistureSensors[] = {
    {SOIL_SENSOR_1, SOIL_SENSOR_1_MIN, SOIL_SENSOR_1_MAX},
    {SOIL_SENSOR_2, SOIL_SENSOR_2_MIN, SOIL_SENSOR_2_MAX},
    {SOIL_SENSOR_3, SOIL_SENSOR_3_MIN, SOIL_SENSOR_3_MAX},
};
int lightIntensity = 0;

Sensor::Sensor() {}

Sensor::~Sensor() {}

void Sensor::parseSerialCommand(const char *command, const char *param) {
    int value = atoi(param);
    if (strcmp(command, "temp") == 0) {
        currentTemperature = value;
    }
    if (strcmp(command, "hum") == 0) {
        currentHumidity = value > 0 ? value : 0;
    }
    if (strcmp(command, "sh1") == 0) {
        soilMoisture[0] = value > 0 ? value : 0;
    }
    if (strcmp(command, "sh2") == 0) {
        soilMoisture[1] = value > 0 ? value : 0;
    }
    if (strcmp(command, "sh3") == 0) {
        soilMoisture[2] = value > 0 ? value : 0;
    }
    if (strcmp(command, "st1") == 0) {
        soilTemperature[0] = value > -100 ? value : 0;
    }
    if (strcmp(command, "st2") == 0) {
        soilTemperature[1] = value > -100 ? value : 0;
    }
    if (strcmp(command, "st3") == 0) {
        soilTemperature[2] = value > -100 ? value : 0;
    }
    if (strcmp(command, "light") == 0) {
        lightIntensity = value > 0 ? value : 0;
    }
}

// temperature

int Sensor::temperatureGet() {
    return currentTemperature;
}

// humidity

int Sensor::humidityGet() {
    return currentHumidity;
}

// soil

int Sensor::getSoilMoisture(int sensorId) {
    int value = soilMoisture[sensorId - 1];
    if (value) {
        const int varsLen = *(&soilMoistureSensors + 1) - soilMoistureSensors;
        for (int i = 0; i < varsLen; i++) {
            if (soilMoistureSensors[i].sensorId == sensorId) {
                value = map(value, soilMoistureSensors[i].min, soilMoistureSensors[i].max, 0, 100);
                if (value < 0) {
                    value = 0;
                } else if (value > 100) {
                    value = 100;
                }
            }
        }
    }

    return value;
}

int Sensor::getSoilTemperature(int sensorId) {
    return soilTemperature[sensorId - 1];
}

// light

int Sensor::getLightIntensity() {
    return lightIntensity;
}
