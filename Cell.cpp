#include "Arduino.h"
#include "Cell.h"

Cell::Cell(byte rowNumber, byte pinVoltage, byte pinFetVoltage, byte dischargeControlPin) : rowNumber(rowNumber), pinVoltage(pinVoltage), pinFetVoltage(pinFetVoltage), dischargeControlPin(dischargeControlPin) {
    pinMode(pinVoltage, INPUT);
    pinMode(pinFetVoltage, INPUT);
    pinMode(dischargeControlPin, OUTPUT);
}

void Cell::process() {
    if (cellStatus != CellStatus::DISCHARGING) {
        digitalWrite(dischargeControlPin, LOW); // turn off the load
    }

    cellVoltage = readCellVoltage();
    if (cellStatus == CellStatus::DISCHARGING) {
        // Calculate the time duration between the last reading (in milliseconds)
        unsigned long currentTime = millis();
        unsigned long duration = currentTime - prevTime;
        prevTime = currentTime;
        elapsedDischargeTime = currentTime - dischargeStartTime;

        unsigned int lowerThreshold = (cellType == CellType::LI_ION ? LIION_MIN_VOLTAGE : NIMH_MIN_VOLTAGE);
        if (cellVoltage >= lowerThreshold) {
            unsigned int fetVoltage = analogRead(pinFetVoltage) / 1023.0 * REF_VOLTAGE;
            // Current through resistor is voltage across the resistor divided by load resistance. Since the voltage is in millivolts, the current will be in milliamps
            unsigned int loadCurrent = (cellVoltage - fetVoltage) / LOAD_RESISTANCE;
            // Must divide by (60*60*1000) to convert duration in micro seconds to hours. But doing this now would cause a loss of precision, hence division by 3600 which will result in microamp hours to be summed.
            charge += (loadCurrent * duration) / 3600;
            lastGoodSample = millis();
        } else if (millis() - lastGoodSample > 1500) { // jeśli ostatni dobry pomiar był wcześniej niż 1500 ms temu, kończymy rozładowywanie
            cellStatus = CellStatus::DONE;
            tone(SPEAKER_PIN, 1800, 1000);
        }
    } else if (cellStatus == CellStatus::DETECTING_TYPE) {
        if (cellVoltage >= LIION_MIN_VOLTAGE && cellVoltage <= LIION_MAX_VOLTAGE) {
            cellType = CellType::LI_ION;
        } else if (cellVoltage >= NIMH_MIN_VOLTAGE && cellVoltage <= NIMH_MAX_VOLTAGE) {
            cellType = CellType::NI_MH;
        } else {
            lastBadSample = millis();
        }

        if (millis() - lastBadSample > 1000) { // jeśli ostatni błędny pomiar był wcześniej niż 1000 ms temu, rozpoczynamy rozładowywanie
            digitalWrite(dischargeControlPin, HIGH); // turn on the FET
            delay(500);
            unsigned int voltageAfterApplyingLoad = readCellVoltage();
            internalResistance = LOAD_RESISTANCE * 1000.0 * (cellVoltage - voltageAfterApplyingLoad) / voltageAfterApplyingLoad;

            tone(SPEAKER_PIN, 7000, 100);
            cellStatus = CellStatus::DISCHARGING;
            charge = 0;
            dischargeStartTime = millis();
            prevTime = millis();
        } 
    } else if (cellStatus == CellStatus::DONE) {
        if (cellVoltage < ABSOLUTE_MIN_VOLTAGE) {
            // napięcie spadło = ogniwo zostało wyjęte, więc umożliwiamy przetestowanie kolejnego
            cellStatus = CellStatus::NOT_INSTALLED;
        }
    } else if (cellStatus == CellStatus::NOT_INSTALLED) {
        if (cellVoltage > ABSOLUTE_MIN_VOLTAGE) {
            cellStatus = CellStatus::DETECTING_TYPE;
        }
    }
}

unsigned int Cell::readCellVoltage() {
    return analogRead(pinVoltage) / 1023.0 * REF_VOLTAGE;
}

CellStatus Cell::getCellStatus() {
    return cellStatus;
}

CellType Cell::getCellType() {
    return cellType;
}

byte Cell::getRowNumber() {
    return rowNumber;
}

unsigned long Cell::getCharge() {
    return charge;
}

unsigned long Cell::getElapsedDischargeTime() {
    return elapsedDischargeTime;
}

unsigned int Cell::getCellVoltage() {
    return cellVoltage;
}

unsigned int Cell::getInternalResistance() {
    return internalResistance;
}

