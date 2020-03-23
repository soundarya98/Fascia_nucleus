/** \file NovaXR_Library.cpp
* \brief    Project:  NovaXR
*    
* Library for:  NovaXR Device - Higher Level APIs 
* \author   Ioannis Smanis
* \version  1.16 
* \date     01/08/2020 
* 
* Contact Info:  smanismech@gmail.com
* 
*/

#include "NovaXR_Library.h"



/**
 * Initialization of the selected NovaXR PCB Revision 
 *                
 * @return True if wrong/unknown NovaXR PCB version is selected
 * 
 */
bool Config_Board_Rev(float HW_Board_Rev){
    Serial.print("PCB Rev.: ");Serial.println(HW_Board_Rev);
    bool error_occured = false;
    uint8_t PCB_id = HW_Board_Rev*10;
    switch(PCB_id){  
      
        case 01:
          Serial.println("\n..... DEV BOARD MKR1010 CONFIGS loaded ... ");
        	Serial.println("Dev Board: Not implemented");
          error_occured =true;
        break; 
       

        case 10:
            Serial.println("NovaXR PCB V1: Not implemented");
          error_occured =true;            
        break; 

        case 11:
            Serial.println("NovaXR V1.1 + S1");
            // ----- Start the ADD1299 Module -----------------------------------
            Begin_ADS();
            delay(600);
      
            // ----- EDA Initialization
            test_ADC_init();
      
            // ----- PPG Sensor Initialization
            //PPG_Init();
      
            // ---- Write to Device ROM memeory Device information  ------------- 
            ROM_Init();
            
            // ---- Select ADS1299 if multiple are connected --------------------  
            ADS_Init_Config(MAIN_BOARD);
            delay(500);
          
            ADS_Init_Config(SISTER_BOARD); 
            delay(500);
      
        break;

        case 20:

            Serial.println("NovaXR V2");
            // ----- Start the ADD1299 Module -----------------------------------
            Begin_ADS();
            delay(1000);
      
            // ----- EDA Initialization
            MCU_ADC_Init();
            
            // ----- PPG Sensor Initialization
            //PPG_Init();
      
            // ---- Write to Device ROM memeory Device information  ------------- 
            ROM_Init();
            
            // ---- Select ADS1299 if multiple are connected --------------------  
            ADS_Init_Config(MAIN_BOARD);
            delay(500);
          
            ADS_Init_Config(SISTER_BOARD); 
            delay(500);
      
        break;


        case 21:
            Serial.println("NovaXR Mainboard PCB: V2.1 + Sisterboard PCB V2.1");
            Serial.println("Not existing compatible configuration");
            error_occured =true;
        break; 
          
        default:
           Serial.println("Wrong Board Selection");
           error_occured =true;
        break;  

    }

  return error_occured;
}
