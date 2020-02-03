/*
 * Author:       Ioannis Smanis
 * Library for:  EDA sensor and Battery Level sensor
 * Project:      NovaXR
 * Date:         11/5/2019
 * 
 */

#include "NXR_AUX_Sensors.h"
#include "SAMD_AnalogCorrection.h"

#define DEBUG

// -------------- Global Variables  ---------------------------------------------- //
int offsetCorrectionValue = 0;
uint16_t gainCorrectionValue   = ADC_UNITY_GAIN;



// -------------- Initialize SAMD21 ADC Configs --------------------------------- //
void MCU_ADC_Init(void){
    MCU_ADC_Calibration();
}


void test_ADC_init(){
  analogReadResolution(ADC_RESOLUTION_BITS);
  analogReference(AR_DEFAULT);
}



// -------------- Calculate Sensory Voltgae Output ---------------------------------- //
float calc_Sensory_Output(uint16_t sensor_value, float voltage_ref){
   float calc_voltage = sensor_value * (float)( voltage_ref /(float)ADC_RES);
   return calc_voltage; 
}



// -------------- Calbrate 12Bit ADC of SAMD21 -------------------------------------- //
void MCU_ADC_Calibration(void){
  // ============= Set Default Settings ===================================== //
  MCU_ADC_Default();
  
  // =============  Correct ADC OffSet ====================================== //
  offSet_Correction();
  
  // =============  Correct ADC Gain  ======================================= //
  gain_Correction();

  // ============  Set corrected OffSet and Gain for SAMD21 ADC ============= //
  analogReadCorrection(offsetCorrectionValue, gainCorrectionValue);
   #ifdef DEBUG
      Serial.println(" =========================================== ");
      Serial.println(" ******* Readings After Corrections ******** ");
      Serial.print(" GND Level: ");Serial.println(readGND_Level());
      Serial.print(" VDD Level: ");Serial.println(readVDD_Level());
      Serial.println(" ************ Correction Values ************ ");
      Serial.print(" Offset = "); Serial.println(offsetCorrectionValue);
      Serial.print(" Gain   = "); Serial.println(gainCorrectionValue);
      Serial.println(" =========================================== ");
   #endif 
}



// -------------- Set Default ADC Settigns Initially -------------------------------- //
void MCU_ADC_Default(void){

    analogReadResolution(ADC_RESOLUTION_BITS);
    uint16_t GND_val = readGND_Level();
    float groundVolt = calc_Sensory_Output(GND_val,VDD);
    
    uint16_t VDD_val = readVDD_Level();
    float vddVolt =calc_Sensory_Output(VDD_val,VDD);
    
    // ....... Set default correction values and enable correction .....
    analogReadCorrection(offsetCorrectionValue, gainCorrectionValue);
  
    #ifdef DEBUG
        Serial.println("\r\nReading GND and VDD ADC levels");
        Serial.print("GND Level: "); 
        Serial.print(groundVolt); 
        Serial.print("V\t=> RAW: "); 
        Serial.println(GND_val);    
        Serial.print("VDD Level: ");
        Serial.print(vddVolt,4);
        Serial.print("V\t=> RAW: ");
        Serial.println(VDD_val);
    #endif
}



// ----------------- Perform OffSet Correction --------------------------------------- //
int offSet_Correction(void){

    #ifdef DEBUG
        Serial.print("Offset correction using Default Gain = ");Serial.println(gainCorrectionValue);
        Serial.println("\r\n===== Offset Correction ================");
    #endif
    
    for (int offset = 0; offset < (int)(ADC_OFFSETCORR_MASK >> 1); ++offset){       
        analogReadCorrection(offset, gainCorrectionValue);
        uint16_t lowLevelread = readGND_Level();
        
       #ifdef DEBUG
          Serial.print("\r Offset: ");
          Serial.print(offset);    
          Serial.print("  -> GND Level:  ");
          Serial.println(lowLevelread);
       #endif  
       
        if (lowLevelread == 0){
           offsetCorrectionValue = offset;
           break;
        }
    }
    Serial.println();

  return offsetCorrectionValue;
}



// ----------------- Perform Gain Correction --------------------------------------- //
uint16_t gain_Correction(void){

    uint8_t  topValueReadsCount = 0U;
    uint16_t minGain = 0U;
    uint16_t maxGain = 0U;
    uint16_t highLevelRead = 0;
    
    analogReadCorrection(offsetCorrectionValue, gainCorrectionValue);
    highLevelRead = readVDD_Level();
  
    #ifdef DEBUG
      Serial.println("\r===== Gain Correction ================");
      Serial.print("\r Gain: ");
      Serial.print(gainCorrectionValue);
      Serial.print(" -> VDD Level: ");
      Serial.println(highLevelRead);
    #endif  
  
    if (highLevelRead < ADC_TOP_VALUE) {
         for (uint16_t gain = ADC_UNITY_GAIN + 1; gain <= ADC_MAX_GAIN; ++gain) {    
              analogReadCorrection(offsetCorrectionValue, gain);
              highLevelRead = readVDD_Level();
            
              #ifdef DEBUG
                  Serial.print(" Gain: ");
                  Serial.print(gain);
                  Serial.print(" -> VDD Level: ");
                  Serial.println(highLevelRead);
              #endif 
              
              if (highLevelRead == ADC_TOP_VALUE) {
              
                 if (minGain == 0U)minGain = gain;
                 if (++topValueReadsCount >= MAX_TOP_VALUE_READS) {
                    maxGain = minGain;
                    break;
                 }  
                 maxGain = gain;
              }
              if (highLevelRead > ADC_TOP_VALUE)break;
         }  
         
     }else if (highLevelRead >= ADC_TOP_VALUE) {    
         if (highLevelRead == ADC_TOP_VALUE)maxGain = ADC_UNITY_GAIN;  
         for (uint16_t gain = ADC_UNITY_GAIN - 1; gain >= ADC_MIN_GAIN; --gain) {
           
            analogReadCorrection(offsetCorrectionValue, gain);
            highLevelRead = readVDD_Level();
            
            #ifdef DEBUG
                Serial.print(" Gain: ");
                Serial.print(gain);
                Serial.print(" -> VDD Level: ");
                Serial.println(highLevelRead);
            #endif 
            
            if (highLevelRead == ADC_TOP_VALUE){
              if (maxGain == 0U)maxGain = gain;     
              minGain = gain;
            }
      
            if (highLevelRead < ADC_TOP_VALUE) break;
          }   
     } 
     gainCorrectionValue = (minGain + maxGain) >> 1;
    
  return gainCorrectionValue;
}



// -------------- Read Analog Sensor Level ------------------------ //
uint16_t get_AnalogSense_Level(const uint8_t analogPin) {  
    uint32_t readAccumulator = 0;
    for (int i = 0; i < ADC_READS_COUNT; ++i) {
      readAccumulator += analogRead(analogPin);
    }
    uint16_t readValue = readAccumulator >> ADC_READS_SHIFT; 
    return readValue;
}



// -------------- Read Real Ground Voltgae Level ------------------ //
uint16_t readGND_Level(void) {  
    uint32_t readAccumulator = 0;
    for (int i = 0; i < ADC_READS_COUNT; ++i) {
      readAccumulator += analogRead(GND_ref_pin);
    }
    uint16_t readValue = readAccumulator >> ADC_READS_SHIFT; 
    return readValue;
}


// -------------- Read VDD Voltage Level ------------------------- //
uint16_t readVDD_Level(void) {
    uint32_t readAccumulator = 0;
    for (int i = 0; i < ADC_READS_COUNT; ++i){
      readAccumulator += analogRead(VDD_ref_pin);
    }   
     
    uint16_t readValue = readAccumulator >> ADC_READS_SHIFT;        
    if (readValue < (ADC_RANGE >> 1)){
      readValue += ADC_RANGE;
    }   
  return readValue;
}

// ------------- Get Battery Charge Percentage -------------------- //
uint8_t getBattery_ChargeLevel(const uint8_t BATT_sensePin, float Batter_ref_volt ){

   uint16_t Raw_Val = get_AnalogSense_Level(BATT_sensePin);
   float curr_Battery_Volt = calc_Sensory_Output(Raw_Val, Batter_ref_volt);
   uint8_t batt_percent = (curr_Battery_Volt / Batter_ref_volt)*100;

   return batt_percent; 
}
