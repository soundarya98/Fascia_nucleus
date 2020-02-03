/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V1 board
   Firmware:            Version 1.12
   Device Mode:         Infrastructure Mode
*/



#include "OpenBCI_NovaXR_Definitions.h"
#include "NXR_Communication.h"
#include "WiFi_Management.h"
#include "NXR_Flash.h"
#include "OpenBCI_NovaXR.h"
#include "HTML_Server.h"
#include "ADS1299_lib.h"


#define DEBUG
#define BOARD_V1      // NovaXR Board V1 - config - Comment this line for MKR1010 Dev Board







    // =========================== HARDWARE BOARD CONFIGURATIONS ======================================
#ifndef BOARD_V1

    // ****************************** DEV BOARD - MKR1010 *****************************
    //:::::::::::::: for Main board ::::::::::::::::::::::::::
    const uint8_t MainCS    = PA21;   // 7 Chip Select pin  for main board
    const uint8_t MainDRDY  = PB10;   // 4 Data Ready pin for main board  - Hardware Interrupt Ready
    
    //:::::::::::::: for Sister board ::::::::::::::::::::::::
    const uint8_t SisCS     = PA11;   // 3 Chip Select pin  for sister board
    const uint8_t SisDRDY   = PB11;   // 5 Data Ready pin for sister board 

#else   

    // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO A REAL NOVA XR BOARD ===================
    // *************************** NOVA XR BOARD V1 Modified **************************
    //:::::::::::::: for Main board ::::::::::::::::::::::::::
    const uint8_t MainCS    = PA20; //  Chip Select pin  for main board
    const uint8_t MainDRDY  = PA10; //  Data Ready pin for main board  - Hardware Interrupt Ready
    
    //:::::::::::::: for Sister board ::::::::::::::::::::::::
    const uint8_t SisCS     = PA21; //  Chip Select pin  for sister board
    const uint8_t SisDRDY   = PA10; //  Data Ready pin for sister board 
    const uint8_t Reset_pin = PA23; //  Reset Pin Connected to SAMD21 GPIO
    
#endif 
// ====================================================================================================







    
void setup() {

    Serial.begin(230400);
    
    #ifdef DEBUG   // ----- Debuging Enable-----------------------------   
      while(!Serial);
    #endif 

    #ifdef BOARD_V1
      Serial.println("..... NOVA XR BOARD CONFIGS loaded ... ");
      pinMode(Reset_pin    ,OUTPUT);
      digitalWrite(Reset_pin, HIGH);
    #else
      Serial.println("..... DEV BOARD MKR1010 CONFIGS loaded ... ");
    #endif 
    
    // ----- Start the ADD1299 Module -----------------------------------
    Begin_ADS();
    delay(1000);
    
    // ---- Write to Device ROM memeory Device information  ------------- 
    ROM_Init();
    
    // ---- Select ADS1299 if multiple are connected --------------------  
    ADS_Init_Config(MAIN_BOARD);
    delay(500);
    ADS_Init_Config(SISTER_BOARD); 
    delay(500);
    
    // ---- Manage WiFi Connectivity of the Device ----------------------
    WiFi_Management_Init();
 
    // ---- Start a UDP Server ------------------------------------------
    Serial.println("Starting Data Streamer....");
    startTCP_Server(NETPORT);

  
}





void loop() {
    
     bool test=false;
     uint8_t id=0;

     long starT = micros();
     test = data_Exchange();        
     long endT = micros();
     
     if(test){
        Serial.print("Session Duration: ");
        Serial.println(endT - starT);
     }
  

 }











 
