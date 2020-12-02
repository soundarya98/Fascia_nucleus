/*
 * Author:       Ioannis Smanis
 * Library Ver.: 0.1
 * Library for:  PPG sensor
 * Project:      NovaXR
 * Date:         11/28/2019
 * Module:       NovaXR_PPG_Sensor.h
 */
#ifndef NXR_PPG_Sensor_H
#define NXR_PPG_Sensor_H



// ------------- Prototypes --------------------
void PPG_Presence_Sense_Init(void);
bool getPresence(void);
void PPG_Temp_Sense_Init(void);
float get_Skin_Temp(bool CelciousFormat);
uint8_t get_Heart_Rate(void);
void PPG_HR_Sense_Init(void);
void PPG_Init(void);
void HR_config_Default(void);
void HR_config(void);
void HR_config_4(void);
void PPG_Presence_Config(void);
void PPG_Temp_Config(void);
void En_Int_FIFO_config(void);
uint8_t get_calculated_HR(long IR_Value);
void RealTime_Sensor_Calibration();
void adjust_configs(uint16_t configVal, char factor);
uint16_t read_return_int(void);
void Config_and_Calibrate(uint16_t Min, uint16_t Max, char command);
void printPPG_Configs(void);
void printInfo(void);





#endif
