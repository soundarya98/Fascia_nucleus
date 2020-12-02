/*
   Author:        Ioannis Smanis
   Hardware:      NovaXR Board
   Firmware:      Version 1.0
   File:          Utility Library Header File
*/
#ifndef NXR_UTILITY_LIB_h
#define NXR_UTILITY_LIB_h


#include "Arduino.h"
#include "Data_Handler.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>


// ------ Macro Definitions -------------------
#define WIFISSID      "xxxxxxx"       // WiFi name
#define PASSWRD       "xxxxxxx"       // WiFi password
#define APSSID        "NovaXR"        // WiFi name
#define APSPASS       "novaxr1!"      // WiFi name
#define NETPORT		     2390           // UDP network port number
#define DPACKET_SIZE    72            // 72 bytes full unix timestamp



// =================== Prototypes ====================== //
// ------- Performane Test Methods --------------------- //
void UDP_Server_Benchmarking(char Buffer[]);
void startTCP_Server( unsigned int Port);

// ------- Send Data over WiFi ------------------------- //
size_t Send_Data_PktBuffer(char DBuffer[]);
size_t Send_Data(char Buffer[]);

size_t Send_TCP_outputBuffer(WiFiClient &tcp_client);
bool Send_UDP_outputBuffer(WiFiUDP &udp_server);

void startTCP_Server(unsigned int Port);
void startUDP_Server(unsigned int Port);
void getHeader(WiFiUDP &udp_server);
bool UDP_Data_Exchange(void);
bool TCP_Data_Exchange(void );
char TCPcommandCheck(WiFiClient &tcp_client);
char UDPcommandCheck(WiFiUDP &udp_server);

// ------- Test methods ----------------- //
void Data_Stream(WiFiClient &tcp_client, Sensors_Data * data, DataPacket *packet, uint8_t pkgNumber,uint8_t IDnum, uint8_t signalPeriod);
char commandCheck(WiFiClient &tcp_client);
void start_DStream(WiFiClient &tcp_client, Sensors_Data * data, DataPacket *d_packet, uint8_t pkgNumber);


#endif
