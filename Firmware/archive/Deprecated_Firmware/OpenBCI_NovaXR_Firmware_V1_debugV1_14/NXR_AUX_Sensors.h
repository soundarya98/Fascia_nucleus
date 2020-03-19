#ifndef NXR_Int_Analog_Sensors_H
#define NXR_Int_Analog_Sensors_H



#include "Arduino.h"

#define VDD                  3.3
#define BATTVOLT             4.2
#define ADC_RES              4096
#define ADC_READS_SHIFT      1
#define ADC_READS_FAST       8
#define ADC_READS_COUNT      (1 << ADC_READS_SHIFT)
#define ADC_MIN_GAIN         0x0400
#define ADC_UNITY_GAIN       0x0800
#define ADC_MAX_GAIN         (0x1000 - 1)
#define ADC_RESOLUTION_BITS  12
#define ADC_RANGE            (1 << ADC_RESOLUTION_BITS)
#define ADC_TOP_VALUE        (ADC_RANGE - 1)
#define MAX_TOP_VALUE_READS  10


// =============== External API Definitions =============== //
extern const uint8_t VDD_ref_pin;
extern const uint8_t GND_ref_pin;
extern const uint8_t BATT_Sense_pin;
extern const uint8_t EDA_Sense_pin;

// =============== Prototypes ============================= //
    void MCU_ADC_Init(void); 
    void MCU_ADC_Default(void);       
    void MCU_ADC_Calibration(void);
    void test_ADC_init();
     int offSet_Correction(void);
uint16_t gain_Correction(void);
uint16_t readGND_Level(void);
uint16_t readVDD_Level(void);
uint16_t get_AnalogSense_Level(const uint8_t analogPin);
   float calc_Sensory_Output(uint16_t sensor_value, float voltage_ref);
uint8_t getBattery_ChargeLevel(const uint8_t BATT_sensePin, float Batter_ref_volt );

// ---- demo -----
void test_ADC_init();



#endif
