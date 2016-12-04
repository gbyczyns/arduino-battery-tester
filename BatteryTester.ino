#include "Cell.h"

void setup() {
}

void loop() {
    OutputHandler * outputHandler = new OutputHandler();
    Cell cell1(outputHandler, 0, A0, A1, 2);
    Cell cell2(outputHandler, 1, A2, A3, 3);
    Cell cell3(outputHandler, 2, A4, A5, 4);
    Cell cell4(outputHandler, 3, A6, A7, 5);

    outputHandler->printHeader();
    while (1) {
        cell1.process();
        cell2.process();
        cell3.process();
        cell4.process();
        delay(100);
    }
}
