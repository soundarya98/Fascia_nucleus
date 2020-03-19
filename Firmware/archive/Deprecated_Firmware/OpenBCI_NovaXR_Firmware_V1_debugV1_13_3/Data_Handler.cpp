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


// ------- Declare Useful Structure Variables ----------
adc_data         ADC_data;    
Sensors_Data     SenseD;
DataPacket       outBuffer[UDP_TCP_PKT_SIZE];
switchOutBuffer  swOutBuffer[BUFFPOOL_SIZE];




void getADS_RAWdata(ADS_Data * data){

    select_ADC(MAIN_BOARD);     // -- Get Main Board ADS fresh data 
    get_ADC_Data_SS(&ADC_data); // -- single readback
    
    //--- EEG_FC 
    for(int id=0; id<3; id++)data->EEG_FC[0][id] = ADC_data.ADS_paylaod.DataCH0[id];
    for(int id=0; id<3; id++)data->EEG_FC[1][id] = ADC_data.ADS_paylaod.DataCH1[id]; 

    //--- EEG_OL
    for(int id=0; id<3; id++)data->EEG_OL[2][id] = ADC_data.ADS_paylaod.DataCH2[id];        
    for(int id=0; id<3; id++)data->EEG_OL[3][id] = ADC_data.ADS_paylaod.DataCH3[id]; 
    for(int id=0; id<3; id++)data->EEG_OL[4][id] = ADC_data.ADS_paylaod.DataCH4[id];        
    for(int id=0; id<3; id++)data->EEG_OL[5][id] = ADC_data.ADS_paylaod.DataCH5[id]; 
    for(int id=0; id<3; id++)data->EEG_OL[6][id] = ADC_data.ADS_paylaod.DataCH6[id];        
    for(int id=0; id<3; id++)data->EEG_OL[7][id] = ADC_data.ADS_paylaod.DataCH7[id];      

    select_ADC(SISTER_BOARD);   // -- Get Sister Board fresh data 
    get_ADC_Data_SS(&ADC_data); // -- single readback 

    //--- EEG_OL
    for(int id=0; id<3; id++)data->EEG_OL[0][id] = ADC_data.ADS_paylaod.DataCH0[id];        
    for(int id=0; id<3; id++)data->EEG_OL[1][id] = ADC_data.ADS_paylaod.DataCH1[id]; 
    
    //--- EOG
    for(int id=0; id<3; id++)data->EOG[0][id] = ADC_data.ADS_paylaod.DataCH2[id];
    for(int id=0; id<3; id++)data->EOG[1][id] = ADC_data.ADS_paylaod.DataCH3[id];         

    //--- EMG
    for(int id=0; id<3; id++)data->EMG[0][id] = ADC_data.ADS_paylaod.DataCH4[id];
    for(int id=0; id<3; id++)data->EMG[1][id] = ADC_data.ADS_paylaod.DataCH5[id];
    for(int id=0; id<3; id++)data->EMG[2][id] = ADC_data.ADS_paylaod.DataCH6[id];
    for(int id=0; id<3; id++)data->EMG[3][id] = ADC_data.ADS_paylaod.DataCH7[id];   
 
}


void getIMU_RAWdata(IMU_Data * data){
   #define CELCIOUS 1
   #define FAHR     0

   uint8_t battery_level =(uint8_t)getBattery_ChargeLevel(PA05, BATTVOLT);  
   data->AXL_X=battery_level; // <-- after this value 
   //Serial.print("--Battery: ");Serial.print(data->AXL_X);Serial.println("%");
  
   uint16_t SKIN_Temp = (uint16_t)get_Skin_Temp(CELCIOUS);
   data->AXL_Y=SKIN_Temp;
   //Serial.print("--Stream Skin temp: ");Serial.println(data->AXL_Y);
   data->AXL_Z=SKIN_Temp;
   data->GYR_X=251;
   data->GYR_Y=252;
   data->GYR_Z=258;
}


//long startTimer =0;
void getAux_RAWdata(AuxSensor_Data * data){
    uint8_t avg_HR =0;
    uint16_t Val3 = get_AnalogSense_Level(PA02);
    data->EDA = Val3; // Read EDA data   
    //Serial.print("--Stream EDA: ");Serial.println(data->EDA);
   
   /* 
    startTimer = micros();
    while((micros() - startTimer)< 100000){
       avg_HR = get_Heart_Rate();
    }
    */
    //avg_HR = get_Heart_Rate();

    data->PPG = 78;   // Rerad AVG Heart Rate
    // Serial.print("--Stream AVG HR: ");Serial.println(data->PPG);
    
}


void printEDA(void){
    float EDA_level = analogRead(PA02); // Read EDA data 
    Serial.print("EDA: "); Serial.print(EDA_level);Serial.println("Raw Voltage");
}


void getBatteryLevel(Sensors_Data * data){
    
     int raw_val = analogRead(PA05); // Read EDA data 
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
     float Batt_level = raw_val * (4.2 / 1023.0);
    Serial.print("Battery Level: "); Serial.println(Batt_level);Serial.println();
}


void printBatt_level(void){
     getBatteryLevel(&SenseD);
}


double getTimeStamp(void){
    return 15644257.345322;
}


double putTimeStamp(void){
    double timest = (double)millis()/6;
    return timest;
}


void getUpdatedSensoryData(Sensors_Data * data){
     getADS_RAWdata(&data->ADS);
     getIMU_RAWdata(&data->IMU); 
     getAux_RAWdata(&data->AUX); 
}



void fill_outBuffer(Sensors_Data *payload, uint8_t pkgID, uint8_t buff_id){
    outBuffer[buff_id].PacketNum = pkgID;
    outBuffer[buff_id].Payload.AUX = payload->AUX;
    outBuffer[buff_id].Payload.ADS = payload->ADS;
    outBuffer[buff_id].Payload.IMU = payload->IMU;
    outBuffer[buff_id].timeStamp = putTimeStamp();
}


void fill_SwitchBuffer(Sensors_Data *payload, uint8_t pkgID, uint8_t entry_id, uint8_t swBuff_id){
    swOutBuffer[swBuff_id].oBuff[entry_id].PacketNum   = pkgID;
    swOutBuffer[swBuff_id].oBuff[entry_id].Payload.AUX = payload->AUX;
    swOutBuffer[swBuff_id].oBuff[entry_id].Payload.ADS = payload->ADS;
    swOutBuffer[swBuff_id].oBuff[entry_id].Payload.IMU = payload->IMU;
    swOutBuffer[swBuff_id].oBuff[entry_id].timeStamp   = putTimeStamp(); 
}



// ------------------- Assemble Output Buffer ----------------------------------
void assemble_outBuffer(uint8_t dataPkt_ID){
    for(uint8_t Buff_id=0; Buff_id<UDP_TCP_PKT_SIZE; Buff_id++){
       getUpdatedSensoryData(&SenseD);  
      fill_outBuffer(&SenseD, dataPkt_ID++, Buff_id); 
    }
}


// ------------------- Assemble Switch Output Buffer --------------------------
void assemble_swBuffer(uint8_t dataPkt_ID , uint8_t swBuff_id){
    for(uint8_t entry_id=0; entry_id<UDP_TCP_PKT_SIZE; entry_id++){
       getUpdatedSensoryData(&SenseD); 
       fill_SwitchBuffer(&SenseD, dataPkt_ID++ , entry_id, swBuff_id); 
    }
}



// ------------------- Initialize Switch Buffers ------------------------------
void reset_swBuffers(void){
    for(uint8_t id=0; id<BUFFPOOL_SIZE; id++){
       swOutBuffer[id].fullBuff=false;
       swOutBuffer[id].emptyBuff=true;
    }
}


// -------------------- get the ID of one available empty buffer --------------
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



// ------------------ get the ID of one available full buffer -------------------
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



// ------------------ Mark a Switch Buffer as Full -------------------------------
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



// ------------------ Mark a Switch Buffer as Empty ------------------------------
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
