/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V1.X board
   Firmware:            Version 1.13.2
   Device Mode:         Infrastructure Mode or AP Mode
   Data Transmission Protocol: TCP/UDP
   Release Date:        11/2/2019
*/


#include "NXR_Flash.h"
#include "WiFi_Management.h"
#include "ADS1299_lib.h"
#include "NXR_Communication.h"
#include "LED_Indication.h"

#include "NXR_AUX_Sensors.h"
#include "NovaXR_PPG_Sensor.h"


#define DEBUG 
#define DEV_BOARD     0  // Developemnt Board based on MKR1010 - set to '1' to enable or '0' to disable
#define BOARD_V1      0  // NovaXR Board V1   - config - set to '1' to enable or '0' to disable
#define BOARD_V1_1    0  // NovaXR Board V1.1 (V1.1 SISTER) - config - set to '1' to enable or '0' to disable
#define BOARD_V1_1_S1 1  // NovaXR Board V1.1 (V1 SISTER)   - config - set to '1' to enable or '0' to disable
#define BOARD_V2      0  // NovaXR Board V2   - config - set to '1' to enable or '0' to disable


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

    #elif BOARD_V1_1_S1 //%%%%%%%%%%%%%%% WARNING: USE Sister Board V1 ONLY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V1.1 ==================
        // *************************** NOVA XR BOARD V1.1  *******************************       
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        const uint8_t MainCS      = PA21; //  Chip Select pin  for main board
        const uint8_t MainDRDY    = PA10; //  Data Ready pin for main board  - Hardware Interrupt Ready
        const uint8_t ResetMB_pin = PA23; //  ADC Reset Pin for MainBoard
            
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS       = PA20; //  Chip Select pin  for sister board
        const uint8_t SisDRDY     = PA10; //  Data Ready pin for sister board 
        const uint8_t ResetSB_pin = PA23; //  ADC Reset Pin for SisterBoard
        
    #elif BOARD_V1_1  //%%%%%%%%%%%%%%% WARNING: USE Sister Board V1.1 or later %%%%%%%%%%%%%%%%%%%%%%%%%%%%  
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

        #define PA02 15 // =========== NXR1: A0 - EDA Signal
        #define PA05 19 // =========== NXR1: A4 - VBATT Level 
        #define PB09 32 // =========== NXR1: A2 
        #define PA06 20 // =========== NXR1: D8 - A6
        
        const uint8_t VDD_ref_pin    = PB09;
        const uint8_t GND_ref_pin    = PA06;
        const uint8_t BATT_Sense_pin = PA05; 
        const uint8_t EDA_Sense_pin  = PA02;
    
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





void setup() {
      // ------- Initialize uBlox GPIOs ----------------------------------   
      uBlox_HW_Init();    
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
      #elif BOARD_V1_1 or BOARD_V1_1_S1
        Serial.println("\n..... NOVA XR BOARD V1.1 CONFIGS loaded ... ");
        pinMode(ResetMB_pin    ,OUTPUT); 
        pinMode(ResetSB_pin    ,OUTPUT);
        digitalWrite(ResetMB_pin, HIGH);
        digitalWrite(ResetSB_pin, HIGH);
      #elif DEV_BOARD
        Serial.println("\n..... DEV BOARD MKR1010 CONFIGS loaded ... ");
      #endif 
      
      // ----- Start the ADD1299 Module -----------------------------------
      Begin_ADS();
      delay(1000);

      // ----- EDA Initialization
      //MCU_ADC_Init();
      test_ADC_init();

      // ----- PPG Sensor Initialization
      PPG_Init();

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
      Serial.println("\n---------- FIRMWARE INSTRUCTIONS ------------"); 
      Serial.println(" Type 'U' for UDP or 'T' for TCP accordingly ");
      //Serial.println(" Type 'S' for UDP or 'R' TCP   Switch Buffer ");

      
}






void loop() {
    
      // ADS_Example_2_Single_Shot();
      //ADS_MB_Interrupt(MAIN_BOARD,3000000);
      UDP_or_TCP_data_exchage();

}







 
