/*
  Based on Emon Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
*/

#ifndef ENERGYMANAGER_H
#define ENERGYMANAGER_H

#include "PinManager.h"
#include "SerialLogger.h"
#include "FlashManager.h"
#include "LcdManager.h"
#include "TimeManager.h"
#include "RelayManager.h"

#define ADC_BITS    10
#define ADC_COUNTS  (1<<ADC_BITS)
#define V_CO 132.2
#define I_CO 3.72
#define PHASE_SHIFT 1.7
#define SYSTEM_VOLTAGE 3.3


class EnergyManager {
  private:
    void displayTime(int row);
    void displayVI(int row);
    void displayP(int row);
    void displaykWh(int row);
    void setLastDay(int today);
    void resetKWh();
    void saveKWh();

    int sampleV, sampleI;  // Holds the raw analog read value
    double vRatio, iRatio;
    double lastFilteredV, filteredV; // Filtered is the raw analog value minus the DC offset
    double filteredI;
    double offsetV, offsetI; // Low-pass filter output
    double phaseShiftedV; // Holds the calibrated phase shifted voltage.
    double sqV, sumV, sqI, sumI, instP, sumP; // sq = squared, sum = Sum, inst = instantaneous
    int startV; // Instantaneous voltage at start of sample window.
    bool lastVCross, checkVCross; // Used to measure number of times threshold is crossed.

    String kWhMem; // Uset to store values in flash
    String resetMem;
    String periodMem;
    String lastDayMem;
    short int lastDay;  // Day of last mesurement

    unsigned long lastmillis;
    double realPower, apparentPower, powerFactor, Vrms, Irms;

    //variables used to send avgs to db
    bool first; // ignore first period (unstable V, I read on startup)
    int sampleCount;
    double dbV, dbI, dbP;

  public:
    EnergyManager();
    void init();
    void calc(unsigned int crossings, unsigned int timeout);
    void print();
    void display();
    void calcAvg();
    bool setReset(char status);
    bool setPeriod(char* data);
    void checkPeriod();
    bool isFirstRead();

    double kWh;
    double avgV, avgI, avgP;

    short int period; // First day of period
    short int reset; // If reset kWh on first day of period
};

extern EnergyManager energyManager;

#endif //ENERGYMANAGER_H
