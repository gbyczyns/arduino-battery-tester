#include "Arduino.h"
#include "Cell.h"

Cell::Cell(OutputHandler * outputHandler, byte rowNumber, byte pinVoltage, byte pinFetVoltage, byte dischargeControlPin) :
        rowNumber(rowNumber), outputHandler(outputHandler), pinVoltage(pinVoltage), pinFetVoltage(pinFetVoltage), dischargeControlPin(dischargeControlPin) {
    pinMode(pinVoltage, INPUT);
    pinMode(pinFetVoltage, INPUT);
    pinMode(dischargeControlPin, OUTPUT);
    dischargingAnimation = Animation::createDischargingAnimation();
    doneAnimation = Animation::createDoneAnimation();
}

void Cell::printSummary() {
    outputHandler->clearLine(rowNumber);

    // print voltage
    char buff[5];
    sprintf(buff, "%4u", cellVoltage >= ABSOLUTE_MIN_VOLTAGE ? cellVoltage : 0);
    outputHandler->print(2, rowNumber, buff);
    outputHandler->print(6, rowNumber, "mV" );

    if (cellStatus == CellStatus::DETECTING_TYPE) {
        outputHandler->print(9, rowNumber, "brak ogniwa");
    } else {
        char buff[5];
        sprintf(buff, "%4u", charge / 1000);
        outputHandler->print(9, rowNumber, buff);
        outputHandler->print(13, rowNumber, "mAh" );
        outputHandler->printCustomChar(17, rowNumber, cellStatus == CellStatus::DISCHARGING ? dischargingAnimation->getNextFrame() : doneAnimation->getNextFrame());
        outputHandler->printCustomChar(18, rowNumber, cellType == CellType::LI_ION ? 4 : 6);
        outputHandler->printCustomChar(19, rowNumber, cellType == CellType::LI_ION ? 5 : 7);
    }
}

void Cell::process() {
    if (cellStatus != CellStatus::DISCHARGING) {
        digitalWrite(dischargeControlPin, LOW); // turn off the load
    }

    cellVoltage = analogRead(pinVoltage) / 1023.0 * REF_VOLTAGE;
    if (cellStatus == CellStatus::DISCHARGING) {
        // Calculate the time duration between the last reading (in milliseconds)
        unsigned long currentTime = millis();
        unsigned long duration = currentTime - prevTime;
        prevTime = currentTime;

        unsigned int lowerThreshold = (cellType == CellType::LI_ION ? LIION_MIN_VOLTAGE : NIMH_MIN_VOLTAGE);
        if (cellVoltage >= lowerThreshold) {
            unsigned int fetVoltage = analogRead(pinFetVoltage) / 1023.0 * REF_VOLTAGE;
            // Current through resistor is voltage across the resistor divided by load resistance. Since the voltage is in millivolts, the current will be in milliamps
            unsigned int loadCurrent = (cellVoltage - fetVoltage) / LOAD_RESISTANCE;
            // Must divide by (60*60*1000) to convert duration in micro seconds to hours. But doing this now would cause a loss of precision, hence division by 3600 which will result in microamp hours to be summed.
            charge += (loadCurrent * duration) / 3600;
            lastGoodSample = millis();
        } else if (millis() - lastGoodSample > 3000) { // jeśli ostatni dobry pomiar był wcześniej niż 3000 ms temu, kończymy rozładowywanie
            cellStatus = CellStatus::DONE;
            outputHandler->beep(1800, 1000);
        }
    } else if (cellStatus == CellStatus::MEASURING_RESISTANCE) {
        
    } else if (cellStatus == CellStatus::DETECTING_TYPE) {
        if (cellVoltage >= LIION_MIN_VOLTAGE && cellVoltage <= LIION_MAX_VOLTAGE) {
            cellType = CellType::LI_ION;
        } else if (cellVoltage >= NIMH_MIN_VOLTAGE && cellVoltage <= NIMH_MAX_VOLTAGE) {
            cellType = CellType::NI_MH;
        } else {
            lastBadSample = millis();
        }

        if (millis() - lastBadSample > 500) { // jeśli ostatni błędny pomiar był wcześniej niż 500 ms temu, rozpoczynamy rozładowywanie
            cellStatus = CellStatus::DISCHARGING;
            charge = 0;
            prevTime = millis();
            digitalWrite(dischargeControlPin, HIGH); // turn on the FET
            outputHandler->beep(7000, 100);
        } 
    } else if (cellStatus == CellStatus::DONE) {
        if ((cellVoltage > ABSOLUTE_MIN_VOLTAGE) ^ (charge == 0)) {
            cellStatus = CellStatus::DETECTING_TYPE;
        }
    }

    printSummary();
}

