#include "Arduino.h"
#include "OutputHandler.h"

OutputHandler::OutputHandler() {
    lcd = new LiquidCrystal(10, 11, 6, 7, 8, 9);
    byte batteryFull[] = { 0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
    byte batteryAlmostFull[] = { 0b01110, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
    byte batteryHalf[] = { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111 };
    byte batteryEmpty[] = { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111 };
    byte typeLiIon1[] = { 0b00100, 0b00100, 0b00100, 0b10111, 0b10000, 0b10111, 0b10101, 0b10111 };
    byte typeLiIon2[] = { 0b10000, 0b00000, 0b10000, 0b10000, 0b00000, 0b11000, 0b10100, 0b10100 };
    byte typeNiMH1[] = { 0b01001, 0b01101, 0b01011, 0b01001, 0b00000, 0b11011, 0b10101, 0b10001 };
    byte typeNiMH2[] = { 0b10000, 0b00000, 0b10000, 0b10000, 0b00000, 0b10010, 0b11110, 0b10010 };
    lcd->begin(20, 4);
    print(0, 1, "    Tester ogniw");
    print(0, 2, "  Li-Ion/NiMH/NiCD");
    lcd->createChar(0, batteryFull);
    lcd->createChar(1, batteryAlmostFull);
    lcd->createChar(2, batteryHalf);
    lcd->createChar(3, batteryEmpty);
    lcd->createChar(4, typeLiIon1);
    lcd->createChar(5, typeLiIon2);
    lcd->createChar(6, typeNiMH1);
    lcd->createChar(7, typeNiMH2);
    pinMode(SPEAKER_PIN, OUTPUT);
    beep(5000, 50);
    delay(1000);
    lcd->clear();
}

void OutputHandler::setCursor(byte colNum, byte rowNum) {
    lcd->setCursor(colNum, rowNum);
}

void OutputHandler::print(byte colNum, byte rowNum, const char c[]) {
    setCursor(colNum, rowNum);
    lcd->print(c);
}

void OutputHandler::print(byte colNum, byte rowNum, int i) {
    setCursor(colNum, rowNum);
    lcd->print(i);
}

void OutputHandler::printCustomChar(byte colNum, byte rowNum, byte charNumber) {
    setCursor(colNum, rowNum);
    lcd->write(charNumber);
}

void OutputHandler::printHeader() {
    this->print(0, 0, 1);
    this->print(0, 1, 2);
    this->print(0, 2, 3);
    this->print(0, 3, 4);
}

void OutputHandler::clearLine(byte rowNum) {
    this->print(2, rowNum, "                  ");
}

void OutputHandler::beep(unsigned int frequency, unsigned int length) {
    tone(SPEAKER_PIN, frequency, length);
}
