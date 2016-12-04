#include "Cell.h"
#include "OutputHandler.h"

void setup() {
    pinMode(SPEAKER_PIN, OUTPUT);
    tone(SPEAKER_PIN, 5000, 50);
}

void loop() {
    OutputHandler * outputHandler = new OutputHandler();
    Cell cell1(0, A0, A1, 2);
    Cell cell2(1, A2, A3, 3);
    Cell cell3(2, A4, A5, 4);
    Cell cell4(3, A6, A7, 5);

    while (1) {
        cell1.process();
        cell2.process();
        cell3.process();
        cell4.process();

        outputHandler->printSummary(&cell1);
        outputHandler->printSummary(&cell2);
        outputHandler->printSummary(&cell3);
        outputHandler->printSummary(&cell4);
        outputHandler->allDone();
        delay(50);
    }
}
