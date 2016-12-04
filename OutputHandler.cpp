#include "Arduino.h"
#include "OutputHandler.h"

OutputHandler::OutputHandler() {
    lcd = new LiquidCrystal(10, 11, 6, 7, 8, 9);
    byte batteryFull[] = { 0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
    //byte batteryAlmostFull[] = { 0b01110, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
    byte batteryHalf[] = { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111 };
    byte batteryEmpty[] = { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111 };
    byte miliOhm[] = { 0b11010, 0b10101, 0b10101, 0b00000, 0b01110, 0b10001, 0b01010, 0b11011 };
    byte typeLiIon1[] = { 0b00100, 0b00100, 0b00100, 0b10111, 0b10000, 0b10111, 0b10101, 0b10111 };
    byte typeLiIon2[] = { 0b10000, 0b00000, 0b10000, 0b10000, 0b00000, 0b11000, 0b10100, 0b10100 };
    byte typeNiMH1[] = { 0b01001, 0b01101, 0b01011, 0b01001, 0b00000, 0b11011, 0b10101, 0b10001 };
    byte typeNiMH2[] = { 0b10000, 0b00000, 0b10000, 0b10000, 0b00000, 0b10010, 0b11110, 0b10010 };

    lcd->begin(20, 4);
    print(0, 1, "    Tester ogniw");
    print(0, 2, "  Li-Ion/NiMH/NiCD");
    lcd->createChar(0, batteryFull);
    lcd->createChar(1, batteryHalf);
    lcd->createChar(2, batteryEmpty);
    lcd->createChar(3, miliOhm);
    lcd->createChar(4, typeLiIon1);
    lcd->createChar(5, typeLiIon2);
    lcd->createChar(6, typeNiMH1);
    lcd->createChar(7, typeNiMH2);
    delay(1000);
    lcd->clear();
}

void OutputHandler::printSummary(Cell * cell) {
    print(0, cell->getRowNumber(), "                    ");

    // print voltage
    char buff[5];
    sprintf(buff, "%4u", cell->getCellVoltage() >= ABSOLUTE_MIN_VOLTAGE ? cell->getCellVoltage() : 0);
    print(0, cell->getRowNumber(), buff);
    print(4, cell->getRowNumber(), "mV" );

    if (cell->getCellStatus() == CellStatus::DETECTING_TYPE) {
        print(7, cell->getRowNumber(), "brak ogniwa");
    } else {
        sprintf(buff, "%4u", cell->getCharge() / 1000);
        print(7, cell->getRowNumber(), buff);
        print(11, cell->getRowNumber(), "mAh" );
        if (internalResistanceRendered) {
            sprintf(buff, "%4u", cell->getInternalResistance());
            print(15, cell->getRowNumber(), buff);
            printCustomChar(19, cell->getRowNumber(), 3);
        } else {
            printCustomChar(17, cell->getRowNumber(), cell->getCellStatus() == CellStatus::DISCHARGING ? dischargingAnimation->getFrame() : doneAnimation->getFrame());
            printCustomChar(18, cell->getRowNumber(), cell->getCellType() == CellType::LI_ION ? 4 : 6);
            printCustomChar(19, cell->getRowNumber(), cell->getCellType() == CellType::LI_ION ? 5 : 7);
        }
    }
}

void OutputHandler::allDone() {
    unsigned long currentTime = millis();
    if (currentTime - lastAnimationFrameIncrement > 100) { // zmiana klatki animacji co 100 milisekund
        dischargingAnimation->advanceToNextFrame();
        doneAnimation->advanceToNextFrame();
        lastAnimationFrameIncrement = currentTime;
    }
    if (currentTime - lastSummaryCycle > 2000) {
        internalResistanceRendered = !internalResistanceRendered;
        lastSummaryCycle = currentTime;
    }
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
