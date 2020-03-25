#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Ethernet.h>

// #define SECRET_SSID "raspi_wifi"
// #define SECRET_PASS "fluidfluid"
<<<<<<< Updated upstream
#define SECRET_SSID "Rotaryguru2"
#define SECRET_PASS "rx7turbo2"
#define HOST_ID     "10.0.0.242" /*"192.168.0.101"*/
=======
//#define SECRET_SSID "gitgudbruh"
//#define SECRET_PASS "giganticorchestra203"
#define SECRET_SSID "#8103"
#define SECRET_PASS "1423qweasd"
#define HOST_ID     "10.0.0.77"/*"192.168.0.13" /*"192.168.0.101"*/
>>>>>>> Stashed changes
#define PORT_NUM    8899

#define SEND_SIZE 19
// 1 for serial count, 
// 1 for valid array for the data packet (1 maps to data means invalid)
// 8 for ADS data, ( - status bits, incorporated into valid array)
// 6 for IMU data,
// 1 for EDA
// 1 for temperature
// 1 for PPG
#define NUM_ELEMENTS 19
#define ELEM_SIZE 4
#define PACKET_SIZE (ELEM_SIZE*NUM_ELEMENTS)

// INDICES of PACKET WHERE EACH CATEGORY STARTS
#define i_VALID 1
#define i_ADS i_VALID + 1
#define i_IMU i_ADS + 8
#define i_EDA i_IMU + 6
#define i_TEM i_EDA + 1
#define i_PPG i_TEM + 1
