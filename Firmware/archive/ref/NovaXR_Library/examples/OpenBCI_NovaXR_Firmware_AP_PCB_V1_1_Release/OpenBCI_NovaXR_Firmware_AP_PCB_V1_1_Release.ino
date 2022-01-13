/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V1.1 board
   Firmware:            Version 1.16
   Device Mode:         Infrastructure Mode or AP Mode
   Data Transmission Protocol: TCP/UDP
   Release Date:        01/13/2020
*/

#include "NovaXR_Library.h"


                // ============================================================== //
                // ------- Nova XR Hardware Revision oriewnted firmware --------- //
                // ============================================================== //


// ****************************** NOVA XR V1.1  *************************************
#define BOARD_REVISION_ID 1.1  ///< PCB Revision Number
#define WIFI_MODE         'A'  ///< WiFi Interface 'A' for Access Point Mode  , 'I' for Infrastructure Mode. 


//:::::::::::::: for Main board V1.1 ::::::::::::::::::::::::::
const uint8_t MainCS         = PA21; ///<  Chip Select pin  for main board
const uint8_t MainDRDY       = PA10; ///<  Data Ready pin for main board  - Hardware Interrupt Ready   
const uint8_t ResetMB_pin    = PA23; ///<  ADC Reset Pin for MainBoard
const uint8_t OTG_pin        = PA18; ///<  USB OTG Pin for MainBoard    

//:::::::::::::: for Sister board V1 ::::::::::::::::::::::::
const uint8_t SisCS          = PA20; ///<  Chip Select pin  for sister board
const uint8_t SisDRDY        = PA07; ///<  Data Ready pin for sister board 
const uint8_t ResetSB_pin    = PA23; ///<  ADC Reset Pin for SisterBoard   
const uint8_t VDD_ref_pin    = PB09; ///<  VDD for EDA Circuit 
const uint8_t GND_ref_pin    = PA06; ///<  GND for EDA Circuit 
const uint8_t BATT_Sense_pin = PA05; ///<  Battery Level Sense Signal
const uint8_t EDA_Sense_pin  = PA02; ///<s  EDA Sensor Sense Signal


bool error_flag = false;



void setup() {
     
      // ------- Initialize uBlox GPIOs ----------------------------------   
      uBlox_HW_Init();    
      powered_On_Indication(); // ------- Indicate Device is Powered Up correctly
      Serial.begin(230400);
      error_flag = Config_Board_Rev(BOARD_REVISION_ID);
      
      // ---- Choose: type of WiFi Connectivity of the Device -------------      
      WiFi_connectivityOption(WIFI_MODE);
}





void loop() {
  
      if (!error_flag)UDP_or_TCP_data_exchage();
}

