/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V1.X board
   Firmware:            Version 1.14
   Device Mode:         Infrastructure Mode or AP Mode
   Data Transmission Protocol: TCP/UDP
   Release Date:        01/08/2020
*/

#include "NovaXR_Library.h"


#define DEBUG 

                // ============================================================== //
                // ------- Nova XR Hardware Revision oriewnted firmware --------- //
                // ============================================================== //

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
           
      error_flag = set_board_revision();
  
}





void loop() {
    
      //ADS_Example_2_Single_Shot();
      //ADS_MB_Interrupt(MAIN_BOARD,3000000);
      if (!error_flag)UDP_or_TCP_data_exchage();
}







 
