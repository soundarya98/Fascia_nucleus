#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Ethernet.h>

// #define SECRET_SSID "raspi_wifi"
// #define SECRET_PASS "fluidfluid"

//#define SECRET_SSID "gitgudbruh"
//#define SECRET_PASS "giganticorchestra203"

#define SECRET_SSID "#8103"
#define SECRET_PASS "1423qweasd"
#define HOST_ID     "10.0.0.74"
#define PORT_NUM    8899

#define SEND_SIZE 22
// 1 for serial count, 
// 1 for valid array for the data packet (1 maps to data means invalid)
// 8 for ADS data, ( - status bits, incorporated into valid array)
// 6 for IMU data,
// 1 for EDA
// 1 for temperature
// 1 for PPG
#define NUM_ELEMENTS 17
#define ELEM_SIZE 4
#define PACKET_SIZE (ELEM_SIZE*NUM_ELEMENTS)

// INDICES of PACKET WHERE EACH CATEGORY STARTS
#define i_VALID 1
#define i_ADS i_VALID + 1
#define i_IMU i_ADS + 8
#define i_EDA i_IMU + (3) /* IMU is 6 points, each of which is only 2 bytes */
#define i_TEM i_EDA + 1
#define i_PPG i_TEM + 1
#define i_TIM i_PPG + 1

// Indices of the valid bit for each of these data 
// (this is not the same as the indices above due to the size 
//  difference between the data-- the IMU data is only 2 bytes, 
//  but are 6 differernt data points)
#define v_ADS 2
#define v_IMU v_ADS + 8
#define v_EDA v_IMU + 6
#define v_TEM v_EDA + 1
#define v_PPG v_TEM + 1
#define v_TIM v_PPG + 1
