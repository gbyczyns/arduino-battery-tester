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

void Cell::printVoltage(unsigned int cellVoltage) {
    char buff[5];
    sprintf(buff, "%4u", cellVoltage >= ABSOLUTE_MIN_VOLTAGE ? cellVoltage : 0);
    outputHandler->print(2, rowNumber, buff);
    outputHandler->print(6, rowNumber, "mV" );
}

void Cell::printCellSummary(unsigned int cellVoltage) {
    printVoltage(cellVoltage);
    char buff[5];
    sprintf(buff, "%4u", charge / 1000);
    outputHandler->print(9, rowNumber, buff);
    outputHandler->print(13, rowNumber, "mAh" );
    outputHandler->printCustomChar(17, rowNumber, cellStatus == CellStatus::DISCHARGING ? dischargingAnimation->getNextFrame() : doneAnimation->getNextFrame());
    outputHandler->printCustomChar(18, rowNumber, cellType == CellType::LI_ION ? 4 : 6);
    outputHandler->printCustomChar(19, rowNumber, cellType == CellType::LI_ION ? 5 : 7);
}

void Cell::process() {
    if (cellStatus != CellStatus::DISCHARGING) {
        digitalWrite(dischargeControlPin, LOW); // turn off the load
    }
    outputHandler->clearLine(rowNumber);
    cellVoltage = analogRead(pinVoltage) / 1023.0 * REF_VOLTAGE;
    if (cellStatus == CellStatus::DISCHARGING) {
        // Calculate the time duration between the last reading (in milliseconds)
        unsigned long currentTime = millis();
        unsigned long duration = currentTime - prevTime;
        prevTime = currentTime;
        if (cellVoltage > ABSOLUTE_MIN_VOLTAGE) { // zabezpieczenie przed dalszym zliczaniem pojemności w przypadku nagłego odłączenia ogniwa
            unsigned int fetVoltage = analogRead(pinFetVoltage) / 1023.0 * REF_VOLTAGE;
            // Current through resistor is voltage across the resistor divided by load resistance. Since the voltage is in millivolts, the current will be in milliamps
            unsigned int loadCurrent = (cellVoltage - fetVoltage) / LOAD_RESISTANCE;
            // Must divide by (60*60*1000) to convert duration in micro seconds to hours. But doing this now would cause a loss of precision, hence division by 3600 which will result in microamp hours to be summed.
            charge += (loadCurrent * duration) / 3600;
        }
        unsigned int lowerThreshold = (cellType == CellType::LI_ION ? LIION_MIN_VOLTAGE : NIMH_MIN_VOLTAGE);
        if (cellVoltage >= lowerThreshold) {
            badSamplesCount = 0; // reset counter since this reading was good
        } else if (badSamplesCount >= 3 ) {
            cellStatus = CellStatus::DONE;
            outputHandler->beep(1800, 1000);
        } else {
            badSamplesCount++;
        }
        printCellSummary(cellVoltage);
    } else if (cellStatus == CellStatus::MEASURING_RESISTANCE) {
        
    } else if (cellStatus == CellStatus::DETECTING_TYPE) {
        printVoltage(cellVoltage);
        outputHandler->print(9, rowNumber, " wykrywanie");
        if (goodSamplesCount > 3) {
            if ((cellVoltage >= LIION_MIN_VOLTAGE && cellVoltage <= LIION_MAX_VOLTAGE) || (cellVoltage >= NIMH_MIN_VOLTAGE && cellVoltage <= NIMH_MAX_VOLTAGE)) {
                // Initialize variables and start discharge
                cellType = cellVoltage >= LIION_MIN_VOLTAGE ? CellType::LI_ION : CellType::NI_MH;
                cellStatus = CellStatus::DISCHARGING;
                charge = 0;
                prevTime = millis();
                digitalWrite(dischargeControlPin, HIGH); // turn on the FET
                outputHandler->beep(7000, 100);
            } else {
                cellStatus = CellStatus::NOT_INSTALLED;
                goodSamplesCount = 0;
            }
        } else if (cellVoltage > NIMH_MIN_VOLTAGE) {
          // not enough good samples yet
            goodSamplesCount++;
            badSamplesCount = 0;
        } else {
            badSamplesCount++;
            goodSamplesCount = 0;
        }
        if (badSamplesCount > 3) {
            cellStatus = CellStatus::NOT_INSTALLED;
        }
    } else if (cellStatus == CellStatus::NOT_INSTALLED) {
        if (charge > 0) {
            // wcześniej było ładowane jakieś ogniwo, ale zostało wyjęte, więc pokazujemy jego statystyki
            printCellSummary(cellVoltage);
        } else {
            outputHandler->print(2, rowNumber, "  <brak ogniwa>   ");
        }
        if (cellVoltage >= NIMH_MIN_VOLTAGE) {
            // This condition indicates that a battery has been installed, change status to Detecting
            cellStatus = CellStatus::DETECTING_TYPE;
        }
    } else if (cellStatus == CellStatus::DONE) {
        if (cellVoltage < ABSOLUTE_MIN_VOLTAGE) {
            // napięcie spadło = ogniwo zostało wyjęte, więc umożliwiamy przetestowanie kolejnego
            cellStatus = CellStatus::NOT_INSTALLED;
        }
        printCellSummary(cellVoltage);
    }
}

