/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - V1 board
   Firmware:            Version 1.08
   Device Mode:         Infrastructure Mode
*/


// TO DOs:
// fix the last update time and date
// fix SSIDs with spaces 



#include "OpenBCI_NovaXR_Definitions.h"
#include "NXR_Utility_Lib.h"
#include "NXR_Flash.h"
#include "OpenBCI_NovaXR.h"
#include "HTML_Server.h"

//#define APMODE
#define DEBUG
#define FWUPDATE 108



Sensors_Data  SensorD;
DataPacket    D_Packet;
boolean       streamingFlag = STOP_STREAMING;




 
void setup() {

   #ifdef DEBUG 
    // ----- Debuging ------
    Serial.begin(9600);
    while(!Serial);
    delay(1000); 
    //Serial.print("Datapacket size: ");
    //Serial.println(sizeof(DataPacket));
   #endif 
   
      // ---- Device will create a WiFi network
      // ---- Device is in Access Point Mode
      // ---- Device will connect to a WiFi Access Point
      // ---- Device is in Infrastructure Mode

   bool connStatus = false;
   ROM_Init();


   int totalWiFi = count_SSIDsInList();
   Serial.print("Total WiFi:  ");Serial.println(totalWiFi); 

   if(totalWiFi>0){  // ------ Connect to a saved WiFi SSID --------------
        Serial.println("Connecting ....");
        // get the last connected WiFi credential
        uint8_t lastWiFi = lastWiFi_ID();
        for(uint8_t index=0; index<totalWiFi; index++){  
              Serial.print("connect_lastWiFi: # ");Serial.println(lastWiFi);             
              connStatus = connect_lastWiFi(lastWiFi);
              connStatus =0;
              if (connStatus){
                break;
              }else{
                lastWiFi = previous_WiFi_ID(); 
                 
              }
         }
         Serial.println("Creating AP #0....");
         if(!connStatus){ // if none of the stored SSIDs are near by then get in AP mode to register a new one
          start_HTMLserver(CreateSSID(), CreatePASS());
         }
             
    }else {  // ------ Create an WiFi network, get in AP mode --------------
      Serial.println("Creating AP #1....");
      start_HTMLserver(CreateSSID(), CreatePASS());
       
   }

     
    // ---- Start a UDP Server-----
    Serial.println("Starting Data Streamer....");
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


 
