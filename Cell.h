#ifndef CELL_H
#define CELL_H

#define LIION_MAX_VOLTAGE 4500
#define LIION_MIN_VOLTAGE 2600
#define NIMH_MAX_VOLTAGE 1700
#define NIMH_MIN_VOLTAGE 900
#define ABSOLUTE_MIN_VOLTAGE 200
#define LOAD_RESISTANCE 3.3d
#define REF_VOLTAGE 5000
#define SPEAKER_PIN 12

enum class CellStatus {DETECTING_TYPE, DISCHARGING, DONE};
enum class CellType {LI_ION, NI_MH};

class Cell {
    CellStatus cellStatus = CellStatus::DETECTING_TYPE;
    CellType cellType = CellType::LI_ION;
    const byte rowNumber;
    const byte pinVoltage;               // Analog sensor pin for reading battery voltage
    const byte pinFetVoltage;            // Analog sensor pin to read voltage across FET
    const byte dischargeControlPin;      // Output Pin that controlls the load for this battery
    unsigned long charge = 0;            // Total microamp hours for this battery
    unsigned long prevTime = 0;          // Previous time reading (in milliseconds)
    unsigned long lastGoodSample;
    unsigned long lastBadSample;
    unsigned int cellVoltage;
    unsigned int internalResistance;

    unsigned int readCellVoltage();

    public:
    Cell(byte, byte, byte, byte);
    void process();

    CellStatus getCellStatus();
    CellType getCellType();
    byte getRowNumber();
    unsigned long getCharge();
    unsigned int getCellVoltage();
    unsigned int getInternalResistance();
};

#endif
