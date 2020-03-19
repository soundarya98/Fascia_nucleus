/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V1.X board
   Firmware:            Version 1.13
   Device Mode:         Infrastructure Mode or AP Mode
   Data Transmission Protocol: TCP/UDP
   Release Date:        11/2/2019
*/


#include "OpenBCI_NovaXR_Definitions.h"
#include "NXR_Communication.h"
#include "WiFi_Management.h"
#include "NXR_Flash.h"
#include "OpenBCI_NovaXR.h"
#include "HTML_Server.h"
#include "ADS1299_lib.h"


#define DEBUG 
#define DEV_BOARD   0  // Developemnt Board based on MKR1010 - set to '1' to enable or '0' to disable
#define BOARD_V1    0  // NovaXR Board V1   - config - set to '1' to enable or '0' to disable
#define BOARD_V1_1  1  // NovaXR Board V1.1 - config - set to '1' to enable or '0' to disable
#define BOARD_V2    0  // NovaXR Board V2   - config - set to '1' to enable or '0' to disable


                // ============================================================== //
                // ------- Nova XR Hardware Revision oriewnted firmware --------- //
                // ============================================================== //

// =========================== HARDWARE BOARD CONFIGURATIONS ========================================
    #if DEV_BOARD
        // ****************************** DEV BOARD - MKR1010 *****************************
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        const uint8_t MainCS    = PA21;   // 7 Chip Select pin  for main board
        const uint8_t MainDRDY  = PB10;   // 4 Data Ready pin for main board  - Hardware Interrupt Ready
        
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS     = PA11;   // 3 Chip Select pin  for sister board
        const uint8_t SisDRDY   = PB11;   // 5 Data Ready pin for sister board 

    #elif BOARD_V1
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V1 modified ==========
        // *************************** NOVA XR BOARD V1 Modified **************************
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        const uint8_t MainCS    = PA21; //  Chip Select pin  for main board
        const uint8_t MainDRDY  = PA10; //  Data Ready pin for main board  - Hardware Interrupt Ready
        
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS     = PA20; //  Chip Select pin  for sister board
        const uint8_t SisDRDY   = PA10; //  Data Ready pin for sister board 
        const uint8_t Reset_pin = PA23; //  ADC Reset Pin for both Boards
          
    #elif BOARD_V1_1  
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V1.1 ==================
        // *************************** NOVA XR BOARD V1.1  *******************************
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        const uint8_t MainCS      = PA21; //  Chip Select pin  for main board
        const uint8_t MainDRDY    = PA10; //  Data Ready pin for main board  - Hardware Interrupt Ready
        const uint8_t ResetMB_pin = PA23; //  ADC Reset Pin for MainBoard
            
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS       = PA20; //  Chip Select pin  for sister board
        const uint8_t SisDRDY     = PA07; //  Data Ready pin for sister board 
        const uint8_t ResetSB_pin = PA23; //  ADC Reset Pin for SisterBoard
    
    #elif BOARD_V2  
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V2 =================
        // *************************** NOVA XR BOARD V2  *********************************
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        const uint8_t MainCS      = PA21; //  Chip Select pin  for main board
        const uint8_t MainDRDY    = PB10; //  Data Ready pin for main board  - Hardware Interrupt Ready   
        const uint8_t ResetMB_pin = PA23; //  ADC Reset Pin for MainBoard
        const uint8_t OTG_pin     = PA18; //  USB OTG Pin for MainBoard    
        
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS       = PA20; //  Chip Select pin  for sister board
        const uint8_t SisDRDY     = PB02; //  Data Ready pin for sister board 
        const uint8_t ResetSB_pin = PA23; //  ADC Reset Pin for SisterBoard
    
    #endif 
// =====================================================================================================



bool countT=false;
bool countU=false;  


void setup() {
   
      powered_On_Indication(); // ------- Indicate Device is Powered Up correctly
      Serial.begin(230400);
      
      #ifdef DEBUG   // ----- Debuging Enable-----------------------------   
        while(!Serial);
        Serial.println("\n ^^^^^^^^^^  TCP/UDP  ^^^^^^^^^^ ");
      #endif 
          
      #if BOARD_V1
        Serial.println("\n..... NOVA XR BOARD V1 CONFIGS loaded ... ");
        pinMode(Reset_pin    ,OUTPUT); 
        digitalWrite(Reset_pin, HIGH);
      #elif BOARD_V1_1
        Serial.println("\n..... NOVA XR BOARD V1.1 CONFIGS loaded ... ");
        pinMode(ResetMB_pin    ,OUTPUT); 
        pinMode(ResetSB_pin    ,OUTPUT);
        digitalWrite(ResetMB_pin, HIGH);
        digitalWrite(ResetSB_pin, HIGH);
      #elif DEV_BOARD
        Serial.println("\n..... DEV BOARD MKR1010 CONFIGS loaded ... ");
      #endif 
      
      // ------- Initialize uBlox GPIOs ----------------------------------   
      uBlox_HW_Init();
      
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

      // ---- Choose: type of WiFi Connectivity of the Device -------------
      WiFi_connectivityOption();
   
      // ---- Instructions ------------------------------------------------
      Serial.println("*********************************************");
      Serial.println("\n---------- FIRMWARE INSTRUCTIONS ----------"); 
      Serial.println("  Type U for UDP or T for TCP accordingly  ");\
      
}





void loop() {
    
    char inCMD=0;
    if (Serial.available() > 0) {
        Serial.println("  Type U for UDP or T for TCP accordingly    ");
        // read the incoming byte:
        inCMD = Serial.read();
  
        if(inCMD =='U'){
            // ---- Start a UDP Server ------------------------------------------
            Serial.println("\n>>> Starting UDP Streamer....");            
            if(!countU)startUDP_Server(NETPORT);
            countU=true;
            inCMD=0; // Reset incoming Command
            while(1){
              long starT = micros();
              bool check1 = UDP_Data_Exchange();
              long endT = micros();
              if(check1){ Serial.print("UDP Session Duration: ");Serial.println(endT - starT); Serial.println(); break; }
            }
            
        }else if(inCMD =='T'){
            // ---- Start a TCP Server ------------------------------------------
            Serial.println("\n>>> Starting TCP Streamer....");          
            if(!countT)startTCP_Server(NETPORT); 
            countT=true;
            inCMD=0; // Reset incoming Command         
            while(1){
              long starT = micros();
              bool check2 = TCP_Data_Exchange();
              long endT = micros();
              if(check2){ Serial.print("TCP Session Duration: ");Serial.println(endT - starT);Serial.println(); break; }
            }               
       }   
    }
    
 }















 
