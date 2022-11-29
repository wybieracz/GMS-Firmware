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
  first = true;
}

// String kWhMem;
//     String resetMem;
//     short int reset;
//     String periodMem;
//     short int period;
//     String lastDayMem;
//     short int lastDay;

void EnergyManager::init() {
  kWhMem = flashManager.read(KWH_PATH);
  if(kWhMem=="") kWhMem = "0.0";
  kWh = atof(kWhMem.c_str());
  Serial.print("START KWH: ");
  Serial.println(kWh, 5);

  resetMem = flashManager.read(RESET_PATH);
  if(resetMem=="") resetMem = "1";
  reset = atoi(resetMem.c_str());
  Serial.print("START RESET: ");
  Serial.println(reset);

  periodMem = flashManager.read(PERIOD_PATH);
  if(periodMem=="") periodMem = "1";
  period = atoi(periodMem.c_str());
  Serial.print("START PERIOD: ");
  Serial.println(period);

  lastDayMem = flashManager.read(LAST_DAY_PATH);
  if(lastDayMem=="") lastDayMem = "1";
  lastDay = atoi(lastDayMem.c_str());
  Serial.print("START LASTDAY: ");
  Serial.println(lastDay);
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
  Vrms = vRatio * sqrt(sumV / numberOfSamples);

  if(relayManager.state) {
    Irms = iRatio * sqrt(sumI / numberOfSamples);
    //Calculation power values
    realPower = vRatio * iRatio * sumP / numberOfSamples;
    apparentPower = Vrms * Irms;
    powerFactor = realPower / apparentPower;
  } else {
    Irms = 0.0;
    realPower = 0.0;
    apparentPower = 0.0;
    powerFactor = 0.0;
  }
  

  //Reset accumulators
  sumV = 0;
  sumI = 0;
  sumP = 0;
  if(!first) {
    dbV = dbV + Vrms;
    dbI = dbI + Irms;
    dbP = dbP + apparentPower;
    kWh = kWh + apparentPower*(millis()-lastmillis)/3600000000.0;
    sampleCount++;
  }
  lastmillis = millis();
}

void EnergyManager::print() {

  Serial.print("Vrms: ");
  Serial.print(Vrms, 2);
  Serial.print("V");

  Serial.print("\tIrms: ");
  Serial.print(Irms, 4);
  Serial.print("A");
  
  Serial.print("\tPower: ");
  Serial.print(apparentPower, 4);
  Serial.print("W");
  
  Serial.print("\tkWh: ");
  Serial.print(kWh, 5);
  Serial.println("kWh");
  lastmillis = millis();
}

void EnergyManager::displayTime(int row) {
  lcdManager.print(timeManager.getDataString(true, false), 0, row);
}

void EnergyManager::displayVI(int row) {
  if(Vrms < 10.0) {
    lcdManager.print("V:00", 0, row);
    lcdManager.printFloat((float)Vrms, 2, 4, row);
  } else if(Vrms < 100.0) {
    lcdManager.print("V:0", 0, row);
    lcdManager.printFloat((float)Vrms, 2, 3, row);
  } else {
    lcdManager.print("V:", 0, row);
    lcdManager.printFloat((float)Vrms, 2, 2, row);
  }

  if(Irms < 10.0) {
    lcdManager.print("I:0", 9, row);
    lcdManager.printFloat((float)Irms, 2, 12, row);
  } else {
    lcdManager.print("I:", 9, row);
    lcdManager.printFloat((float)Irms, 2, 11, row);
  }
}

void EnergyManager::displayP(int row) {
  if(Vrms < 10.0) {
    lcdManager.print("P:000", 0, row);
    lcdManager.printFloat((float)Vrms, 2, 5, row);
  } else if(Vrms < 100.0) {
    lcdManager.print("P:00", 0, row);
    lcdManager.printFloat((float)Vrms, 2, 4, row);
  } else if(Vrms < 1000.0) {
    lcdManager.print("P:", 0, row);
    lcdManager.printFloat((float)Vrms, 2, 3, row);
  } else {
    lcdManager.print("P:", 0, row);
    lcdManager.printFloat((float)apparentPower, 2, 2, row);
  }
}

void EnergyManager::displaykWh(int row) {
  lcdManager.print("kWh:", 0, row);
  lcdManager.printFloat(kWh, 5, 4, row);
}

void EnergyManager::display() {
  char* settings = lcdManager.getSettings();
  lcdManager.clear();
  for(int i = 0; i < 2; i++) {
    switch(settings[i]) {
      case '1': displayTime(i); break;
      case '2': displayVI(i); break;
      case '3': displayP(i); break;
      case '4': displaykWh(i); break;
      default: displayTime(i); break;
    }
  }
}

void EnergyManager::calcAvg() {
  if(sampleCount > 0) {
    avgV = dbV / sampleCount;
    avgI = dbI / sampleCount;
    avgP = dbP / sampleCount;
  }
  dbV = 0.0;
  dbI = 0.0;
  dbP = 0.0;
  sampleCount = 0; 
}

void EnergyManager::saveKWh() {
  char result[64];
  dtostrf(kWh, 1, 8, result);
  Serial.print("ZAPIS KWH:::");
  Serial.println(result);
  flashManager.write(KWH_PATH, result);
}

void EnergyManager::resetKWh() {
  kWh = 0.0;
  flashManager.write(KWH_PATH, "0.0");
}

bool EnergyManager::setReset(char status) {

  if(status == 48) {
    reset = false;
    flashManager.write(RESET_PATH, "0");
    return true;
  }

  if(status == 49) {
    reset = true;
    flashManager.write(RESET_PATH, "1");
    return true;
  }

  return false;
}

bool EnergyManager::setPeriod(char* data) {
  int temp = atoi(data);
  if(temp < 0 && temp > 28) return false;
  period = temp;
  flashManager.write(PERIOD_PATH, data);
  return true;
}

void EnergyManager::setLastDay(int today) {
  char temp[3];
  timeManager.refresh(false);
  lastDay = timeManager.get.tm_mday;
  itoa(lastDay, temp, 10);
  flashManager.write(LAST_DAY_PATH, temp);
}

void EnergyManager::checkPeriod() {
  timeManager.refresh(false);
  int today = timeManager.get.tm_mday;
  if(reset && today == period && today != lastDay) {
    resetKWh();
  }
  if(today != lastDay) {
    setLastDay(today);
  }
  saveKWh();
}

bool EnergyManager::isFirstRead() {
  if(first) {
    first = false;
    return true;
  }
  return false;
}

EnergyManager energyManager;
