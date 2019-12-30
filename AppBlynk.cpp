#include <Arduino.h>
#include <EspOta.h>

#include "def.h"

#ifdef DEBUG
#define BLYNK_DEBUG // Optional, this enables lots of prints
#define BLYNK_PRINT Serial
#endif
#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT     // Disable float operations
#define BLYNK_MSG_LIMIT 50

#include <BlynkSimpleEsp32.h>

#include "AppBlynkDef.h"
#include "AppBlynk.h"
#include "AppWiFi.h"
#include "AppStorage.h"
#include "Tools.h"
#include "Sensor.h"
#include "Relay.h"
#include "AppTime.h"
#include "Watering.h"
#include "Heating.h"

// Blynk virtual pins
const int pinTemperature = V0;
const int pinHumidity = V1;
const int pinLightIntensity = V25;
const int pinVersion = V5;
const int pinRtcBattery = V9;
const int pinOtaHost = V20;
const int pinOtaBin = V21;
const int pinOtaLastUpdateTime = V22;
const int pinUptime = V11;
const int pinRtcTemperature = V12;
const int pinSoilMoisture1 = V16;
const int pinSoilMoisture2 = V17;
const int pinSoilMoisture3 = V18;
const int pinSoilTemperature1 = V2;
const int pinSoilTemperature2 = V3;
const int pinSoilTemperature3 = V4;
const int pinWSoilMstrMin = V23;
const int pinAutoWatering = V29;
const int pinManualWatering = V38;
const int pinWatering = V24;
const int pinLastWatering = V33;
const int pinWateringInterval = V26;
const int pinMegaUptime = V34;
const int pinScreenEnabled = V15;
const int pinAutoHeating = V40;
const int pinHSoilTmpMin = V41;
const int pinHSoilTmpMax = V42;
const int pinLastHeating = V43;

// cache
int fishIntCache = -32000;
int temperatureCache = 0;
int humidityCache = 0;
int lightIntensityCache = 0;
int versionCache = 0;
int rtcBatteryCache = 0;
int rtcTemperatureCache = 0;
int soilMoistureCache1 = 0;
int soilMoistureCache2 = 0;
int soilMoistureCache3 = 0;
int soilTemperatureCache1 = 0;
int soilTemperatureCache2 = 0;
int soilTemperatureCache3 = 0;
int wateringCache = 0;
String fishStringCache = "fish";
String otaHostCache = "";
String otaBinCache = "";
String otaLastUpdateTimeCache = "";
String uptimeCache = "";
String lastWateringCache = "";
String megaUptimeCache = "";
String lastHeatingCache = "";

const unsigned long blynkConnectAttemptTime = 5UL * 1000UL;  // try to connect to blynk server only 5 seconds
bool blynkConnectAttemptFirstTime = true;
WidgetTerminal blynkTerminal(V30);

static BlynkIntVariable intVariables[15];
static BlynkStringVariable stringVariables[10];
static BlynkSyncVariable syncVariables[] = {
    {"otaHost",           false},
    {"otaBin",            false},
    {"otaLastUpdateTime", false},
    {"uptime",            false},
    {"version",           false},
    {"temperature",       false},
    {"humidity",          false},
    {"rtcBattery",        false},
    {"rtcTemperature",    false},
    {"soilMoisture1",     false},
    {"soilMoisture2",     false},
    {"soilMoisture3",     false},
    {"soilTemperature1",  false},
    {"soilTemperature2",  false},
    {"soilTemperature3",  false},
    {"watering",          false},
    {"lightIntensity",    false},
    {"lastWatering",      false},
    {"megaUptime",        false},
    {"lastHeating",      false},
};
const int syncValuesPerSecond = 5;

AppBlynk::AppBlynk() {};

AppBlynk::~AppBlynk() {};

// private

int AppBlynk::getPinById(const char *pinId) {
    if (strcmp(pinId, "temperature") == 0) return pinTemperature;
    if (strcmp(pinId, "humidity") == 0) return pinHumidity;
    if (strcmp(pinId, "lightIntensity") == 0) return pinLightIntensity;
    if (strcmp(pinId, "version") == 0) return pinVersion;
    if (strcmp(pinId, "rtcBattery") == 0) return pinRtcBattery;
    if (strcmp(pinId, "otaHost") == 0) return pinOtaHost;
    if (strcmp(pinId, "otaBin") == 0) return pinOtaBin;
    if (strcmp(pinId, "otaLastUpdateTime") == 0) return pinOtaLastUpdateTime;
    if (strcmp(pinId, "uptime") == 0) return pinUptime;
    if (strcmp(pinId, "rtcTemperature") == 0) return pinRtcTemperature;
    if (strcmp(pinId, "soilMoisture1") == 0) return pinSoilMoisture1;
    if (strcmp(pinId, "soilMoisture2") == 0) return pinSoilMoisture2;
    if (strcmp(pinId, "soilMoisture3") == 0) return pinSoilMoisture3;
    if (strcmp(pinId, "soilTemperature1") == 0) return pinSoilTemperature1;
    if (strcmp(pinId, "soilTemperature2") == 0) return pinSoilTemperature2;
    if (strcmp(pinId, "soilTemperature3") == 0) return pinSoilTemperature3;
    if (strcmp(pinId, "wSoilMstrMin") == 0) return pinWSoilMstrMin;
    if (strcmp(pinId, "autoWatering") == 0) return pinAutoWatering;
    if (strcmp(pinId, "manualWatering") == 0) return pinManualWatering;
    if (strcmp(pinId, "watering") == 0) return pinWatering;
    if (strcmp(pinId, "lastWatering") == 0) return pinLastWatering;
    if (strcmp(pinId, "wInterval") == 0) return pinWateringInterval;
    if (strcmp(pinId, "megaUptime") == 0) return pinMegaUptime;
    if (strcmp(pinId, "screenEnabled") == 0) return pinScreenEnabled;
    if (strcmp(pinId, "autoHeating") == 0) return pinAutoHeating;
    if (strcmp(pinId, "hSoilTmpMin") == 0) return pinHSoilTmpMin;
    if (strcmp(pinId, "hSoilTmpMax") == 0) return pinHSoilTmpMax;
    if (strcmp(pinId, "lastHeating") == 0) return pinLastHeating;

    return -1;
}

int &AppBlynk::getIntCacheValue(const char *pinId) {
    if (strcmp(pinId, "temperature") == 0) return temperatureCache;
    if (strcmp(pinId, "humidity") == 0) return humidityCache;
    if (strcmp(pinId, "lightIntensity") == 0) return lightIntensityCache;
    if (strcmp(pinId, "version") == 0) return versionCache;
    if (strcmp(pinId, "rtcBattery") == 0) return rtcBatteryCache;
    if (strcmp(pinId, "rtcTemperature") == 0) return rtcTemperatureCache;
    if (strcmp(pinId, "soilMoisture1") == 0) return soilMoistureCache1;
    if (strcmp(pinId, "soilMoisture2") == 0) return soilMoistureCache2;
    if (strcmp(pinId, "soilMoisture3") == 0) return soilMoistureCache3;
    if (strcmp(pinId, "soilTemperature1") == 0) return soilTemperatureCache1;
    if (strcmp(pinId, "soilTemperature2") == 0) return soilTemperatureCache2;
    if (strcmp(pinId, "soilTemperature3") == 0) return soilTemperatureCache3;
    if (strcmp(pinId, "watering") == 0) return wateringCache;

    return fishIntCache;
}

String &AppBlynk::getStringCacheValue(const char *pinId) {
    if (strcmp(pinId, "otaHost") == 0) return otaHostCache;
    if (strcmp(pinId, "otaBin") == 0) return otaBinCache;
    if (strcmp(pinId, "otaLastUpdateTime") == 0) return otaLastUpdateTimeCache;
    if (strcmp(pinId, "uptime") == 0) return uptimeCache;
    if (strcmp(pinId, "megaUptime") == 0) return megaUptimeCache;
    if (strcmp(pinId, "lastWatering") == 0) return lastWateringCache;
    if (strcmp(pinId, "lastHeating") == 0) return lastHeatingCache;

    return fishStringCache;
}

int &AppBlynk::getIntVariable(const char *pin) {
    const int intVarsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < intVarsLen; i++) {
        if (intVariables[i].pin == pin) {
            return *intVariables[i].var;
        }
    }

    return fishIntCache;
}

String &AppBlynk::getStringVariable(const char *pin) {
    const int stringVarsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < stringVarsLen; i++) {
        if (stringVariables[i].pin == pin) {
            return *stringVariables[i].var;
        }
    }

    return fishStringCache;
}

void AppBlynk::sync() { // every second
    DEBUG_PRINTLN("Check connect:");
    DEBUG_PRINT("Wifi connected: ");
    DEBUG_PRINTLN(AppWiFi::isConnected());
    DEBUG_PRINT("Blynk connected: ");
    DEBUG_PRINTLN(Blynk.connected());
    DEBUG_PRINT("Millis: ");
    DEBUG_PRINTLN(millis());
    DEBUG_PRINT("Overflow is close: ");
    DEBUG_PRINTLN(Tools::millisOverflowIsClose());
    if (!AppWiFi::isConnected() || !Blynk.connected() || Tools::millisOverflowIsClose()) {
        return;
    }

    int syncCounter = 0;
    const int varsLen = *(&syncVariables + 1) - syncVariables;
    DEBUG_PRINT("Vars to sync: ");
    DEBUG_PRINTLN(varsLen);
    for (int i = 0; i < varsLen; i++) {
        if (syncCounter < syncValuesPerSecond) {
            if (syncVariables[i].synced) {
                continue;
            }

            const char *pin = syncVariables[i].pin;
            DEBUG_PRINT("Sync pin: ");
            DEBUG_PRINT(pin);
            DEBUG_PRINT(": ");
            if (strcmp(pin, "otaHost") == 0) {
                String &otaHostVariable = AppBlynk::getStringVariable(pin);
                AppBlynk::postData(pin, otaHostVariable);
            };
            if (strcmp(pin, "otaBin") == 0) {
                String &otaBinVariable = AppBlynk::getStringVariable(pin);
                AppBlynk::postData(pin, otaBinVariable);
            };
            if (strcmp(pin, "otaLastUpdateTime") == 0) {
                AppBlynk::postData(pin, EspOta::getUpdateTime());
            };
            if (strcmp(pin, "uptime") == 0) {
                AppBlynk::postData(pin, String(Tools::getUptime()));
            };
            if (strcmp(pin, "version") == 0) {
                AppBlynk::postData(pin, VERSION);
            };
            if (strcmp(pin, "temperature") == 0) {
                AppBlynk::postData(pin, Sensor::temperatureGet());
            };
            if (strcmp(pin, "humidity") == 0) {
                AppBlynk::postData(pin, Sensor::humidityGet());
            };
            if (strcmp(pin, "rtcBattery") == 0) {
                AppBlynk::postData(pin, AppTime::RTCBattery() ? 255 : 0);
            };
            if (strcmp(pin, "rtcTemperature") == 0) {
                AppBlynk::postData(pin, AppTime::RTCGetTemperature());
            };
            if (strcmp(pin, "soilMoisture1") == 0) {
                AppBlynk::postData(pin, Sensor::getSoilMoisture(SOIL_SENSOR_1));
            };
            if (strcmp(pin, "soilMoisture2") == 0) {
                AppBlynk::postData(pin, Sensor::getSoilMoisture(SOIL_SENSOR_2));
            };
            if (strcmp(pin, "soilMoisture3") == 0) {
                AppBlynk::postData(pin, Sensor::getSoilMoisture(SOIL_SENSOR_3));
            };
            if (strcmp(pin, "soilTemperature1") == 0) {
                AppBlynk::postData(pin, Sensor::getSoilTemperature(SOIL_SENSOR_1));
            };
            if (strcmp(pin, "soilTemperature2") == 0) {
                AppBlynk::postData(pin, Sensor::getSoilTemperature(SOIL_SENSOR_2));
            };
            if (strcmp(pin, "soilTemperature3") == 0) {
                AppBlynk::postData(pin, Sensor::getSoilTemperature(SOIL_SENSOR_3));
            };
            if (strcmp(pin, "watering") == 0) {
                AppBlynk::postData(pin, Relay::isWateringEnabled() ? 255 : 0);
            };
            if (strcmp(pin, "lightIntensity") == 0) {
                AppBlynk::postData(pin, Sensor::getLightIntensity());
            };
            if (strcmp(pin, "lastWatering") == 0) {
                AppBlynk::postData(pin, Watering::getStringVariable(pin));
            };
            if (strcmp(pin, "megaUptime") == 0) {
                AppBlynk::postData(pin, String(Tools::getMegaUptime()));
            };
            if (strcmp(pin, "megaUptime") == 0) {
                AppBlynk::postData(pin, String(Tools::getMegaUptime()));
            };
            if (strcmp(pin, "lastHeating") == 0) {
                AppBlynk::postData(pin, Heating::getStringVariable(pin));
            };
            syncVariables[i].synced = true;
            syncCounter++;
        }
    }

    if (syncCounter < syncValuesPerSecond) {
        for (int i = 0; i < varsLen; i++) {
            syncVariables[i].synced = false;
        }
    }

#if PRODUCTION

#endif
}

void writeHandler(const char *pin, int value, bool store) {
    int &variable = AppBlynk::getIntVariable(pin);
    AppBlynk::getData(variable, pin, value, store);
}

void writeHandler(const char *pin, String value, bool store) {
    String &variable = AppBlynk::getStringVariable(pin);
    AppBlynk::getData(variable, pin, value, store);
}

BLYNK_WRITE(V20) { // otaHost
    writeHandler("otaHost", param.asStr(), true);
};
BLYNK_WRITE(V21) { // otaBin
    writeHandler("otaBin", param.asStr(), true);
};
BLYNK_WRITE(V15) { // screenEnabled
    writeHandler("screenEnabled", param.asInt(), true);
};
BLYNK_WRITE(V23) { // wSoilMstrMin
    writeHandler("wSoilMstrMin", param.asInt(), true);
};
BLYNK_WRITE(V26) { // wInterval
    writeHandler("wInterval", param.asInt(), true);
};
BLYNK_WRITE(V29) { // autoWatering
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("autoWatering", value, true);
};
BLYNK_WRITE(V38) { // manualWatering
    int value = param.asInt();
    if (value == 0) {
        Watering::stop();
    }
    writeHandler("manualWatering", value, false);
};
BLYNK_WRITE(V40) { // autoHeating
    int value = param.asInt();
    if (value == 0) {
        Heating::stop();
    }
    writeHandler("autoHeating", value, true);
};
BLYNK_WRITE(V41) { // hSoilTmpMin
    writeHandler("hSoilTmpMin", param.asInt(), true);
};
BLYNK_WRITE(V42) { // hSoilTmpMax
    writeHandler("hSoilTmpMax", param.asInt(), true);
};
BLYNK_WRITE(V10) { // ping
    if (param.asInt() == 1) {
        Blynk.notify("PONG");
        Blynk.virtualWrite(V10, 0);
    }
};
BLYNK_WRITE(V13) { // get time
    if (param.asInt() == 1) {
        AppTime::print();
        Blynk.virtualWrite(V13, 0);
    }
};
BLYNK_WRITE(V31) { // restart
    if (param.asInt() == 1) {
        Blynk.virtualWrite(V31, 0);
        delay(2000);
        ESP.restart();
    }
};
BLYNK_WRITE(V35) { // mega restart
    if (param.asInt() == 1) {
        Blynk.virtualWrite(V35, 0);
        Tools::megaRestart();
        delay(2000);
    }
};

BLYNK_CONNECTED() {
    Blynk.syncAll();
};

// public

void AppBlynk::setVariable(int *var, const char *pin) {
    int varsCount = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < varsCount; i++) {
        if (!intVariables[i].pin) {
            intVariables[i] = BlynkIntVariable(var, pin);
            break;
        }
    }
}

void AppBlynk::setVariable(String *var, const char *pin) {
    int varsCount = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < varsCount; i++) {
        if (!stringVariables[i].pin) {
            stringVariables[i] = BlynkStringVariable(var, pin);
            break;
        }
    }
}

void AppBlynk::checkConnect() {
    DEBUG_PRINTLN("Check connect:");
    DEBUG_PRINT("Wifi connected: ");
    DEBUG_PRINTLN(AppWiFi::isConnected());
    DEBUG_PRINT("Blynk connected: ");
    DEBUG_PRINTLN(Blynk.connected());
    DEBUG_PRINT("Millis: ");
    DEBUG_PRINTLN(millis());
    DEBUG_PRINT("Overflow is close: ");
    DEBUG_PRINTLN(Tools::millisOverflowIsClose());
    if (!blynkConnectAttemptFirstTime && Tools::millisOverflowIsClose()) {
        return;
    }
    if (AppWiFi::isConnected() && !Blynk.connected()) {
        unsigned long startConnecting = millis();
        while (!Blynk.connected()) {
            Blynk.connect();
            if (millis() > startConnecting + blynkConnectAttemptTime) {
                Serial.println("Unable to connect to Blynk server.\n");
                break;
            }
        }
        if (Blynk.connected() && blynkConnectAttemptFirstTime) {
            blynkTerminal.clear();
        }
        blynkConnectAttemptFirstTime = false;
    }
}

void AppBlynk::initiate() {
    Blynk.config(blynkAuth, blynkDomain, blynkPort);
    AppBlynk::checkConnect();
}

void AppBlynk::run() {
    if (Blynk.connected()) {
        Blynk.run();
    }
}

void AppBlynk::getData(int &localVariable, const char *pinId, int pinData, const bool storePreferences) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (localVariable == -1 || blynkPin == -1) {
        return;
    }
    if (pinData != localVariable) {
        localVariable = pinData;
        if (storePreferences) {
            AppStorage::putUInt(pinId, pinData);
        }
    }
}

void AppBlynk::getData(String &localVariable, const char *pinId, String pinData, const bool storePreferences) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (localVariable == "fish" || blynkPin == -1) {
        return;
    }
    if (pinData != localVariable) {
        localVariable = pinData;
        if (storePreferences) {
            AppStorage::putString(pinId, pinData);
        }
    }
}

void AppBlynk::postData(const char *pinId, int value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    int &cacheValue = AppBlynk::getIntCacheValue(pinId);
    if (cacheValue != -32000 || cacheValue != value) { // post data also if cache not applied for pin
        if (Blynk.connected()) {
            Blynk.virtualWrite(blynkPin, value);
        }
        if (cacheValue != -32000) {
            cacheValue = value;
        }
    }
}

void AppBlynk::postData(const char *pinId, String value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    String &cacheValue = AppBlynk::getStringCacheValue(pinId);
    if (cacheValue != "fish" || cacheValue != value) { // post data also if cache not applied for pin
        if (Blynk.connected()) {
            Blynk.virtualWrite(blynkPin, value);
        }
        if (cacheValue != "fish") {
            cacheValue = value;
        }
    }
}

void AppBlynk::postDataNoCache(const char *pinId, int value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    if (Blynk.connected()) {
        Blynk.virtualWrite(blynkPin, value);
    }
}

void AppBlynk::postDataNoCache(const char *pinId, String value) {
    int blynkPin = AppBlynk::getPinById(pinId);
    if (blynkPin == -1) {
        return;
    }
    if (Blynk.connected()) {
        Blynk.virtualWrite(blynkPin, value);
    }
}

void AppBlynk::print(String value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::print(char *value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::print(int value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::print(double value) {
    Serial.print(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
    }
}

void AppBlynk::println(String value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.print(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::println(char *value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.println(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::println(int value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.println(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::println(double value) {
    Serial.println(value);
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        blynkTerminal.println(value);
        blynkTerminal.flush();
    }
}

void AppBlynk::notify(String value) {
    if (AppWiFi::isConnected() && Blynk.connected() && !Tools::millisOverflowIsClose()) {
        Blynk.notify(value);
    }
}
