#include "SAMD_AnalogCorrection.h"

const int EDASignal=A0;
int sensorValue=0;
int gsr_average=0;
int raw=0;

void setup(){
  Serial.begin(115200);
}

void loop(){
  long sum=0;
  for(int i=0;i<10;i++)           //Average the 10 measurements to remove 
      {
      sensorValue=analogRead(EDASignal);
      sum += sensorValue;

      delay(1);
      }
      
   gsr_average = sum/10;

   raw=analogRead(EDASignal);

   //Serial.print(raw);
  // Serial.print(",");
   Serial.println(gsr_average);

}
