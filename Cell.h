#ifndef CELL_H
#define CELL_H
#include "OutputHandler.h"
#include "Animation.h"

#define LIION_MAX_VOLTAGE 4500
#define LIION_MIN_VOLTAGE 2600
#define NIMH_MAX_VOLTAGE 1700
#define NIMH_MIN_VOLTAGE 900
#define ABSOLUTE_MIN_VOLTAGE 200
#define LOAD_RESISTANCE 3.3d
#define REF_VOLTAGE 5000

enum {NOT_INSTALLED, DETECTING_TYPE, UNKNOWN_VOLTAGE, TEST_IN_PROGRESS, DONE}; // battery status values
enum {LI_ION_TYPE, NI_MH_TYPE};

class Cell {
    OutputHandler * outputHandler;
    Animation * dischargingAnimation;
    Animation * doneAnimation;
    const byte rowNumber;
    const byte pinVoltage;               // Analog sensor pin (0-5) for reading battery voltage
    const byte pinFetVoltage;            // Analog sensor pin (0-5) to read voltage across FET
    const byte dischargeControlPin;      // Output Pin that controlls the load for this battery
    byte battStatus = DETECTING_TYPE;    // set this to DONE when this cell's test is complete
    unsigned long charge = 0;            // Total microamp hours for this battery
    byte cellType = 0;          // voltage at which discharge is complete (mV)
    unsigned long prevTime = 0;          // Previous time reading (in milliseconds)
    unsigned int numSamplesAboveMin = 0; // number of good voltage readings (to determine battery installed)
    unsigned int numSamplesBelowMin = 0; // number of samples read below minimum (to determine battery discharged)
    void printVoltage(unsigned int);
    void printCellSummary(unsigned int);

    public:
    Cell(OutputHandler *, byte, byte, byte, byte);
    void process();
};

#endif
