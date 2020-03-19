/*
   Author:              Ioannis Smanis
   Compatible Hardware: NovaXR - MKR1010 board
   Firmware:            Version 1.6
   Device Mode:         Infrastructure Mode
*/


#include "OpenBCI_NovaXR_Definitions.h"
#include "NXR_Utility_Lib.h"
#include "OpenBCI_NovaXR.h"


//#define APMODE
#define DEBUG


Sensors_Data  SensorD;
DataPacket D_Packet;

boolean streamingFlag = STOP_STREAMING;



void setup() {

   #ifdef DEBUG 
    // ----- Debuging ------
    Serial.begin(9600);
    while(!Serial);
    delay(1000); 
   #endif
   Serial.print("Datapacket size: ");
   Serial.println(sizeof(DataPacket));
  
    #ifdef APMODE 
      // ---- Device will create a WiFi network
     char ssidAP[]= APSSID;
     char passAP[]= APSPASS;
     CreateAccessPoint(ssidAP, passAP);
      // ---- Device is in Access Point Mode
      #else
      // ---- Device will connect to a WiFi Access Point
        Connect_to_AP();
      // ---- Device is in Infrastructure Mode
    #endif
  
    // ---- Start a UDP Server-----
    startUDP_Server(NETPORT);
  
}



void loop() {

      Receive_Command();
       while(streamingFlag){
         start_DStream(&SensorD,&D_Packet);
         bool assertion = Receive_Command();
          if(assertion){
            Serial.println("Error sending data..");
            break;  
          }
       }

 }

 
