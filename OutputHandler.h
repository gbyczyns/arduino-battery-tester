#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H
#include <LiquidCrystal.h>
#define SPEAKER_PIN 12

class OutputHandler {
    LiquidCrystal * lcd;
    void setCursor(byte, byte);

    public:
    OutputHandler();
    void print(byte, byte, const char[]);
    void print(byte, byte, int);
    void printCustomChar(byte, byte, byte);
    void printHeader();
    void clearLine(byte);
    void beep(unsigned int, unsigned int);
};

#endif
