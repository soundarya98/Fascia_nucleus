/*
   Author:        Ioannis Smanis
   Hardware:      NovaXR - V1 board
   Firmware:      Version 1.13.2
   Firmware Component: NXR Communication Library source file
*/

#include "Arduino.h"
#include "OpenBCI_NovaXR.h"
#include "NXR_Communication.h"

//#define DEBUG
//#define DEBUG_CMD


// ------------------------ Library Declarations --------------------------------------------------------------
boolean streamingFlag = STOP_STREAMING;
uint16_t  UDP_PORT;
WiFiServer TCP_Server(NETPORT);
WiFiUDP UDP_Server;
IPAddress RemoteIP;
uint32_t pktCount =0;
bool countT=false;
bool countU=false;  
char Command_String[CMD_SIZE]="ZZZZZZZZ";



/**
 * @brief     Starts a TCP Server.
 *
 * @param     Port  The network TCP port
 */
void startTCP_Server( unsigned int Port){
   TCP_Server.begin();
   show_standBy_State();
   delay(400);
}



/**
 * @brief    Start a UDP Server.
 *
 * @param    Port  The network UDP port
 */
void startUDP_Server( unsigned int Port){
   UDP_Server.begin(Port);
   show_standBy_State();
   delay(400);
}



/**
 * @brief      Get Remote Client ID info.
 *
 * @param      udp_server  The udp server class instance
 */
void getHeader(WiFiUDP &udp_server){
   RemoteIP = udp_server.remoteIP();
   UDP_PORT = udp_server.remotePort();
}



/**
 * @brief    Test UDP and TCP data communication
 */
void UDP_or_TCP_data_exchage(void){

    char inCMD=0;
  //  if (Serial.available() > 0) {
   if(1){ // demo
        Serial.println("  Type U for UDP or T for TCP accordingly    ");
        // read the incoming byte:
        inCMD = Serial.read();
       inCMD ='U';
       if(inCMD =='U'){
            // ---- Start a UDP Server ------------------------------------------
            Serial.println("\n>>> Starting UDP Streamer....");            
            if(!countU)startUDP_Server(NETPORT);
            countU=true;
            inCMD=0; // Reset incoming Command
            while(1){
              long starT = micros();
              bool check1 = UDP_Data_Exchange();
              long endT = micros();
              if(check1){ Serial.print("UDP Session Duration: ");Serial.println(endT - starT); Serial.println(); break; }
            }
            
        }else if(inCMD =='T'){
            // ---- Start a TCP Server ------------------------------------------
            Serial.println("\n>>> Starting TCP Streamer....");          
            if(!countT)startTCP_Server(NETPORT); 
            countT=true;
            inCMD=0; // Reset incoming Command         
            while(1){
              long starT = micros();
              bool check2 = TCP_Data_Exchange();
              long endT = micros();
              if(check2){ Serial.print("TCP Session Duration: ");Serial.println(endT - starT);Serial.println(); break; }
            }               
       } else if(inCMD =='R'){
            // ---- Start a TCP Server ------------------------------------------
            Serial.println("\n>>> Starting TCP Streamer .. SW buffer ....");          
            if(!countT)startTCP_Server(NETPORT); 
            countT=true;
            inCMD=0; // Reset incoming Command         
            while(1){
              long starT = micros();
              bool check2 = TCP_DataSW_Exchange();
              long endT = micros();
              if(check2){ Serial.print("TCP Session SW Duration: ");Serial.println(endT - starT);Serial.println(); break; }
            }               
       } else if(inCMD =='S'){
            // ---- Start a UDP Server ------------------------------------------
            Serial.println("\n>>> Starting UDP Streamer.. SW buffer ....");            
            if(!countU)startUDP_Server(NETPORT);
            countU=true;
            inCMD=0; // Reset incoming Command
            while(1){
              long starT = micros();
              bool check1 = UDP_DataSW_Exchange();
              long endT = micros();
              if(check1){ Serial.print("UDP Session SW Duration: ");Serial.println(endT - starT); Serial.println(); break; }
            }
        
    }

  }
}



/**
 * @brief     Data Exchange with UDP Client
 *
 * @return    returns the Data exchange Status: True for still active or false for stopped operation
 */
bool UDP_Data_Exchange(void){

    uint8_t id=0;
    uint16_t byteCounter=0;
    bool i,state=false;  
    
    show_standBy_State();
    if(UDPcommandCheck(UDP_Server)==OPENBCI_STREAM_START){
        getHeader(UDP_Server);
        
        while (1){  // ---- Send forever UDP packets  Unless you get stop message   
             long t0=micros();           
               UDPcommandCheck(UDP_Server);                 // --- Check incomming Commands
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
                printBatt_level();
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



/**
 * @brief     Data Exchange with UDP Client using Switch Buffers
 *
 * @return    returns the Data exchange Status: True for still active or false for stopped operation
 */
bool UDP_DataSW_Exchange(void){

    uint8_t id=0;
    uint16_t byteCounter=0;
    bool i,state=false;  
    reset_swBuffers();
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
                
                uint8_t empty_id = getEmpty_swBuffer_ID();
               // Serial.print("empty_ID: : ");Serial.println(empty_id);  
                assemble_swBuffer(id,empty_id);                            
                markFull_swBuffer(empty_id);   
                    
                #ifdef DEBUG                 
                  long t2=micros();
                #endif
                
                uint8_t full_id = getFull_swBuffer_ID();
                //Serial.print("full_ID: : ");Serial.println(full_id); 
                Send_UDP_swBuffer(UDP_Server,full_id);
                markEmpty_swBuffer(full_id);   

                   
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
                printBatt_level();
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



/**
 * @brief     Data Exchange with TCP Client
 *
 * @return    returns the Data exchange Status: True for still active or false for stopped operation
 */
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
                TCP_Client.flush(); 
                TCP_Client.stop();   // ------------ Close TCP connection          
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
            TCP_Client.flush(); 
            TCP_Client.stop();
            diconnected_Client_Indication();   // ----------- Unexpected TCP Client 
            Serial.println(">>>>>> TCP Client Disonnected !! <<<<<");
          }        
          state=true;
    }
    return state;
}



/**
 * @brief    Check Incoming Commands over TCP
 *
 * @param    tcp_client  Sets the TCP client class instance
 *
 * @return   returns the incomming command
 */
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
           Serial.println();
         #endif   
       }
   
   return CMD;
}



/**
 * @brief     Check Incoming Commands over UDP
 *
 * @param     udp_server  Sets the UDP server class instance
 *
 * @return    returns the incomming command
 */
char UDPcommandCheck(WiFiUDP &udp_server){

     uint8_t validCMD_flag = 0;  // checked CMD     
     if(udp_server.parsePacket()) {         // if there's bytes to read from the client,            
           udp_server.read(Command_String, 9);               
           #ifdef DEBUG_CMD
             Serial.print("\n --> Read CMD: ");Serial.println(Command_String);
           #endif  
                                       
           if(processChar(Command_String[0])){              // -----  not checked CMD             
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
   
   return Command_String[0];
}



/**
 * @brief    Data Exchange with TCP Client Switch Buffers
 *
 * @return   returns the Data exchange Status: True for still active or false for stopped operation
 */
bool TCP_DataSW_Exchange(void ){
 
    uint8_t id=0;
    bool i,state=false; 
    WiFiClient TCP_Client = TCP_Server.available();   // listen for incoming clients
    
    if (TCP_Client){
       Serial.println("New TCP Client"); 
       id=0; 
       show_standBy_State();
       reset_swBuffers();
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
                
                uint8_t empty_id = getEmpty_swBuffer_ID();
               // Serial.print("empty_ID: : ");Serial.println(empty_id);  
                assemble_swBuffer(id,empty_id);                            
                markFull_swBuffer(empty_id);

                                           
                #ifdef DEBUG                 
                  long t2=micros();
                  Serial.print("Client Connection #3: "); Serial.println(TCP_Client.connected());
                  long t3=micros();
                #endif
                
                uint8_t full_id = getFull_swBuffer_ID();
                //Serial.print("full_ID: : ");Serial.println(full_id); 
                Send_TCP_swBuffer(TCP_Client,full_id);
                markEmpty_swBuffer(full_id);   

                       
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
                Serial.println("/=========== End of TCP swStream ===========/");  
                TCP_Client.flush();  // Test 2
                TCP_Client.stop();              // ------------ Close TCP connection          
                Serial.print(" Total D_Packets Sent:\t");Serial.print(pktCount*UDP_TCP_PKT_SIZE);Serial.println("\tPackets");
                Serial.print(" Total Data Sent:\t");Serial.print(pktCount*UDP_TCP_PKT_SIZE*DPACKET_SIZE);Serial.println("\tBytes");
                Serial.println("/**** TCP Client SW disonnected Normally ****/"); 
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
            Serial.println(">>>>>> TCP Client sw Disonnected !! <<<<<");
          }        
          state=true;
    }
    return state;
}







// ============================= SEND DATA =================================

/**
 * @brief      Sends the Data Buffer over TCP.
 *
 * @param      tcp_client  Sets The TCP client calss instance
 *
 * @return     returns how many bytes are sent successfully 
 */
size_t Send_TCP_outputBuffer(WiFiClient &tcp_client){
    pktCount++;      
    size_t res = tcp_client.write((byte*)outBuffer,sizeof(outBuffer));  
    return res;
}


/**
 * @brief      Sends the Data Buffer over UDP.
 *
 * @param      udp_server  Sets the UDP server calss instance
 *
 * @return     returns true if the data transaction performed successfully
 */
bool Send_UDP_outputBuffer(WiFiUDP &udp_server){   
    pktCount++; 
    udp_server.beginPacket(RemoteIP, UDP_PORT);
    udp_server.write((byte*)outBuffer,sizeof(outBuffer)); 
    bool res = udp_server.endPacket();    
    return res;
}


/**
 * @brief      Sends the Switch Output Buffer over TCP.
 *
 * @param      tcp_client      Sets The TCP client calss instance
 * @param[in]  full_swBuff_id  The full switch-buffer identifier
 *
 * @return     returns how many bytes are sent successfully 
 */
size_t Send_TCP_swBuffer(WiFiClient &tcp_client, uint8_t full_swBuff_id){   
    pktCount++;
    size_t ret = tcp_client.write((byte*)swOutBuffer[full_swBuff_id].oBuff, sizeof(swOutBuffer[full_swBuff_id].oBuff)); 
    return ret;
}


// --------------- Sends a Switch Output Buffer over UDP -------------------------------

/**
 * @brief      Sends a Switch Output Buffer over UDP.
 *
 * @param      udp_server      Sets the UDP server calss instance
 * @param[in]  full_swBuff_id  The full switch-buffer identifier
 *
 * @return     returns true if the data transaction performed successfully
 */
bool Send_UDP_swBuffer(WiFiUDP &udp_server, uint8_t full_swBuff_id){   
    pktCount++; 
    udp_server.beginPacket(RemoteIP, UDP_PORT);
    udp_server.write((byte*)swOutBuffer[full_swBuff_id].oBuff,sizeof(swOutBuffer[full_swBuff_id])); 
    bool res = udp_server.endPacket();    
    return res;
}
