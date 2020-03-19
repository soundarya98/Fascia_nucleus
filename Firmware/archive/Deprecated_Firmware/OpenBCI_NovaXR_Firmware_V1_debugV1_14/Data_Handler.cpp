/*
 * Author:       ioannis Smanis
 * Library for:  Managing Data for Packets and updating sensors raw data
 * Project:      NovaXR 
 * Date:         10/13/2019
 * 
 */

#include "Data_Handler.h"
#include "ADS1299_lib.h"
#include "NXR_AUX_Sensors.h"
#include "NovaXR_PPG_Sensor.h"


//#define debug_mode


// ------- Declare Useful Structure Variables ----------
adc_data         ADC_data;    
Sensors_Data     SenseD;
DataPacket       outBuffer[UDP_TCP_PKT_SIZE];
switchOutBuffer  swOutBuffer[BUFFPOOL_SIZE];



/**
 * @brief      Stores the ADS1299 raw data in the instace of the ADC_data structure.
 *
 * @param      data  The target instace of the ADC_data structure
 */
void getADS_RAWdata(ADS_Data * data){

    select_ADC(MAIN_BOARD);     ///< -- Get Main Board ADS fresh data 
    get_ADC_Data_SS(&ADC_data); ///< -- single readback

   #ifndef debug_mode 
      //--- EEG_FC 
      for(int id=0; id<3; id++)data->EEG_FC[0][id] = ADC_data.ADS_paylaod.DataCH0[2-id];   
      for(int id=0; id<3; id++)data->EEG_FC[1][id] = ADC_data.ADS_paylaod.DataCH1[2-id]; 
     
      //--- EEG_OL
      for(int id=0; id<3; id++)data->EEG_OL[0][id] = ADC_data.ADS_paylaod.DataCH2[2-id];         
      for(int id=0; id<3; id++)data->EEG_OL[1][id] = ADC_data.ADS_paylaod.DataCH3[2-id];     
      for(int id=0; id<3; id++)data->EEG_OL[2][id] = ADC_data.ADS_paylaod.DataCH4[2-id]; 
      for(int id=0; id<3; id++)data->EEG_OL[3][id] = ADC_data.ADS_paylaod.DataCH5[2-id];      
      for(int id=0; id<3; id++)data->EEG_OL[4][id] = ADC_data.ADS_paylaod.DataCH6[2-id];    
      for(int id=0; id<3; id++)data->EEG_OL[5][id] = ADC_data.ADS_paylaod.DataCH7[2-id];  

   #else
       data->EEG_FC[0][0] = 0x14;  data->EEG_FC[0][1] = 0x00; data->EEG_FC[0][2] = 0x00;
       data->EEG_FC[1][0] = 0x15;  data->EEG_FC[1][1] = 0x00; data->EEG_FC[1][2] = 0x00;   
       data->EEG_OL[0][0] = 0x16;  data->EEG_OL[0][1] = 0x00; data->EEG_OL[0][2] = 0x00;
       data->EEG_OL[1][0] = 0x17;  data->EEG_OL[1][1] = 0x00; data->EEG_OL[1][2] = 0x00;         
       data->EEG_OL[2][0] = 0x18;  data->EEG_OL[2][1] = 0x00; data->EEG_OL[2][2] = 0x00;
       data->EEG_OL[3][0] = 0x19;  data->EEG_OL[3][1] = 0x00; data->EEG_OL[3][2] = 0x00; 
       data->EEG_OL[4][0] = 0x1a;  data->EEG_OL[4][1] = 0x00; data->EEG_OL[4][2] = 0x00;
       data->EEG_OL[5][0] = 0x1b;  data->EEG_OL[5][1] = 0x00; data->EEG_OL[5][2] = 0x00;
   #endif

   
    select_ADC(SISTER_BOARD);   ///< -- Get Sister Board fresh data 
    get_ADC_Data_SS(&ADC_data); ///< -- single readback 
   
   #ifndef debug_mode   
      //--- EEG_OL  
      for(int id=0; id<3; id++)data->EEG_OL[6][id] = ADC_data.ADS_paylaod.DataCH0[2-id];            
      for(int id=0; id<3; id++)data->EEG_OL[7][id] = ADC_data.ADS_paylaod.DataCH1[2-id]; 
      
      //--- EOG
      for(int id=0; id<3; id++)data->EOG[0][id] = ADC_data.ADS_paylaod.DataCH2[2-id];      
      for(int id=0; id<3; id++)data->EOG[1][id] = ADC_data.ADS_paylaod.DataCH3[2-id];
  
      //--- EMG  
      for(int id=0; id<3; id++)data->EMG[0][id] = ADC_data.ADS_paylaod.DataCH4[2-id];
      for(int id=0; id<3; id++)data->EMG[1][id] = ADC_data.ADS_paylaod.DataCH5[2-id];
      for(int id=0; id<3; id++)data->EMG[2][id] = ADC_data.ADS_paylaod.DataCH6[2-id];
      for(int id=0; id<3; id++)data->EMG[3][id] = ADC_data.ADS_paylaod.DataCH7[2-id];   
   
    #else 
       data->EEG_OL[6][0]= 0x0a;  data->EEG_OL[6][1] = 0x00; data->EEG_OL[6][2] = 0x00;  
       data->EEG_OL[7][0]= 0x0b;  data->EEG_OL[7][1] = 0x00; data->EEG_OL[7][2] = 0x00;
       data->EOG[0][0]   = 0x0c;  data->EOG[0][1]    = 0x00; data->EOG[0][2]    = 0x00;           
       data->EOG[1][0]   = 0x0d;  data->EOG[1][1]    = 0x00; data->EOG[1][2]    = 0x00;
       data->EMG[0][0]   = 0x0e;  data->EMG[0][1]    = 0x00; data->EMG[0][2]    = 0x00; 
       data->EMG[1][0]   = 0x0f;  data->EMG[1][1]    = 0x00; data->EMG[1][2]    = 0x00;
       data->EMG[2][0]   = 0x10;  data->EMG[2][1]    = 0x00; data->EMG[2][2]    = 0x00;
       data->EMG[3][0]   = 0x11;  data->EMG[3][1]    = 0x00; data->EMG[3][2]    = 0x00;
         
    #endif  

}





/**
 * @brief      Gets Auxilary Raw Data data.
 *
 * @param      data  The data
 */
void getIMU_RAWdata(IMU_Data * data){
   #define CELCIOUS 1
   #define FAHR     0
   //  long tA=micros();
   //  uint8_t battery_level =(uint8_t)getBattery_ChargeLevel(PA05, BATTVOLT);  
   //  long tB=micros();
   //  uint16_t SKIN_Temp = (uint16_t)get_Skin_Temp(CELCIOUS);  
   //  long tC=micros();
   //  Serial.print("Batt level time: ");Serial.println(tB-tA);
   //  Serial.print("SKIN Temp time: ");Serial.println(tC-tB);
   data->BATT=83; 
   data->SKIN_T=(36.63*100); //SKIN_Temp;
   data->FW_CODE=011;//SKIN_Temp;
   data->AXL  =2533;
   data->GYR_X=251;
   data->GYR_Y=252;
   data->GYR_Z=24;
   
}



void getAux_RAWdata(AuxSensor_Data * data){
   //uint8_t avg_HR =0;
   // long t1=micros();
   // uint16_t reflexion =  get_RAW_IR_Reflection();
   // long t2=micros();
   // Serial.print("IR Reflexion time: ");Serial.println(t2-t1);
   // uint16_t EDA_sig = get_AnalogSense_Level_Fast(PA02);
   // uint16_t EDA_sig = get_AnalogSense_Level(PA02);
  data->EDA = 5334; // Read EDA data   
  data->PPG = 78;   // Rerad AVG Heart Rate
  
}








/**
 * @brief      Prints a Battery Charge Level.
 */
void printBatt_level(void){
  uint8_t battery_level =(uint8_t)getBattery_ChargeLevel(PA05, BATTVOLT);
  Serial.print("Battery Level: "); Serial.println(battery_level);Serial.println("%");
}



/**
 * @brief      Create a local timestamp.
 *
 * @return     timest returns timestamp in microseconds
 */
double putTimeStamp(void){
    double timest = (double)millis()/6;
    return timest;
}



/**
 * @brief      Updates the local Sensors_Data structure with updated sensory data.
 *
 * @param      data  Instance of the local Sensors_Data structure
 */
void getUpdatedSensoryData(Sensors_Data * data){
     getADS_RAWdata(&data->ADS);
     getIMU_RAWdata(&data->IMU); 
     getAux_RAWdata(&data->AUX);
}


/**
 * @brief      Fills a data pacekt of a single output buffer with sensory data
 *
 * @param      payload  The sensory data - payload
 * @param[in]  pkgID    The data package id
 * @param[in]  buff_id  The buffer identifier
 */
void fill_outBuffer(Sensors_Data *payload, uint8_t pkgID, uint8_t buff_id){
    outBuffer[buff_id].PacketNum   = pkgID;
    outBuffer[buff_id].Payload.AUX = payload->AUX;
    outBuffer[buff_id].Payload.ADS = payload->ADS;
    outBuffer[buff_id].Payload.IMU = payload->IMU;
    outBuffer[buff_id].timeStamp   = putTimeStamp();
}



/**
 * @brief      Fills a data pacekt of a switch buffer with sensory data
 *
 * @param      payload    The sensory data - payload
 * @param[in]  pkgID      The data package id
 * @param[in]  entry_id   The entry identifier
 * @param[in]  swBuff_id  The switch buffer identifier
 */
void fill_SwitchBuffer(Sensors_Data *payload, uint8_t pkgID, uint8_t entry_id, uint8_t swBuff_id){
    swOutBuffer[swBuff_id].oBuff[entry_id].PacketNum   = pkgID;
    swOutBuffer[swBuff_id].oBuff[entry_id].Payload.AUX = payload->AUX;
    swOutBuffer[swBuff_id].oBuff[entry_id].Payload.ADS = payload->ADS;
    swOutBuffer[swBuff_id].oBuff[entry_id].Payload.IMU = payload->IMU;
    swOutBuffer[swBuff_id].oBuff[entry_id].timeStamp   = putTimeStamp(); 
}



/**
 * @brief      Assemble Output Buffer
 *
 * @param[in]  dataPkt_ID  The data-packet id
 */
void assemble_outBuffer(uint8_t dataPkt_ID){
    for(uint8_t Buff_id=0; Buff_id<UDP_TCP_PKT_SIZE; Buff_id++){
       getUpdatedSensoryData(&SenseD);  
      fill_outBuffer(&SenseD, dataPkt_ID++, Buff_id); 
    }
}



/**
 * @brief      Assemble a full Switch Output Buffer
 *
 * @param[in]  dataPkt_ID  The data packet id
 * @param[in]  swBuff_id   The software buffer identifier
 */
void assemble_swBuffer(uint8_t dataPkt_ID , uint8_t swBuff_id){
    for(uint8_t entry_id=0; entry_id<UDP_TCP_PKT_SIZE; entry_id++){
       getUpdatedSensoryData(&SenseD); 
       fill_SwitchBuffer(&SenseD, dataPkt_ID++ , entry_id, swBuff_id); 
    }
}



/**
 * @brief      Reset both switch buffers state
 */
void reset_swBuffers(void){
    for(uint8_t id=0; id<BUFFPOOL_SIZE; id++){
       swOutBuffer[id].fullBuff=false;
       swOutBuffer[id].emptyBuff=true;
    }
}



/**
 * @brief      Gets the ID of one available empty buffer.
 *
 * @return     The empty switch buffer id.
 */
uint8_t getEmpty_swBuffer_ID(void){
   int8_t swBUFF_num=-1; // error 
   for(uint8_t id=0; id<BUFFPOOL_SIZE; id++){
      if(swOutBuffer[id].emptyBuff){
        swBUFF_num = id;
        break;
      }  
   }
   if(swBUFF_num==-1)Serial.println("ERROR: NONE EMPTY Switch Buffer !!");
   return swBUFF_num;
}



/**
 * @brief      Gets the ID of one available full switch buffer.
 *
 * @return     The full switch buffer id.
 */
uint8_t getFull_swBuffer_ID(void){
   int8_t swBUFF_num=-1; // error 
   for(uint8_t id=0; id<BUFFPOOL_SIZE; id++){
      if(swOutBuffer[id].fullBuff){
        swBUFF_num = id;
        break;
      }  
   }
   if(swBUFF_num==-1)Serial.println("ERROR: NONE FULL Switch Buffer !!");
   return swBUFF_num;
}



/**
 * @brief      Mark a Switch Buffer as Full
 *
 * @param[in]  swBuff_ID  The switch buffer id
 *
 * @return     confirm  it returns True if buffer was empty
 */
bool markFull_swBuffer(uint8_t swBuff_ID){
   bool confirm=false;  
   if (swOutBuffer[swBuff_ID].fullBuff){
      Serial.println("ERROR: Switch Buffer is already Full");
   }else{
    swOutBuffer[swBuff_ID].fullBuff=true;
    swOutBuffer[swBuff_ID].emptyBuff=false;
    confirm = true;
   }  
  return confirm;
}



/**
 * @brief      Mark a Switch Buffer as Empty.
 *
 * @param[in]  swBuff_ID  The switch buffer id
 *
 * @return     confirm  it returns True if buffer was full
 */
bool markEmpty_swBuffer(uint8_t swBuff_ID){
   bool confirm=false;  
   if (swOutBuffer[swBuff_ID].emptyBuff){
      Serial.println("ERROR: Switch Buffer is already Empty");
   }else{
    swOutBuffer[swBuff_ID].fullBuff=false;
    swOutBuffer[swBuff_ID].emptyBuff=true;
    confirm = true;
   }  
  return confirm;
}
