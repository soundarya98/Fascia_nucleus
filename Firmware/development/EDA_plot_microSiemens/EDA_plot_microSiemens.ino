#include "SAMD_AnalogCorrection.h"

const int EDASignal=A0;
float sensorValue=0;
float sensorValue1=0;

float eda_average=0;
int raw=0;

void setup(){

  Serial.begin(115200);
}

void loop(){
  float sum=0;
  for(int i=0;i<50;i++)           //Average the 50 measurements to remove 
      {
      sensorValue=getCskin();     // use getRskin() for resistance or getCskin() for conductance
      sum += sensorValue;

      delay(1);
      }
      
   eda_average = sum/50.f;
   Serial.println(eda_average,4);
}


float getCskin() {
  analogReadResolution(12);
  float sensorValue = analogRead(A0);
  float Vout = (sensorValue * 3.3)/4095;

  // these are constants- should not change between iterations
  // values are from the PCB layout/schematic in Fascia Physio Board V0
  int Rref = 820000; // reference resistor between - opamp and gnd
  // Rref might actually be 2M or 820K -- undocumented board build value
  float Vref = 3.3 * 20./(20.+140.); // voltage divider output (virtual gnd)
  float i = (float)Vref / (float)Rref;

  float Rskin = (Vout - Vref) / i;
  float Cskin = 1./Rskin;
  Cskin =  Cskin * 1000000;
  return Cskin;
}


float getRskin() {
  analogReadResolution(12);
  float sensorValue = analogRead(A0);
  float Vout = (sensorValue * 3.3)/4095;

  // these are constants- should not change between iterations
  // values are from the PCB layout/schematic in Fascia Physio Board V0
  int Rref = 820000; // reference resistor between - opamp and gnd
  // Rref might actually be 2M or 820K -- undocumented board build value
  float Vref = 3.3 * 20./(20.+140.); // voltage divider output (virtual gnd)
  float i = (float)Vref / (float)Rref;

  float Rskin = (Vout - Vref) / i;
  float Cskin = 1./Rskin;

  return Rskin;
}
