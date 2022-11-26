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

#define ADC_BITS    10
#define ADC_COUNTS  (1<<ADC_BITS)
#define V_CO 132.2
#define I_CO 3.72
#define PHASE_SHIFT 1.7
#define SYSTEM_VOLTAGE 3.3


class EnergyManager {
public:
  
  EnergyManager();

  void calc(unsigned int crossings, unsigned int timeout);
  void print();

  double realPower, apparentPower, powerFactor, Vrms, Irms;

private:

  int sampleV, sampleI;  // Holds the raw analog read value
  double vRatio, iRatio;
  double lastFilteredV, filteredV; // Filtered is the raw analog value minus the DC offset
  double filteredI;
  double offsetV, offsetI; // Low-pass filter output
  double phaseShiftedV; // Holds the calibrated phase shifted voltage.
  double sqV, sumV, sqI, sumI, instP, sumP; // sq = squared, sum = Sum, inst = instantaneous
  int startV; // Instantaneous voltage at start of sample window.
  bool lastVCross, checkVCross; // Used to measure number of times threshold is crossed.

  float kWh;
  unsigned long lastmillis;
};

extern EnergyManager energyManager;

#endif //ENERGYMANAGER_H
