/*
   Author:        Ioannis Smanis
   Hardware:      NovaXR - MKR1010 board
   Firmware:      Version 1.0
   Firmware Component: NXR Utility Library source file
*/

#include "Arduino.h"
#include "NXR_Flash.h"
#include "OpenBCI_NovaXR.h"
#include "NXR_Utility_Lib.h"
#include "HTML_Server.h"

WiFiUDP UDP_Server;

#define ADS_DEV0 0
#define ADS_DEV1 1


// ------------ Set fake values ----------
uint8_t packetNUM = 21;

void putEEG_FC(ADS_Data * data, long value){

  byte byte0 = value>>16;
  byte byte1 = value>>8;
  byte byte2 = value;
  
  data->EEG_FC[0][0]= byte0;
  data->EEG_FC[0][1]= byte1;
  data->EEG_FC[0][2]= byte2;
  
  data->EEG_FC[1][0]= byte0;
  data->EEG_FC[1][1]= byte1;
  data->EEG_FC[1][2]= byte2;

}


void putEEG_OL(ADS_Data * data, long value){

    byte byte0 = value>>16;
    byte byte1 = value>>8;
    byte byte2 = value;

    // --- EEG_OL ---------
    data->EEG_OL[0][0]= byte0; 
    data->EEG_OL[0][1]= byte1; 
    data->EEG_OL[0][2]= byte2; 
                   
    data->EEG_OL[1][0]= byte0; 
    data->EEG_OL[1][1]= byte1;
    data->EEG_OL[1][2]= byte2;
    
    data->EEG_OL[2][0]= byte0;
    data->EEG_OL[2][1]= byte1;
    data->EEG_OL[2][2]= byte2;
           
    data->EEG_OL[3][0]= byte0;
    data->EEG_OL[3][1]= byte1;
    data->EEG_OL[3][2]= byte2;
            
    data->EEG_OL[4][0]= byte0;
    data->EEG_OL[4][1]= byte1;
    data->EEG_OL[4][2]= byte2;
                    
    data->EEG_OL[5][0]= byte0; 
    data->EEG_OL[5][1]= byte1;
    data->EEG_OL[5][2]= byte2;
          
    data->EEG_OL[6][0]= byte0;
    data->EEG_OL[6][1]= byte1; 
    data->EEG_OL[6][2]= byte2;
                   
    data->EEG_OL[7][0]= byte0; 
    data->EEG_OL[7][1]= byte1;
    data->EEG_OL[7][2]= byte2;

}




void putEOG(ADS_Data * data, long value){

      byte byte0 = value>>16;
      byte byte1 = value>>8;
      byte byte2 = value;
  
      // --- EOG ---------
      data->EOG[0][0]= byte0; 
      data->EOG[0][1]= byte1; 
      data->EOG[0][2]= byte2; 
                     
      data->EOG[1][0]= byte0; 
      data->EOG[1][1]= byte1;
      data->EOG[1][2]= byte2;
   
}



void putEMG(ADS_Data * data, long value){
          
      uint8_t byte0 = value>>16;
      uint8_t byte1 = value>>8;
      uint8_t byte2 = value;
      
  
      // --- EMG ---------
      data->EMG[0][0]= byte0; 
      data->EMG[0][1]= byte1; 
      data->EMG[0][2]= byte2; 
                     
      data->EMG[1][0]= byte0; 
      data->EMG[1][1]= byte1;
      data->EMG[1][2]= byte2;

      data->EMG[2][0]= byte0; 
      data->EMG[2][1]= byte1;
      data->EMG[2][2]= byte2;

      data->EMG[3][0]= byte0; 
      data->EMG[3][1]= byte1;
      data->EMG[3][2]= byte2;  
}



void putIMU_data(IMU_Data * data, uint8_t scalef){

   data->AXL_X=21*scalef; // <-- after this value 
   data->AXL_Y=32*scalef;
   data->AXL_Z=1*scalef;
   
   data->GYR_X=2*scalef;
   data->GYR_Y=5*scalef;
   data->GYR_Z=8*scalef;
}

void putAux_data(AuxSensor_Data * data, uint8_t factor ){
    data->EDA=12*factor;
    data->PPG=factor;
}


double putTimeStamp(void){
  double timest = (double)millis()/6;
  return timest;
}



void make_SignalSQwave(Sensors_Data * sense_data, uint8_t idx, uint8_t period){
    
    // ---- factors initialization -------
    long EEG_FC_p_value = 64;
    long EEG_FC_n_value = 6111;
    
    long EEG_OL_p_value = 9004;
    long EEG_OL_n_value = 23;
    
    long EEG_EOG_p_value = 23454;
    long EEG_EOG_n_value = 454;

    long EEG_EMG_p_value = 11134;
    long EEG_EMG_n_value = 23452;

    uint8_t AUXfactor_p = 2;
    uint8_t AUXfactor_n = 150;

    uint8_t IMUfactor_p = 20;
    uint8_t IMUfactor_n = 89;

     
     if(idx<(period/2)){ // ======= Positive Pulse =============
            putEEG_FC(&sense_data->ADS, EEG_FC_p_value);
            putEEG_OL(&sense_data->ADS, EEG_OL_p_value);
               putEOG(&sense_data->ADS, EEG_EOG_p_value);
               putEMG(&sense_data->ADS, EEG_EMG_p_value);
          putIMU_data(&sense_data->IMU, IMUfactor_p);
          putAux_data(&sense_data->AUX, AUXfactor_p);
        
     }else{  // ================ Negative Pulse ==============   
        putEEG_FC(&sense_data->ADS, EEG_FC_n_value);
                putEEG_OL(&sense_data->ADS, EEG_OL_n_value);
                   putEOG(&sense_data->ADS, EEG_EOG_n_value);
                   putEMG(&sense_data->ADS, EEG_EMG_n_value);
              putAux_data(&sense_data->AUX, AUXfactor_n);
              putIMU_data(&sense_data->IMU, IMUfactor_n);              
     }
    
  
}




void getADS_RAWdata(ADS_Data * data, bool ads_id){
     
      //--- EEG_FL
      for(int id=0; id<3; id++)data->EEG_FC[0][id]= 125;
      for(int id=0; id<3; id++)data->EEG_FC[1][id]= 125;
      
      //--- EEG_OL
      for(int id=0; id<3; id++)data->EEG_OL[0][id]= 125;        
      for(int id=0; id<3; id++)data->EEG_OL[1][id]= 125; 
      for(int id=0; id<3; id++)data->EEG_OL[2][id]= 127;        
      for(int id=0; id<3; id++)data->EEG_OL[3][id]= 427; 
      for(int id=0; id<3; id++)data->EEG_OL[4][id]= 157;        
      for(int id=0; id<3; id++)data->EEG_OL[5][id]= 157; 
      for(int id=0; id<3; id++)data->EEG_OL[6][id]= 157;        
      for(int id=0; id<3; id++)data->EEG_OL[7][id]= 157;      

      //--- EOG
      for(int id=0; id<3; id++)data->EOG[0][id]=158;
      for(int id=0; id<3; id++)data->EOG[1][id]=157;         

      //--- EMG
      for(int id=0; id<3; id++)data->EMG[0][id]=226;
      for(int id=0; id<3; id++)data->EMG[1][id]=225;
      for(int id=0; id<3; id++)data->EMG[2][id]=227;
      for(int id=0; id<3; id++)data->EMG[3][id]=228;        
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
    data->EDA=1257;
    data->PPG=65;
}

double getTimeStamp(void){
  return 15644257.345322;
}


// ----------------------------------------------------------------


void startHTMLserver(void){
  
WiFiClient tcpClient;
  fill_form ( tcpClient);
  
}



void UDP_Server_Benchmarking(char DBuffer[]) {
   
  int i = 0;
  
  String val;
  unsigned long StartT, timeP1, timeP2, duration =0;
  int packs = PACKETSNUM;  //1556
  int packetSize = sizeof(DBuffer);

      //Serial.println("Connected to server successful!");
      for (i = 0; i < ITERATIONUM; i++) {
        
          StartT = micros(); // ---------------- Measure Time - Starting Point 
          
          for (int a = 0; a < packs; a++) {
              Send_Data_PktBuffer(DBuffer);
           } 
           
          timeP1 = micros(); // ---------------- Measure Time - Time Point 1 

          Serial.print(i);
          Serial.print(": ");
          Serial.print(packs);
          Serial.print(" Packets in: ");
          Serial.print(timeP1 - StartT);
          Serial.print(" Micro Secs");
      
          Serial.print(" === Rate: ");
          Serial.print((timeP1 - StartT) / packs);
          Serial.print("us/Packet ===>  ");
      
          Serial.print(1000000 / ((timeP1 - StartT) / packs));
          Serial.print(" Packets/Sec ");
          Serial.print((1000000 / ((timeP1 - StartT) / packs)) * packetSize);
          Serial.println(" Bytes/Sec ");
   }
}




bool Receive_Command(void){
   
   bool validCMD = false;
   char packetBuffer[3]; //buffer to hold incoming packet

  // if there's data available, read a packet
  int packetSize = UDP_Server.parsePacket();
  if (packetSize) {
    Serial.print("RX Size: ");
    Serial.print(packetSize);

    /*
    Serial.print("From ");
    IPAddress remoteIp = UDP_Server.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(UDP_Server.remotePort());
   */

    // read the packet into packetBufffer
    int len = UDP_Server.read(packetBuffer, 3);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
  
   Serial.print(" -> Command: ");
   Serial.println(packetBuffer);
   validCMD = processChar(packetBuffer[0]);    
  }
  return validCMD;
}


void getUpdatedSensoryData(Sensors_Data * data){
      getADS_RAWdata(&data->ADS,1);
      getIMU_RAWdata(&data->IMU);   
      getAux_RAWdata(&data->AUX); 
}


void Data_Steam(Sensors_Data * data, DataPacket *packet, uint8_t pkgNumber,uint8_t IDnum, uint8_t signalPeriod){
   make_SignalSQwave(data, IDnum, signalPeriod);
   assemble_DataBuffer(data, pkgNumber, packet);
   sendUDP_DataPkt(packet);
}



  
void start_DStream(Sensors_Data * data, DataPacket *packet, uint8_t pkgNumber){
   getUpdatedSensoryData(data);
   assemble_DataBuffer(data,pkgNumber,packet);
   sendUDP_DataPkt(packet);
  
}


void assemble_DataBuffer(Sensors_Data *payload, uint8_t pkgID, DataPacket *packet){
    packet->PacketNum = pkgID;
    packet->Payload.AUX = payload->AUX;
    packet->Payload.ADS = payload->ADS;
    packet->Payload.IMU = payload->IMU;
    packet->timeStamp = putTimeStamp(); //getTimeStamp();
}


bool sendUDP_DataPkt (DataPacket *packet){

    byte Pkt[sizeof(struct DataPacket)];
    
    //memset(Pkt, NULL, sizeof(struct DataPacket)); 
    memcpy(Pkt,packet,sizeof(DataPacket));
    
    //size_t dataSize1 ;//= strlen(test);
    //Serial.print("Array Size: ");Serial.println(dataSize1);   

    UDP_Server.beginPacket(UDP_Server.remoteIP(), UDP_Server.remotePort());
    UDP_Server.write(Pkt,72);
   bool res = UDP_Server.endPacket();
   return res;
}



size_t Send_Data_PktBuffer(char DBuffer[]){
   
    size_t dataSize = sizeof(DBuffer);
    UDP_Server.beginPacket(UDP_Server.remoteIP(), UDP_Server.remotePort());
    UDP_Server.write(DBuffer);
    UDP_Server.endPacket();
    
  return dataSize;
}






bool CreateAccessPoint(char APssid[], char APpass[]){
  int APstatus = WL_IDLE_STATUS;
  Serial.println("Access Point Device");
  
  //pinMode(led, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address of will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));
  Serial.print("Creating access point named: ");
  Serial.println(APssid);

  // Create open network. Change this line if you want to create an WEP network:
  APstatus = WiFi.beginAP(APssid, APpass);
  
  if (APstatus != WL_AP_LISTENING) {
    Serial.println("Creating Access Point failed");
    // don't continue
    while (1);
  }
  
  // wait 10 seconds for connection:
  delay(10000);
  
   // you're connected now, so print out the status
  printWiFiStatus();


  return true;
  
 }


void test(){

 //eraseDev_Info(&nxrRom);
  
}




void new_wifi_setup (char *ssid, char *pass){
    if (WiFi.status () == WL_NO_SHIELD)
    {
        Serial.println ("WiFi shield not present");
        // don't continue:
        while (true);
    }
    int status = WL_IDLE_STATUS;
    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED)
    {
        Serial.print ("Attempting to connect to SSID: ");
        Serial.println (ssid);
        Serial.print ("pass: ");
        Serial.println (pass);
        status = WiFi.begin(ssid, pass);
        delay (10000);
    }
    Serial.println("Connected to wifi");
}



bool Connect_to_AP(char* ssid, char* pass){
  
    int keyIndex = 0;   // your network key Index number (needed only for WEP)
    int status = WL_IDLE_STATUS;
    unsigned int UDPPort = NETPORT;
    bool connState = false;
    uint8_t attemptCount=0;
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
   
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    attemptCount++;
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    
    if (attemptCount>3) {
      connState = false;
      break;
    }else{
      connState = true;
    }
  }

  if(connState) {
    Serial.println("");
    Serial.println("WiFi connected"); 
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }else{
    Serial.println("Failed to Connect"); 
  }
  return connState;
}




void startUDP_Server( unsigned int Port){
  UDP_Server.begin(Port);
}





void printWiFiStatus(void) {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}
