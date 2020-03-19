#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Ethernet.h>

// #define SECRET_SSID "raspi_wifi"
// #define SECRET_PASS "fluidfluid"
#define SECRET_SSID "gitgudbruh"
#define SECRET_PASS "giganticorchestra203"
#define HOST_ID     "192.168.0.13" /*"192.168.0.101"*/
#define PORT_NUM    8899

#define SEND_SIZE 18
// 1 for serial count, 
// 9 for ADS data,
// 6 for IMU data,)
// 1 for EDA
// 1 for temperature
// 1 for PPG
// 1 for battery level
#define NUM_ELEMENTS 20
#define ELEM_SIZE 4
#define PACKET_SIZE (ELEM_SIZE*NUM_ELEMENTS)

// INDICES of PACKET WHERE EACH CATEGORY STARTS
#define i_ADS 1
#define i_IMU i_ADS + 9
#define i_EDA i_IMU + 6
#define i_TEM i_EDA + 1
#define i_PPG i_TEM + 1
#define i_BAT i_PPG + 1
