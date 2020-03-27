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
  
  analogReadResolution(12);
  EDApin = analogRead(A0);

  Vout = EDApin / 4095.f * 3.3;
  delay(100);
  Serial.print("Voltage Out: "); 
  Serial.print(Vout);
  Vdrop =  (3.3f-Vout);
  Serial.print(" Vdrop: "); 
  Serial.print(Vdrop);
  I = (Vdrop / R_ref);
//  Serial.print(" Current: "); 
//  Serial.print(I,6);
  R_skin = R_ref * (1 / ((3.3/Vout) -1));
  Serial.print(" R_skin1: "); 
  Serial.print(R_skin,6);
   R_skin = (1-((2*Vout)/3.3))*R_ref;
  Serial.print(" R_skin2: "); 
  Serial.println(R_skin,6);
  


}
