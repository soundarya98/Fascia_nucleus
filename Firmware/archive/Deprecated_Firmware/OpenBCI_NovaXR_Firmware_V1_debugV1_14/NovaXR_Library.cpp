/** \file NovaXR_Library.cpp
* \brief    Project:  NovaXR
*    
* Library for:  NovaXR Device - Higher Level APIs 
* \author   Ioannis Smanis
* \version  1.14  
* \date     01/08/2020 
* 
* Contact Info:  smanismech@gmail.com
* 
*/

 
#include "NovaXR_Library.h"



// -- WiFi Interface
#define WIFI_MODE 'A'   ///< 'A' for Access Point Mode  , 'I' for Infrastructure Mode. 

// -- Global Varaiables - Hardware Pin Settings 
#define DEBUG 
#define DEV_BOARD     0  ///< Developemnt Board based on MKR1010 - set to '1' to enable or '0' to disable
#define BOARD_V1      0  ///< NovaXR Board V1                    - config - set to '1' to enable or '0' to disable
#define BOARD_V1_1    0  ///< NovaXR Board V1.1 (V1.1 SISTER)    - config - set to '1' to enable or '0' to disable
#define BOARD_V1_1_S1 1  ///< NovaXR Board V1.1 (V1 SISTER)      - config - set to '1' to enable or '0' to disable
#define BOARD_V2      0  ///< NovaXR Board V2                    - config - set to '1' to enable or '0' to disable











                // ============================================================== //
                // ------- Nova XR Hardware Revision oriewnted firmware --------- //
                // ============================================================== //

// =========================== HARDWARE BOARD CONFIGURATIONS ========================================
    #if DEV_BOARD
        // ****************************** DEV BOARD - MKR1010 *****************************
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        uint8_t HW_Board_Rev    = 0;
        const uint8_t MainCS    = PA21;   ///< 7 Chip Select pin  for main board
        const uint8_t MainDRDY  = PB10;   ///< 4 Data Ready pin for main board  - Hardware Interrupt Ready
        
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS     = PA11;   ///< 3 Chip Select pin  for sister board
        const uint8_t SisDRDY   = PB11;   ///< 5 Data Ready pin for sister board 

    #elif BOARD_V1
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V1 modified ==========
        // *************************** NOVA XR BOARD V1 Modified **************************
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        uint8_t HW_Board_Rev    = 1;
        const uint8_t MainCS    = PA21; ///<  Chip Select pin  for main board
        const uint8_t MainDRDY  = PA10; ///<  Data Ready pin for main board  - Hardware Interrupt Ready
        
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS     = PA20; ///<  Chip Select pin  for sister board
        const uint8_t SisDRDY   = PA10; ///<  Data Ready pin for sister board 
        const uint8_t Reset_pin = PA23; ///<  ADC Reset Pin for both Boards

    #elif BOARD_V1_1_S1 //%%%%%%%%%%%%%%% WARNING: USE Sister Board V1 ONLY %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V1.1 ==================
        // *************************** NOVA XR BOARD V1.1  *******************************       
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        uint8_t HW_Board_Rev      = 2;
        const uint8_t MainCS      = PA21; ///<  Chip Select pin for main board.
        const uint8_t MainDRDY    = PA10; ///<  Data Ready pin for main board  - Hardware Interrupt Ready
        const uint8_t ResetMB_pin = PA23; ///<  ADC Reset Pin for MainBoard
            
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS       = PA20; ///<  Chip Select pin  for sister board
        const uint8_t SisDRDY     = PA10; ///<  Data Ready pin for sister board 
        const uint8_t ResetSB_pin = PA23; ///<  ADC Reset Pin for SisterBoard
        const uint8_t VDD_ref_pin    = PB09;
        const uint8_t GND_ref_pin    = PA06;
        const uint8_t BATT_Sense_pin = PA05; 
        const uint8_t EDA_Sense_pin  = PA02;
        
    #elif BOARD_V1_1  //%%%%%%%%%%%%%%% WARNING: USE Sister Board V1.1 or later %%%%%%%%%%%%%%%%%%%%%%%%%%%%  
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V1.1 ==================
        // *************************** NOVA XR BOARD V1.1  *******************************
        //:::::::::::::: for Main board ::::::::::::::::::::::::::
        uint8_t HW_Board_Rev      = 3;
        const uint8_t MainCS      = PA21; ///<  Chip Select pin  for main board
        const uint8_t MainDRDY    = PA10; ///<  Data Ready pin for main board  - Hardware Interrupt Ready
        const uint8_t ResetMB_pin = PA23; ///<  ADC Reset Pin for MainBoard
            
        //:::::::::::::: for Sister board ::::::::::::::::::::::::
        const uint8_t SisCS          = PA20; ///<  Chip Select pin  for sister board
        const uint8_t SisDRDY        = PA07; ///<  Data Ready pin for sister board 
        const uint8_t ResetSB_pin    = PA23; ///<  ADC Reset Pin for SisterBoard
        const uint8_t VDD_ref_pin    = PB09;
        const uint8_t GND_ref_pin    = PA06;
        const uint8_t BATT_Sense_pin = PA05; 
        const uint8_t EDA_Sense_pin  = PA02;
    
    #elif BOARD_V2  
        // ========== CONFIG HERE BEFORE YOU LOAD THIS FIRMWARE TO NOVA XR BOARD Rev. V2 =================
        // ****************************** NOVA XR V2  *************************************
        //:::::::::::::: for Main board V2 ::::::::::::::::::::::::::
        uint8_t HW_Board_Rev      = 4;
        const uint8_t MainCS      = PA21; ///<  Chip Select pin  for main board
        const uint8_t MainDRDY    = PB10; ///<  Data Ready pin for main board  - Hardware Interrupt Ready   
        const uint8_t ResetMB_pin = PA23; ///<  ADC Reset Pin for MainBoard
        const uint8_t OTG_pin     = PA18; ///<  USB OTG Pin for MainBoard    
        
        //:::::::::::::: for Sister board V2 ::::::::::::::::::::::::
        const uint8_t SisCS          = PA20; ///<  Chip Select pin  for sister board
        const uint8_t SisDRDY        = PA07; ///<  Data Ready pin for sister board 
        const uint8_t ResetSB_pin    = PA22; ///<  ADC Reset Pin for SisterBoard   
        const uint8_t VDD_ref_pin    = PA06; ///<  VDD for EDA Circuit 
        const uint8_t GND_ref_pin    = PB09; ///<  GND for EDA Circuit 
        const uint8_t BATT_Sense_pin = PA05; ///<  Battery Level Sense Signal
        const uint8_t EDA_Sense_pin  = PA02; ///<s  EDA Sensor Sense Signal
    
    #endif 
// =====================================================================================================






/**
 * Initialization of the selected NovaXR PCB Revision 
 *                
 * @return True if wrong/unknown NovaXR PCB version is selected
 * 
 */
bool set_board_revision(void){

    bool error_occured = false;
    Serial.println(HW_Board_Rev);
    switch(HW_Board_Rev){  
      
        case 0:
          Serial.println("\n..... DEV BOARD MKR1010 CONFIGS loaded ... ");
        	Serial.println("Dev Board: Not implemented");
          error_occured =true;
        break; 
      
        case 1:
        	Serial.println("NovaXR PCB V1: Not implemented");
          error_occured =true;            
        break; 
    
        case 2:
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
      
            // ---- Choose: type of WiFi Connectivity of the Device -------------      
            WiFi_connectivityOption(WIFI_MODE);
        break;

        
        case 3:
            Serial.println("NovaXR Mainboard PCB: V1.1 + Sisterboard PCB V1.1");
            Serial.println("Not existing compatible configuration");
            error_occured =true; 
        break; 
        
        case 4:

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
      
            // ---- Choose: type of WiFi Connectivity of the Device -------------      
            WiFi_connectivityOption(WIFI_MODE);
        break;

         
       default:
           Serial.println("Wrong Board Selection");
           error_occured =true; 
        break; 

    }


  return error_occured;

}
