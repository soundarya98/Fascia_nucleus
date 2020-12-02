/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V2.0 board
   Firmware:            Version 1.16
   Device Mode:         AP Mode
   Data Transmission Protocol: TCP/UDP
   Release Date:        01/13/2020
*/

#include "NovaXR_Library.h"

#define DEBUG 



                // ============================================================== //
                // ------- Nova XR Hardware Revision oriewnted firmware --------- //
                // ============================================================== //


// ****************************** NOVA XR V2  *************************************
#define BOARD_REVISION_ID 2.0  ///< PCB Revision Number
#define WIFI_MODE         'A'  ///< WiFi Interface 'A' for Access Point Mode  , 'I' for Infrastructure Mode. 


//:::::::::::::: for Main board V2 ::::::::::::::::::::::::::
const uint8_t MainCS         = PA21; ///<  Chip Select pin  for main board
const uint8_t MainDRDY       = PB10; ///<  Data Ready pin for main board  - Hardware Interrupt Ready   
const uint8_t ResetMB_pin    = PA23; ///<  ADC Reset Pin for MainBoard
const uint8_t OTG_pin        = PA18; ///<  USB OTG Pin for MainBoard    

//:::::::::::::: for Sister board V2 ::::::::::::::::::::::::
const uint8_t SisCS          = PA20; ///<  Chip Select pin  for sister board
const uint8_t SisDRDY        = PB02; ///<  Data Ready pin for sister board 
const uint8_t ResetSB_pin    = PA22; ///<  ADC Reset Pin for SisterBoard   
const uint8_t VDD_ref_pin    = PA06; ///<  VDD for EDA Circuit 
const uint8_t GND_ref_pin    = PB09; ///<  GND for EDA Circuit 
const uint8_t BATT_Sense_pin = PA05; ///<  Battery Level Sense Signal
const uint8_t EDA_Sense_pin  = PA02; ///<s  EDA Sensor Sense Signal


bool error_flag = false;


void setup() {
     
      // ------- Initialize uBlox GPIOs ----------------------------------   
      uBlox_HW_Init();    
      powered_On_Indication(); // ------- Indicate Device is Powered Up correctly
      Serial.begin(230400);
      
      #ifdef DEBUG   // ----- Debuging Enable-----------------------------   
        while(!Serial);
        Serial.println("\n ^^^^^^^^^^  UDP  ^^^^^^^^^^ ");
      #endif 
      error_flag = Config_Board_Rev(BOARD_REVISION_ID);
      
      // ---- Choose: type of WiFi Connectivity of the Device -------------      
      WiFi_connectivityOption(WIFI_MODE);
}





void loop() {
    
      //ADS_Example_2_Single_Shot();
      //ADS_Example_3_Single_Shot_Freq();
      //ADS_MB_Interrupt(MAIN_BOARD,3000000);
      if (!error_flag)UDP_or_TCP_data_exchage();
}







 
 
