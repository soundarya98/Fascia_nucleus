/*
   Author:        Ioannis Smanis
   Hardware:      NovaXR - MKR1010 board
   Firmware:      Version 1.0
   Firmware Component: NXR Utility Library source file
*/

#include "Arduino.h"
#include "OpenBCI_NovaXR.h"
#include "NXR_Communication.h"

#define DEBUG


// ------------------------ Library Declarations --------------------------------------------------------------
boolean streamingFlag = STOP_STREAMING;
WiFiServer TCP_Server(NETPORT);


// ----------------------------- Start TCP Server -------------------------------------------------------------
void startTCP_Server( unsigned int Port){
   TCP_Server.begin();
}


// ----------------------------- Data Exchange with TCP Client ------------------------------------------------
bool data_Exchange(void ){
 
    uint8_t i,id;
    bool state=false;
    
    WiFiClient TCP_Client = TCP_Server.available();   // listen for incoming clients
    
    if (TCP_Client){
       Serial.println("New TCP Client"); 
       id=0; 
       while (TCP_Client.connected()) {    
             
             commandCheck(TCP_Client); //  -------- Check Incomming Commands 
             
             #ifdef DEBUG
               Serial.print("Client Connection #1: "); Serial.println(TCP_Client.connected());
             #endif 
             
             if(streamingFlag){  
              
                #ifdef DEBUG
                  Serial.print("packet: ");Serial.println(id);  
                #endif  
                                
                if(id==255)id=0; // ------------------------------- Reset counter
                
                #ifdef DEBUG
                  long t1=micros();        
                Serial.print("Client Connection #2: "); Serial.println(TCP_Client.connected());
                #endif 
                
                assemble_outBuffer(id);
                
                
                #ifdef DEBUG
                  Serial.print("Client Connection #3: "); Serial.println(TCP_Client.connected());
                  long t2=micros();
                #endif
                
                newSend_outputBuffer(TCP_Client);    //  -------- Send Data Buffer 
                           
                #ifdef DEBUG
                  Serial.print("Client Connection #4: "); Serial.println(TCP_Client.connected());
                  long t3=micros();
                  Serial.print("    Packet Assembly time: \t");Serial.print(t2-t1);Serial.println("uS");
                  Serial.print("               Send time: \t");Serial.print(t3-t2);Serial.println("uS");
                  Serial.print("Total Data Exchange time: \t");Serial.print(t3-t1);Serial.println("uS");
                  Serial.print("            Packets  Num: \t");Serial.println(TCP_PKT_SIZE);
                 
                  Serial.print("Client Connection #5: "); Serial.println(TCP_Client.connected());
                #endif
                
                id = id+TCP_PKT_SIZE; 
                 
             }else{
                Serial.println("==== End of Stream =====");            
                TCP_Client.stop(); //  -------- Close TCP connection
                Serial.println("Client disonnected Normally ");   
                state=true;                      
                break;    
             }          
                                        
    }
        Serial.println("TCP Client Disonnected!");
        state=true;
    }
  return state;
}





// ----------------------------- Check Incoming Commands ------------------------------------------------
char commandCheck(WiFiClient &tcp_client){

     uint8_t validCMD_flag = 0;  // checked CMD
     char CMD='-';
     if(tcp_client.available()) {         // if there's bytes to read from the client,        
             
             #ifdef DEBUG 
                Serial.print("Client Connection 11: "); Serial.println(tcp_client.connected());
             #endif
             
             CMD = tcp_client.read(); 
           
             #ifdef DEBUG 
               Serial.print("Client Connection #12: "); Serial.println(tcp_client.connected());
               Serial.println("\n read CMD: ");Serial.println(CMD);  
             #endif  
                                         
             if(processChar(CMD)){              // -----  not checked CMD             
                 validCMD_flag=2;               // -----  checked Valid CMD 
                
                 #ifdef DEBUG 
                   Serial.println("checked: Valid CMD");
                   Serial.print("Client Connection #13: "); Serial.println(tcp_client.connected());
                 #endif   
                                      
              }else{
                validCMD_flag = 1;             // ----  checked NOT valid CMD
                Serial.println("checked: NOT valid CMD");
              }   
                  
       }else{ 
         validCMD_flag=0;  // not checked CMD
         #ifdef DEBUG 
           Serial.println("NOT available Data"); 
         #endif   
       }
   
   return CMD;
}







// ============================= SEND DATA =================================

void Data_Stream(WiFiClient &tcp_client, Sensors_Data * data, DataPacket *packet, uint8_t pkgNumber,uint8_t IDnum, uint8_t signalPeriod){
   make_SignalSQwave(data, IDnum, signalPeriod);
   assemble_DataBuffer(data, packet, pkgNumber);
   sendTCP_DataPkt(tcp_client, packet);
}



void start_DStream(WiFiClient &tcp_client, Sensors_Data * data, DataPacket *d_packet, uint8_t pkgNumber){
   long time1 = micros();
   getUpdatedSensoryData(data);
   long time2 = micros();
   assemble_DataBuffer(data,d_packet, pkgNumber);
   long time3 = micros();
   sendTCP_DataPkt(tcp_client, d_packet);
   long time4 = micros();
   Serial.print("get data: ");Serial.println(time2 - time1);
   Serial.print("assemble buffer: ");Serial.println(time3 - time2);
   Serial.print("send packet: ");Serial.println(time4 - time3);

}




// 1368 bytes to send
size_t Send_outputBuffer(WiFiClient &tcp_client){   
    
    byte Pkt[sizeof(outBuffer)];
    
    long time1 = micros();
    memcpy(Pkt,outBuffer,sizeof(outBuffer));
    long time2 = micros();    
    size_t ret = tcp_client.write(Pkt,sizeof(outBuffer)); 
    long time3 = micros();

   Serial.print("memcpy :\t");Serial.println(time2 - time1);
   Serial.print("send packet:\t");Serial.println(time3 - time2);
   Serial.println(); 
   
  return ret;
}



// 1368 bytes to send
size_t newSend_outputBuffer(WiFiClient &tcp_client){     
    size_t ret = tcp_client.write((byte*)outBuffer,sizeof(outBuffer)); 
  return ret;
}




size_t sendTCP_DataPkt (WiFiClient &tcp_client, DataPacket *packet ){

    byte Pkt[sizeof(struct DataPacket)];
    
    //memset(Pkt, NULL, sizeof(struct DataPacket)); 
    long A = micros();
    memcpy(Pkt,packet,sizeof(DataPacket));
    long B = micros();
    //size_t dataSize1 ;//= strlen(test);
    //Serial.print("Array Size: ");Serial.println(dataSize1);   

     size_t ret = tcp_client.write(Pkt,72);   
  
     long C = micros();
     
     Serial.print("UDP copy data: ");  Serial.println(B - A);
     Serial.print("UDP actual send: "); Serial.println(C - B);
  
   
   return ret;
}
