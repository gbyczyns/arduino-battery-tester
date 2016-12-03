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

enum class CellStatus {NOT_INSTALLED, DETECTING_TYPE, MEASURING_RESISTANCE, DISCHARGING, DONE};
enum class CellType {LI_ION, NI_MH};

class Cell {
    OutputHandler * outputHandler;
    Animation * dischargingAnimation;
    Animation * doneAnimation;
    CellStatus cellStatus = CellStatus::DETECTING_TYPE;
    CellType cellType = CellType::LI_ION;
    const byte rowNumber;
    const byte pinVoltage;               // Analog sensor pin for reading battery voltage
    const byte pinFetVoltage;            // Analog sensor pin to read voltage across FET
    const byte dischargeControlPin;      // Output Pin that controlls the load for this battery
    unsigned long charge = 0;            // Total microamp hours for this battery
    unsigned long prevTime = 0;          // Previous time reading (in milliseconds)
    unsigned int goodSamplesCount = 0;   // number of good voltage readings (to determine battery installed)
    unsigned int badSamplesCount = 0;    // number of samples read below minimum (to determine battery discharged)
    unsigned int cellVoltage;

    void printSummary();

    public:
    Cell(OutputHandler *, byte, byte, byte, byte);
    void process();
};

#endif
