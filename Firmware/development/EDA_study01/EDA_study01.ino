
float Vout =0;
float EDApin = 0;
String voltageOut;
int R_ref = 820000;
float R_skin = 0.f;
float I = 0.f;
float Vdrop = 0.f;
void setup() {
  voltageOut = String("");

}

void loop() {
  
//  analogReadResolution(12);
//  EDApin = analogRead(A0);
//
//  Vout = EDApin / 4095.f * 3.3;
//  delay(100);
//  Serial.print("Voltage Out: "); 
//  Serial.print(Vout);
//  Vdrop =  (3.3f-Vout);
//  Serial.print(" Vdrop: "); 
//  Serial.print(Vdrop);
//  I = (Vdrop / R_ref);
////  Serial.print(" Current: "); 
////  Serial.print(I,6);
//  R_skin = R_ref * (1 / ((3.3/Vout) -1));
//  Serial.print(" R_skin1: "); 
//  Serial.print(R_skin,6);
//   R_skin = (1-((2*Vout)/3.3))*R_ref;
//  Serial.print(" R_skin2: "); 
//  Serial.println(R_skin,6);

  EDApin = getSkinConductance();
  R_skin = getSkinResistance();
 //Serial.print("EDA :");
  Serial.println(EDApin);
  //Serial.print(",");
  //Serial.print("R_Skin :");
  //Serial.println(R_skin);
  
  


}

float getSkinConductance(){
  float resistance;
  float conductance;
  delay(1);

  analogReadResolution(12);

  // Read an analogic value from analogic2 pin.
  float sensorValue = analogRead(A0);
  float voltage = (sensorValue*3.3)/4095;
  conductance = 2*((voltage - 0.5) / 100000);

  // Conductance calculation
    resistance = 1 / conductance; 
    conductance = conductance * 1000000;
    delay(1);
    
    if (conductance > 1.0)  return conductance;
    else return -1.0;
  }
float getSkinResistance(){
  float resistance;
  float conductance;
  analogReadResolution(12);

  float sensorValue = analogRead(A0);
  float voltage = (sensorValue * 3.3)/4095;

  delay(1);
  conductance = 2*((voltage - 0.5)/ 100000);

  resistance = 1/conductance;
  delay(1);

  if (resistance > 1.0) return resistance;
  else return -1.0;
  
}

float getRskin() {
  analogReadResolution(12);
  float sensorValue = analogRead(A0);
  float Vout = (sensorValue * 3.3)/4095;

  // these are constants- should not change between iterations
  // values are from the PCB layout/schematic in Fascia Physio Board V0
  float Rref = 875000.; // reference resistor between - opamp and gnd
  // Rref might actually be 2M or 820K -- undocumented board build value
  float Vref = 3.3 * 20./(20.+140.); // voltage divider output (virtual gnd)
  float i = (float)Vref / (float)Rref;

  float Rskin = (Vout - Vref) / i;
  float Cskin = 1./Rskin;

  return Rskin;
}
