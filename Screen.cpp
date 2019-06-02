#include <Arduino.h>
#include <U8g2lib.h>

#include "def.h"
#include "Screen.h"
#include "AppTime.h"
#include "Tools.h"
#include "Sensor.h"

U8G2_SSD1327_WS_128X128_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 22, 21);

Screen::Screen() {}

Screen::~Screen() {}

void Screen::initiate() {
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_crox2cb_tr);

    const char loadingStr[] = "...";
    const u8g2_uint_t displayWidth = u8g2.getDisplayWidth();
    const u8g2_uint_t displayHeight = u8g2.getDisplayHeight();
    const int strWidth = u8g2.getStrWidth(loadingStr);

    u8g2.clearBuffer();
    u8g2.setCursor((displayWidth / 2) - (strWidth / 2), displayHeight / 2);
    u8g2.print(loadingStr);
//    u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
    u8g2.sendBuffer();
}

void Screen::clearBuffer() {
    u8g2.clearBuffer();
}

void Screen::sendBuffer() {
    u8g2.sendBuffer();
}

void Screen::printTemperature(int temperature, int humidity) {
    if (isnan(temperature) || isnan(humidity)) {
        return;
    }
    const int line = 13;
    u8g2.setFont(u8g2_font_crox2cb_tr);

    // temperature
    const char *temperatureStr = Tools::intToChar(temperature);
    u8g2.drawStr(0, line, temperatureStr);
    const int temperatureRow = u8g2.getStrWidth(temperatureStr);
    u8g2.drawCircle(temperatureRow + 5, line - 10, 2, U8G2_DRAW_ALL);

    // humidity
    const char *humidityStr = Tools::intToChar(humidity);
    const char *separator = " | ";
    u8g2.drawStr(temperatureRow + 11, line, separator);
    const int separatorRow = u8g2.getStrWidth(separator);
    u8g2.drawStr(temperatureRow + 11 + separatorRow, line, humidityStr);
    const int humidityRow = u8g2.getStrWidth(humidityStr);
    u8g2.drawStr(temperatureRow + 11 + separatorRow + humidityRow, line, "%");
}

void Screen::printLightIntensity(int lightIntensity) {
    if (isnan(lightIntensity)) {
        return;
    }
    const int line = 40;
    u8g2.setFont(u8g2_font_crox2cb_tr);
    const char *title = "Light: ";
    const int titleWidth = u8g2.getStrWidth(title);

    const char *valueStr = Tools::intToChar(lightIntensity);
    u8g2.drawStr(0, line, title);
    u8g2.drawStr(titleWidth, line, valueStr);
}

void Screen::printSoilMoisture(int value1, int value2, int value3) {
    if (isnan(value1) || isnan(value2) || isnan(value3)) {
        return;
    }
    const int line = 70;
    u8g2.setFont(u8g2_font_crox1cb_tf);

    const char *percent = "%";
    const int percentWidth = u8g2.getStrWidth(percent);
    const int gap = percentWidth;

    // value1
    const char *value1String = Tools::intToChar(value1);
    u8g2.drawStr(0, line, value1String);
    const int value1Width = u8g2.getStrWidth(value1String);
    u8g2.drawStr(value1Width, line, percent);

    // value2
    const char *value2String = Tools::intToChar(value2);
    const int value2Row = value1Width + percentWidth + gap;
    u8g2.drawStr(value2Row, line, value2String);
    const int value2Width = u8g2.getStrWidth(value2String);
    u8g2.drawStr(value2Row + value2Width, line, percent);

    // value3
    const char *value3String = Tools::intToChar(value3);
    const int value3Row = value2Row + value2Width + percentWidth + gap;
    u8g2.drawStr(value3Row, line, value3String);
    const int value3Width = u8g2.getStrWidth(value3String);
    u8g2.drawStr(value3Row + value3Width, line, percent);
}

void Screen::printSoilTemperature(int value1, int value2, int value3) {
    if (isnan(value1) || isnan(value2) || isnan(value3)) {
        return;
    }
    const int line = 95;
    u8g2.setFont(u8g2_font_crox1cb_tf);

    const int degreeSignWidth = 5;
    const int gap = degreeSignWidth;

    // value1
    const char *value1String = Tools::intToChar(value1);
    u8g2.drawStr(0, line, value1String);
    const int value1Width = u8g2.getStrWidth(value1String);
    u8g2.drawCircle(value1Width + gap, line - 10, 2, U8G2_DRAW_ALL);

    // value2
    const char *value2String = Tools::intToChar(value2);
    const int value2Row = value1Width + degreeSignWidth + (gap * 2);
    u8g2.drawStr(value2Row, line, value2String);
    const int value2Width = u8g2.getStrWidth(value2String);
    u8g2.drawCircle(value2Row + value2Width + gap, line - 10, 2, U8G2_DRAW_ALL);

    // value3
    const char *value3String = Tools::intToChar(value3);
    const int value3Row = value2Row + value2Width + degreeSignWidth + (gap * 2);
    u8g2.drawStr(value3Row, line, value3String);
    const int value3Width = u8g2.getStrWidth(value3String);
    u8g2.drawCircle(value3Row + value3Width + gap, line - 10, 2, U8G2_DRAW_ALL);
}

void Screen::printAppVersion() {
    u8g2_uint_t displayWidth = u8g2.getDisplayWidth();
    u8g2_uint_t displayHeight = u8g2.getDisplayHeight();
    u8g2.setFont(u8g2_font_u8glib_4_tf);
    u8g2.setCursor(displayWidth - 15, displayHeight);
    u8g2.print(VERSION);
}

void Screen::printTime(struct tm localtime) {
    u8g2_uint_t displayHeight = u8g2.getDisplayHeight();

    u8g2.setFont(u8g2_font_6x10_tn);
    u8g2.setCursor(0, displayHeight);
    u8g2.print(AppTime::getTimeString(localtime, "%02u/%02u/%04u %02u:%02u"));
}

void Screen::printUptime() {
    u8g2_uint_t displayHeight = u8g2.getDisplayHeight();

    u8g2.setFont(u8g2_font_crox2cb_tr);
    u8g2.setCursor(0, displayHeight);
    u8g2.print(Tools::getUptime());
}

void Screen::refresh() {
    clearBuffer();
    printTemperature(Sensor::temperatureGet(), Sensor::humidityGet());
    printLightIntensity(Sensor::getLightIntensity());
    printSoilMoisture(
        Sensor::getSoilMoisture(SOIL_SENSOR_1),
        Sensor::getSoilMoisture(SOIL_SENSOR_2),
        Sensor::getSoilMoisture(SOIL_SENSOR_3)
    );
    printSoilTemperature(
        Sensor::getSoilTemperature(SOIL_SENSOR_1),
        Sensor::getSoilTemperature(SOIL_SENSOR_2),
        Sensor::getSoilTemperature(SOIL_SENSOR_3)
    );
    printAppVersion();
    printUptime();

    sendBuffer();
}
