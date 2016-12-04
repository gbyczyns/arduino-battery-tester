#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H
#include <LiquidCrystal.h>
#include "Cell.h"
#include "Animation.h"

class OutputHandler {
    LiquidCrystal * lcd;
    Animation * dischargingAnimation = Animation::createDischargingAnimation();
    Animation * doneAnimation = Animation::createDoneAnimation();
    unsigned long lastAnimationFrameIncrement;

    unsigned long lastSummaryCycle;
    boolean internalResistanceRendered = false;

    void setCursor(byte, byte);
    void print(byte, byte, const char[]);
    void print(byte, byte, int);
    void printCustomChar(byte, byte, byte);
    void clearLine(byte);

    public:
    OutputHandler();
    void printSummary(Cell * cell);
    void allDone();
};

#endif
