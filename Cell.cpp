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

void Cell::printVoltage(unsigned int batVoltage) {
    char buff[5];
    sprintf(buff, "%4u", batVoltage >= ABSOLUTE_MIN_VOLTAGE ? batVoltage : 0);
    outputHandler->print(2, rowNumber, buff);
    outputHandler->print(6, rowNumber, "mV" );
}

void Cell::printCellSummary(unsigned int batVoltage) {
    printVoltage(batVoltage);
    char buff[5];
    sprintf(buff, "%4u", charge / 1000);
    outputHandler->print(9, rowNumber, buff);
    outputHandler->print(13, rowNumber, "mAh" );
    outputHandler->printCustomChar(17, rowNumber, battStatus == TEST_IN_PROGRESS ? dischargingAnimation->getNextFrame() : doneAnimation->getNextFrame());
    outputHandler->printCustomChar(18, rowNumber, cellType == LI_ION_TYPE ? 4 : 6);
    outputHandler->printCustomChar(19, rowNumber, cellType == LI_ION_TYPE ? 5 : 7);
}

void Cell::process() {
    outputHandler->clearLine(rowNumber);
    unsigned int batVoltage = analogRead(pinVoltage) / 1023.0 * REF_VOLTAGE;
    if (battStatus == TEST_IN_PROGRESS) {
        // Calculate the time duration between the last reading (in milliseconds)
        unsigned long currentTime = millis();
        unsigned long duration = currentTime - prevTime;
        prevTime = currentTime;
        if (batVoltage > ABSOLUTE_MIN_VOLTAGE) { // zabezpieczenie przed dalszym zliczaniem pojemności w przypadku nagłego odłączenia ogniwa
            unsigned int fetVoltage = analogRead(pinFetVoltage) / 1023.0 * REF_VOLTAGE;
            // Current through resistor is voltage across the resistor divided by load resistance. Since the voltage is in millivolts, the current will be in milliamps
            unsigned int loadCurrent = (batVoltage - fetVoltage) / LOAD_RESISTANCE;
            // Must divide by (60*60*1000) to convert duration in micro seconds to hours. But doing this now would cause a loss of precision, hence division by 3600 which will result in microamp hours to be summed.
            charge += (loadCurrent * duration) / 3600;
        }
        unsigned int lowerThreshold = (cellType == LI_ION_TYPE ? LIION_MIN_VOLTAGE : NIMH_MIN_VOLTAGE);
        if (batVoltage >= lowerThreshold) {
            numSamplesBelowMin = 0; // reset counter since this reading was good
        } else if (numSamplesBelowMin >= 3 ) {
            battStatus = DONE;
            outputHandler->beep(1800, 1000);
        } else {
            numSamplesBelowMin++;
        }
        printCellSummary(batVoltage);
    } else if (battStatus == DETECTING_TYPE) {
        printVoltage(batVoltage);
        outputHandler->print(9, rowNumber, " wykrywanie");
        if (numSamplesAboveMin > 3) {
            if (batVoltage > LIION_MAX_VOLTAGE || batVoltage < NIMH_MIN_VOLTAGE || (batVoltage > NIMH_MAX_VOLTAGE && batVoltage <= LIION_MIN_VOLTAGE)) {
                battStatus = UNKNOWN_VOLTAGE;
                numSamplesAboveMin = 0;
            } else {
                // Initialize variables and start discharge
                cellType = batVoltage >= LIION_MIN_VOLTAGE ? LI_ION_TYPE : NI_MH_TYPE;
                battStatus = TEST_IN_PROGRESS;
                charge = 0;
                prevTime = millis();
                digitalWrite(dischargeControlPin, HIGH); // turn on the FET
                outputHandler->beep(7000, 100);
            }
        } else { // not enough good samples yet
            if (batVoltage > NIMH_MIN_VOLTAGE) {
                numSamplesAboveMin++;
                numSamplesBelowMin = 0;
            } else {
                numSamplesBelowMin++;
                numSamplesAboveMin = 0;
            }
        }
        if (numSamplesBelowMin > 3) {
            battStatus = NOT_INSTALLED;
        }
    } else if (battStatus == NOT_INSTALLED) {
        if (charge > 0) {
            // wcześniej było ładowane jakieś ogniwo, ale zostało wyjęte, więc pokazujemy jego statystyki
            printCellSummary(batVoltage);
        } else {
            outputHandler->print(2, rowNumber, "  <brak ogniwa>   ");
        }
        if (batVoltage >= NIMH_MIN_VOLTAGE) {
            // This condition indicates that a battery has been installed, change status to Detecting
            battStatus = DETECTING_TYPE;
        }
    } else if (battStatus == DONE) {
        if (batVoltage < ABSOLUTE_MIN_VOLTAGE) {
            // napięcie spadło = ogniwo zostało wyjęte, więc umożliwiamy przetestowanie kolejnego
            battStatus = NOT_INSTALLED;
        }
        printCellSummary(batVoltage);
    } else if (battStatus == UNKNOWN_VOLTAGE) {
        if (batVoltage < ABSOLUTE_MIN_VOLTAGE) {
            // napięcie spadło = ogniwo zostało wyjęte, więc umożliwiamy przetestowanie kolejnego
            battStatus = NOT_INSTALLED;
        }
        outputHandler->print(2, rowNumber, "nie rozpoznano    ");
    }

    if (battStatus != TEST_IN_PROGRESS) {
        digitalWrite(dischargeControlPin, LOW); // turn off the load
    }
}

