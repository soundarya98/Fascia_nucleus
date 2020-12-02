/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V1 board
   Firmware:            Version 1.07
   Device Mode:         Infrastructure Mode
*/

#include "OpenBCI_NovaXR_Definitions.h"
#include "NXR_Utility_Lib.h"
#include "OpenBCI_NovaXR.h"

//#define APMODE
#define DEBUG


Sensors_Data  SensorD;
DataPacket    D_Packet;
boolean       streamingFlag = STOP_STREAMING;



void setup() {

   #ifdef DEBUG 
    // ----- Debuging ------
    Serial.begin(9600);
    while(!Serial);
    delay(1000); 
    Serial.print("Datapacket size: ");
    Serial.println(sizeof(DataPacket));
   #endif   #endif

     
    #ifdef APMODE 
      // ---- Device will create a WiFi network
     char ssidAP[]= APSSID;
     char passAP[]= APSPASS;
     CreateAccessPoint(ssidAP, passAP);
      // ---- Device is in Access Point Mode
      #else
      // ---- Device will connect to a WiFi Access Point
   
   if(ask_for_WiFi_Cred()){
      Connect_to_AP(wfSSID,wfPASS);
    }
      // ---- Device is in Infrastructure Mode
    #endif
  
    // ---- Start a UDP Server-----
    startUDP_Server(NETPORT);
  
}



void loop() {

      Receive_Command();
      uint8_t id=0;
      uint8_t waveRange=255;



   while(streamingFlag){
        
         //start_DStream(&SensorD,&D_Packet,1);
         Data_Steam(&SensorD, &D_Packet, id, id, waveRange);
         id++;
              Serial.print(D_Packet.PacketNum); // Packet number
              Serial.print(",");
              long a;
              a = SensorD.ADS.EMG[0][0];
              a=a<<8|SensorD.ADS.EMG[0][1];
              a=a<<8|SensorD.ADS.EMG[0][2];
              Serial.print(a);                // EMG channel 0 
              Serial.print(",");

              long b;
              b = SensorD.ADS.EEG_OL[0][0];
              b=b<<8|SensorD.ADS.EEG_OL[0][1];
              b=b<<8|SensorD.ADS.EEG_OL[0][2];
              Serial.print(b);              // EEG_OL channel 0 
              Serial.print(",");

              uint16_t c,d;
              c = SensorD.AUX.EDA;          // EDA data
              Serial.print(c);
              Serial.print(",");
              
              d = SensorD.AUX.PPG;          // PPG data
              Serial.print(d);
              Serial.print(",");
              
              Serial.println(D_Packet.timeStamp);  // Timestamp data

              

        if(id>waveRange){
          id=0; //Reset
          break;
        }
         
        bool assertion = Receive_Command(); 
        if(assertion){
          Serial.println("End of Streaming");
          break;  
        }
        
    }

 }

 
