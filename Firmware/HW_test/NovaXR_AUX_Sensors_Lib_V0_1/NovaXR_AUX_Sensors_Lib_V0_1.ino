
 
#include "NXR_AUX_Sensors.h"


#define DEBUG
//#define DEV_BOARD




#ifdef DEV_BOARD   // ------ Dev Board MKR1010
 
  const uint8_t VDD_ref_pin    = A2;  // Connect this Pin to the VDD of the target circuit 
  const uint8_t GND_ref_pin    = A1;  // Connect this Pin to the Ground of the target circuit 
  const uint8_t BATT_Sense_pin = 32; 
  const uint8_t EDA_Sense_pin  = A0;

#else // ------ Fascia Board V0
 
  #define PA02 15 // =========== NXR1: A0 - EDA Signal
  #define PA05 19 // =========== NXR1: A4 - VBATT Level 
  #define PB09 32 // =========== NXR1: A2 
  #define PA06 21 // =========== NXR1: D8 - A6
  
  const uint8_t VDD_ref_pin    = PA06;
  const uint8_t GND_ref_pin    = PB09;
  const uint8_t BATT_Sense_pin = PA05; 
  const uint8_t EDA_Sense_pin  = PA02;

#endif




void setup(){
  
  Serial.begin(9600);
  while(!Serial);
  Serial.println("\n *********** Test MCU ADC *********** ");
  test_ADC_init();
  
  #ifdef DEV_BOARD
    MCU_ADC_Init();
  #endif
  
}


void loop(){
  print_AnalogSensors();
}
