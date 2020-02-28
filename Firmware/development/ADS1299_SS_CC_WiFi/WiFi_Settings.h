#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Ethernet.h>

#define SECRET_SSID "raspi_wifi"
#define SECRET_PASS "fluidfluid"
#define HOST_ID     "192.168.0.103"
#define PORT_NUM    8899

#define SEND_SIZE 25
#define PACKET_SIZE 40

// /*extern */int cnt = 0;

// void setupWifi(void);
// void pushToBuf(char* packet);
// void sendWiFiDataPacket(void);
