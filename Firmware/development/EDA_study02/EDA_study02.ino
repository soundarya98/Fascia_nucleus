
float Vout =0;
float EDApin = 0;
String voltageOut;
int R_ref = 820000;
float R_skin = 0.f;
float I = 0.f;
float Vdrop = 0.f;
void setup() {

}

void loop() {

  //EDApin = getCskin();
  R_skin = getRskin();
  //Serial.print("EDA :");
  Serial.println(R_skin);
//  Serial.print(",      ");
//  Serial.print("R_Skin :");
//  Serial.println(R_skin);
}

float getRskin() {
  analogReadResolution(12);
  float sensorValue = analogRead(A0);
  float Vout = (sensorValue * 3.3)/4095;

  // these are constants- should not change between iterations
  // values are from the PCB layout/schematic in Fascia Physio Board V0
  int Rref = 820000; // reference resistor between - opamp and gnd
  // Rref might actually be 2M or 820K -- undocumented board build value
  float Vref = 3.3 * 22./(22.+150.); // voltage divider output (virtual gnd)
  float i = (float)Vref / (float)Rref;

  float Rskin = (Vout - Vref) / i;
  float Cskin = 1./Rskin;

  return Rskin;
}


float getCskin() {
  analogReadResolution(12);
  float sensorValue = analogRead(A0);
  float Vout = (sensorValue * 3.3)/4095;

  // these are constants- should not change between iterations
  // values are from the PCB layout/schematic in Fascia Physio Board V0
  int Rref = 820000; // reference resistor between - opamp and gnd
  // Rref might actually be 2M or 820K -- undocumented board build value
  float Vref = 3.3 * 22./(22.+150.); // voltage divider output (virtual gnd)
  float i = (float)Vref / (float)Rref;

  float Rskin = (Vout - Vref) / i;
  float Cskin = 1./Rskin;
  Cskin =  Cskin * 1000000;
  return Cskin;
}
