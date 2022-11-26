/*
  Based on Emon Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
*/

#include "EnergyManager.h"

EnergyManager::EnergyManager() {
  offsetV = ADC_COUNTS >> 1;
  offsetI = ADC_COUNTS >> 1;
  vRatio = V_CO * (SYSTEM_VOLTAGE / ADC_COUNTS);
  iRatio = I_CO *(SYSTEM_VOLTAGE / ADC_COUNTS);
  kWh = 0;
  lastmillis = millis();
}

//--------------------------------------------------------------------------------------
// Calculates realPower,apparentPower,powerFactor,Vrms,Irms,kWh increment
// From a sample window of the mains AC voltage and current.
// The Sample window length is defined by the number of half wavelengths or crossings we choose to measure.
//--------------------------------------------------------------------------------------
void EnergyManager::calc(unsigned int crossings, unsigned int timeout) {

  unsigned int crossCount = 0;                             //Used to measure number of times threshold is crossed.
  unsigned int numberOfSamples = 0;                        //This is now incremented

  //-------------------------------------------------------------------------------------------------------------------------
  // 1) Waits for the waveform to be close to 'zero' (mid-scale adc) part in sin curve.
  //-------------------------------------------------------------------------------------------------------------------------
  unsigned long start = millis();    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.

  while(1) {
    startV = analogRead(V_IN);                    //using the voltage waveform
    if ((startV < (ADC_COUNTS*0.55)) && (startV > (ADC_COUNTS*0.45))) break;  //check its within range
    if ((millis()-start)>timeout) break;
  }

  //-------------------------------------------------------------------------------------------------------------------------
  // 2) Main measurement loop
  //-------------------------------------------------------------------------------------------------------------------------
  start = millis();

  while ((crossCount < crossings) && ((millis()-start)<timeout)) {
    numberOfSamples++;                       //Count number of times looped.
    lastFilteredV = filteredV;               //Used for delay/phase compensation

    //-----------------------------------------------------------------------------
    // A) Read in raw voltage and current samples
    //-----------------------------------------------------------------------------
    sampleV = analogRead(V_IN);                 //Read in raw voltage signal
    sampleI = analogRead(I_IN);                 //Read in raw current signal

    //-----------------------------------------------------------------------------
    // B) Apply digital low pass filters to extract the 2.5 V or 1.65 V dc offset,
    //     then subtract this - signal is now centred on 0 counts.
    //-----------------------------------------------------------------------------
    offsetV = offsetV + ((sampleV-offsetV)/1024);
    filteredV = sampleV - offsetV;
    offsetI = offsetI + ((sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;

    //-----------------------------------------------------------------------------
    // C) Root-mean-square method voltage
    //-----------------------------------------------------------------------------
    sqV= filteredV * filteredV;                 //1) square voltage values
    sumV += sqV;                                //2) sum

    //-----------------------------------------------------------------------------
    // D) Root-mean-square method current
    //-----------------------------------------------------------------------------
    sqI = filteredI * filteredI;                //1) square current values
    sumI += sqI;                                //2) sum

    //-----------------------------------------------------------------------------
    // E) Phase calibration
    //-----------------------------------------------------------------------------
    phaseShiftedV = lastFilteredV + PHASE_SHIFT * (filteredV - lastFilteredV);

    //-----------------------------------------------------------------------------
    // F) Instantaneous power calc
    //-----------------------------------------------------------------------------
    instP = phaseShiftedV * filteredI;          //Instantaneous Power
    sumP +=instP;                               //Sum

    //-----------------------------------------------------------------------------
    // G) Find the number of times the voltage has crossed the initial voltage
    //    - every 2 crosses we will have sampled 1 wavelength
    //    - so this method allows us to sample an integer number of half wavelengths which increases accuracy
    //-----------------------------------------------------------------------------
    lastVCross = checkVCross;

    if (sampleV > startV) checkVCross = true;
    else checkVCross = false;

    if (numberOfSamples==1) lastVCross = checkVCross;
    if (lastVCross != checkVCross) crossCount++;
  }

  //-------------------------------------------------------------------------------------------------------------------------
  // 3) Post loop calculations
  //-------------------------------------------------------------------------------------------------------------------------
  //Calculation of the root of the mean of the voltage and current squared (rms) with calibration coefficients.

  //double V_RATIO = V_CO *(SYSTEM_VOLTAGE / ADC_COUNTS);
  Vrms = vRatio * sqrt(sumV / numberOfSamples);

  //double I_RATIO = I_CO *(SYSTEM_VOLTAGE / ADC_COUNTS);
  Irms = iRatio * sqrt(sumI / numberOfSamples);

  //Calculation power values
  realPower = vRatio * iRatio * sumP / numberOfSamples;
  apparentPower = Vrms * Irms;
  powerFactor=realPower / apparentPower;

  //Reset accumulators
  sumV = 0;
  sumI = 0;
  sumP = 0;
  kWh = kWh + apparentPower*(millis()-lastmillis)/3600000000.0;
  lastmillis = millis();
}

void EnergyManager::print() {
  //calcVI(20, 1000);
  lcdManager.clear();
  Serial.print("Vrms: ");
  lcdManager.print("Vrms: ", 0, 0);
  Serial.print(Vrms, 2);
  lcdManager.printFloat((float)Vrms, 2, 6, 0);
  Serial.print("V");
  
  Serial.print("\tIrms: ");
  lcdManager.print("Irms: ", 0, 1);
  Serial.print(Irms, 4);
  lcdManager.printFloat((float)Irms, 2, 6, 1);
  Serial.print("A");
  
  Serial.print("\tPower: ");
  Serial.print(apparentPower, 4);
  Serial.print("W");
  
  Serial.print("\tkWh: ");
  kWh = kWh + apparentPower*(millis()-lastmillis)/3600000000.0;
  Serial.print(kWh, 4);
  Serial.println("kWh");
  lastmillis = millis();
}

EnergyManager energyManager;

// void EnergyManager::serialprint() {
//   Serial.print(realPower);
//   Serial.print(' ');
//   Serial.print(apparentPower);
//   Serial.print(' ');
//   Serial.print(Vrms);
//   Serial.print(' ');
//   Serial.print(Irms);
//   Serial.print(' ');
//   Serial.print(powerFactor);
//   Serial.println(' ');
//   delay(100);
// }
