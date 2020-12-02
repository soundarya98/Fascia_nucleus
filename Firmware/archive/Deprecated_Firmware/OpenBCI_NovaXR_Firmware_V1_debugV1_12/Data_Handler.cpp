/*
 * Author:       ioannis Smanis
 * Library for:  Managing Data for Packets and updating sensors raw data
 * Project:      NovaXR - MKR1010
 * Date:         10/13/2019
 * 
 */

#include "Data_Handler.h"
#include "ADS1299_lib.h"


// ------- Declare Useful Structure Variables ----------
adc_data      ADC_data;
Sensors_Data  SensorD;
DataPacket    D_Packet;
DataPacket    outBuffer[TCP_PKT_SIZE];



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
   data->AXL_X=2251; // <-- after this value 
   data->AXL_Y=3251;
   data->AXL_Z=271;
   data->GYR_X=251;
   data->GYR_Y=252;
   data->GYR_Z=258;
}


void getAux_RAWdata(AuxSensor_Data * data){
    data->EDA = 152; //analogRead(PA02);
    data->PPG = 69;
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


void assemble_DataBuffer(Sensors_Data *payload, DataPacket *d_packet,  uint8_t pkgID){
    d_packet->PacketNum = pkgID;
    d_packet->Payload.AUX = payload->AUX;
    d_packet->Payload.ADS = payload->ADS;
    d_packet->Payload.IMU = payload->IMU;
    d_packet->timeStamp = putTimeStamp(); //getTimeStamp();
}



void fill_outBuffer(Sensors_Data *payload, uint8_t pkgID, uint8_t buff_id){
    outBuffer[buff_id].PacketNum = pkgID;
    outBuffer[buff_id].Payload.AUX = payload->AUX;
    outBuffer[buff_id].Payload.ADS = payload->ADS;
    outBuffer[buff_id].Payload.IMU = payload->IMU;
    outBuffer[buff_id].timeStamp = putTimeStamp(); //getTimeStamp();
}


 
void assemble_outBuffer( uint8_t dataPkt_ID){

   for(uint8_t Buff_id=0; Buff_id<TCP_PKT_SIZE; Buff_id++){
       getUpdatedSensoryData(&SensorD);  
       fill_outBuffer(&SensorD, dataPkt_ID++, Buff_id); 
    }

    /*
  for(int i=0; i<UDP_PKT_SIZE; i++) { 
    for(uint8_t j=0; j<3; j++){
      Serial.print(outBuffer[i].Payload.ADS.EEG_FC[0][j], HEX); Serial.print(" ");
    }
     for(uint8_t j=0; j<3; j++){
      Serial.print(outBuffer[i].Payload.ADS.EEG_FC[1][j], HEX); Serial.print(" ");
    }

     for(uint8_t j=0; j<3; j++){
      Serial.print(outBuffer[i].Payload.ADS.EEG_OL[0][j], HEX); Serial.print(" ");
    }
    Serial.println();
  }
*/
  
}
