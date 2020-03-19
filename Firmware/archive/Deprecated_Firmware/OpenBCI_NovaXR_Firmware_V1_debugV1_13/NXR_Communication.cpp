/*
   Author:        Ioannis Smanis
   Hardware:      NovaXR - MKR1010 board
   Firmware:      Version 1.12.4
   Firmware Component: NXR Utility Library source file
*/

#include "Arduino.h"
#include "OpenBCI_NovaXR.h"
#include "NXR_Communication.h"

#define DEBUG
//#define DEBUG_CMD

// ------------------------ Library Declarations --------------------------------------------------------------
boolean streamingFlag = STOP_STREAMING;
uint16_t  UDP_PORT;
WiFiServer TCP_Server(NETPORT);
WiFiUDP UDP_Server;
IPAddress RemoteIP;
uint32_t pktCount =0;

// ----------------------------- Start TCP Server -------------------------------------------------------------
void startTCP_Server( unsigned int Port){
   TCP_Server.begin();
   show_standBy_State();
   delay(400);
}


// ----------------------------- Start UDP Server -------------------------------------------------------------
void startUDP_Server( unsigned int Port){
   UDP_Server.begin(Port);
   show_standBy_State();
   delay(400);
}

// ----------------------------- get Remote Client ID --------------------------------------------------------- 
void getHeader(WiFiUDP &udp_server){
   RemoteIP = udp_server.remoteIP();
   UDP_PORT = udp_server.remotePort();
}


// --------------------------- Data Exchange with UDP Client --------------------------------------------------
bool UDP_Data_Exchange(void){

    uint8_t id=0;
    uint16_t byteCounter=0;
    bool i,state=false;  
    
    show_standBy_State();
    if(UDPcommandCheck(UDP_Server)==OPENBCI_STREAM_START){
        getHeader(UDP_Server);
        
        while (1){  // ---- Send forever UDP packets  Unless you get stop message   
             long t0=micros();           
               UDPcommandCheck(UDP_Server);                      // --- Check incomming Commands
             long tA=micros();
             if(streamingFlag && i)show_streamming_State(); // --- Turn ON the GREEN LED Once                       
             if(streamingFlag){               
                i=true;
                #ifdef DEBUG
                  Serial.print("Packet: ");Serial.println(id);  
                #endif  
                                
                if(id==255){
                   id=0;  // Reset counter
                   byteCounter++;
                }
                
                #ifdef DEBUG
                  long t1=micros();     
                #endif
                
                assemble_outBuffer(id);        
                #ifdef DEBUG                 
                  long t2=micros();
                #endif
                
                Send_UDP_outputBuffer(UDP_Server);       
                #ifdef DEBUG
                  long t3=micros();
                  Serial.print("      Command Check time: \t");Serial.print(tA-t0);Serial.println("uS");
                  Serial.print("    Packet Assembly time: \t");Serial.print(t2-t1);Serial.println("uS");
                  Serial.print("               Send time: \t");Serial.print(t3-t2);Serial.println("uS");
                  Serial.print("Total Data Exchange time: \t");Serial.print((t2-t1)+(t3-t2));Serial.println("uS");
                  Serial.print("              Total time: \t");Serial.print((tA-t0)+(t2-t1)+(t3-t2));Serial.println("uS");
                  Serial.print("          Packets in UDP: \t");Serial.println(UDP_TCP_PKT_SIZE);
                #endif             
                id = id+UDP_TCP_PKT_SIZE; 
                  
             }else{               
                i=false;
                Serial.println("\n/=========================================/"); 
                Serial.println("/=========== End of UDP Stream ===========/");    
                // ------------ Close UDP connection
                Serial.print(" Total D_Packets Sent:\t");Serial.print(pktCount*UDP_TCP_PKT_SIZE);Serial.println("\tPackets");
                Serial.print(" Total Data Sent:\t");Serial.print(pktCount*UDP_TCP_PKT_SIZE*DPACKET_SIZE);Serial.println("\tBytes");
                Serial.println("/**** UDP Client disonnected Normally ****/");
                Serial.println("/=========================================/\n");  
                standBy_mode(); 
                state=true;                      
                break;    
             }                                                
          }
                
          if(streamingFlag && i){
            diconnected_Client_Indication();   // ----------- Unexpected TCP Client 
            Serial.println(">>>>>> UDP Client Disonnected !! <<<<<");
          }        
          state=true;
     }
    return state;
}


// ----------------------------- Data Exchange with TCP Client ------------------------------------------------
bool TCP_Data_Exchange(void ){
 
    uint8_t id=0;
    bool i,state=false; 
    WiFiClient TCP_Client = TCP_Server.available();   // listen for incoming clients
    
    if (TCP_Client){
       Serial.println("New TCP Client"); 
       id=0; 
       show_standBy_State();
       
       while (TCP_Client.connected()) {    
             long t0=micros(); 
             TCPcommandCheck(TCP_Client);                    // --- Check incomming Commands
             long tA=micros();
             if(streamingFlag && i)show_streamming_State();  // --- Turn ON the GREEN LED Once           
             #ifdef DEBUG
               Serial.print("Client Connection #1: "); Serial.println(TCP_Client.connected());
             #endif
              
             if(streamingFlag){               
                i=true;
                #ifdef DEBUG
                  Serial.print("packet: ");Serial.println(id);  
                #endif  
                                
                if(id==255)id=0; // Reset counter             
                #ifdef DEBUG
                  Serial.print("Client Connection #2: "); Serial.println(TCP_Client.connected());
                  long t1=micros();     
                #endif
                
                assemble_outBuffer(id);                 
                #ifdef DEBUG                 
                  long t2=micros();
                  Serial.print("Client Connection #3: "); Serial.println(TCP_Client.connected());
                  long t3=micros();
                #endif
                
                Send_TCP_outputBuffer(TCP_Client);             
                #ifdef DEBUG
                  long t4=micros();
                  Serial.print("      Command Check time: \t");Serial.print(tA-t0);Serial.println("uS");
                  Serial.print("    Packet Assembly time: \t");Serial.print(t2-t1);Serial.println("uS");
                  Serial.print("               Send time: \t");Serial.print(t4-t3);Serial.println("uS");
                  Serial.print("Total Data Exchange time: \t");Serial.print((t2-t1)+(t4-t3));Serial.println("uS");
                  Serial.print("              Total time: \t");Serial.print((tA-t0)+(t2-t1)+(t4-t3));Serial.println("uS");
                  Serial.print("          Packets in TCP: \t");Serial.println(UDP_TCP_PKT_SIZE);
                  Serial.print("    Client Connection #4: \t");Serial.println(TCP_Client.connected());
                #endif            
                id = id+UDP_TCP_PKT_SIZE; 
                  
             }else{               
                i=false;
                Serial.println("\n/=========================================/"); 
                Serial.println("/=========== End of TCP Stream ===========/");  
                TCP_Client.flush();  // Test 2
                TCP_Client.stop();              // ------------ Close TCP connection          
                Serial.print(" Total D_Packets Sent:\t");Serial.print(pktCount*UDP_TCP_PKT_SIZE);Serial.println("\tPackets");
                Serial.print(" Total Data Sent:\t");Serial.print(pktCount*UDP_TCP_PKT_SIZE*DPACKET_SIZE);Serial.println("\tBytes");
                Serial.println("/**** TCP Client disonnected Normally ****/"); 
                Serial.println("/=========================================/\n"); 
                standBy_mode(); 
                state=true;                      
                break;    
             }                                               
          } 
          Serial.print("    Client Connection #51: \t");Serial.println(TCP_Client.connected());     
          if((!TCP_Client.connected())&&(streamingFlag)){
            TCP_Client.flush();  // Test 2
            TCP_Client.stop();
            diconnected_Client_Indication();   // ----------- Unexpected TCP Client 
            Serial.println(">>>>>> TCP Client Disonnected !! <<<<<");
          }        
          state=true;
    }
    return state;
}



// ----------------------------- Check Incoming Commands over TCP ------------------------------------------------
char TCPcommandCheck(WiFiClient &tcp_client){

     uint8_t validCMD_flag = 0;  // checked CMD
     char CMD='-';
     if(tcp_client.available()) {         // if there's bytes to read from the client,            
             CMD = tcp_client.read();         
             #ifdef DEBUG_CMD 
               Serial.print("\n--> Read CMD: ");Serial.println(CMD);
               Serial.print("Client Connection #5: "); Serial.println(tcp_client.connected());  
             #endif  
                                         
             if(processChar(CMD)){              // -----  not checked CMD             
                 validCMD_flag=2;               // -----  checked Valid CMD 
                 #ifdef DEBUG_CMD 
                   Serial.println("checked: Valid CMD");
                   Serial.print("Client Connection #6: "); Serial.println(tcp_client.connected());
                 #endif             
              }else{
                validCMD_flag = 1;             // ----  checked NOT valid CMD
                #ifdef DEBUG_CMD
                  Serial.print("Client Connection #7: "); Serial.println(tcp_client.connected()); 
                  Serial.println("checked: NOT valid CMD");
                #endif 
              }   
                              
       }else{ 
         validCMD_flag=0;  // not checked CMD
         #ifdef DEBUG_CMD 
           Serial.print("Client Connection #8: "); Serial.println(tcp_client.connected());
           Serial.println("NOT available Data"); 
         #endif   
       }
   
   return CMD;
}



// ----------------------------- Check Incoming Commands over UDP ------------------------------------------------
char UDPcommandCheck(WiFiUDP &udp_server){

     uint8_t validCMD_flag = 0;  // checked CMD
     char CMD[2];

     if(udp_server.parsePacket()) {         // if there's bytes to read from the client,            
           udp_server.read(CMD, 2);               
           #ifdef DEBUG_CMD
             Serial.print("\n--> Read CMD: ");Serial.println(CMD);
           #endif  
                                       
           if(processChar(CMD[0])){              // -----  not checked CMD             
               validCMD_flag=2;               // -----  checked Valid CMD 
               #ifdef DEBUG_CMD 
                 Serial.println("checked: Valid CMD");
               #endif             
            }else{
               validCMD_flag = 1;             // ----  checked NOT valid CMD
               #ifdef DEBUG_CMD
                 Serial.println("checked: NOT valid CMD");
               #endif
            }   
                  
       }else{ 
           validCMD_flag=0;  // not checked CMD  
       }
   
   return CMD[0];
}








// ============================= SEND DATA =================================

// --------------- Send Data Buffer over TCP -------------------------------
size_t Send_TCP_outputBuffer(WiFiClient &tcp_client){
    pktCount++;      
    size_t res = tcp_client.write((byte*)outBuffer,sizeof(outBuffer));  
    return res;
}


// --------------- Send Data Buffer over UDP -------------------------------
bool Send_UDP_outputBuffer(WiFiUDP &udp_server){   
    pktCount++; 
    udp_server.beginPacket(RemoteIP, UDP_PORT);
    udp_server.write((byte*)outBuffer,sizeof(outBuffer)); 
    bool res = udp_server.endPacket();    
    return res;
}
