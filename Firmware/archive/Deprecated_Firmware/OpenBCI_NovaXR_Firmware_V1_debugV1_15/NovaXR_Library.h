#ifndef NovaXR_Library_H
#define NovaXR_Library_H


#include "WiFi_Management.h"
#include "NXR_DataExchange.h"

#include "NXR_Flash.h"
#include "ADS1299_lib.h"
#include "LED_Indication.h"
#include "NXR_AUX_Sensors.h"
#include "NovaXR_PPG_Sensor.h"


// ----------------------- PIN Assignemts ---------------------------//
// ###################### SAMD21 Pin Table ####################
#define PA02 15 ///< NXR2: A0 - EDA Signal
#define PA05 19 ///< NXR2: A4 - VBATT Level
#define PA06 20 ///< NXR2: A5 - DVDD_ISO - EDA circuit VDD
#define PB09 32 ///< NXR2: A2 - DGND_ISO - EDA Circuite GND
#define PB02 16 ///< NXR2: A1 - HW EXT interrupt DRDY 2
#define PB10 4  ///< NXR2: D4 - HW EXT interrupt DRDY 1
#define PA20 6  ///< NXR2: ADS1299 #1 - SPI CS2
#define PA21 7  ///< NXR2: ADS1299 #0 - SPI CS1
#define PA07 21 ///< NXR1: A6 - ADS1299 #1 - DRDY Sister Board
#define PA10 2  ///< NXR1: ADS1299 #0 - DRDY
#define PA04 18 ///< NXR1: LED Red
#define PA18 24 ///< NXR1: USB OTG sense
#define PA16 8  ///< NXR1: ADS1299 #0/#1 - SPI MOSI
#define PA17 9  ///< NXR1: ADS1299 #0/#1 - SPI SCK
#define PA19 10 ///< NXR1: ADS1299 #0/#1 - SPI MISO
#define PB08 31 ///< NXR1: uBlox RST
#define PA08 11 ///< NXR1: uBlox & IMU & Crypto - I2C 
#define PA09 12 ///< NXR1: uBlox & IMU & Crypto - I2C 
#define PA12 26 ///< NXR1: uBlox - TX_MOSI
#define PA13 27 ///< NXR1: uBlox - RX_MISO
#define PA14 28 ///< NXR1: uBlox - RTS_CS
#define PA15 29 ///< NXR1: uBlox - CTS_SCK
#define PA27 30 ///< NXR1: uBlox - GPIO0
#define PA28 35 ///< NXR1: uBlox - ACK
#define PB22 14 ///< NXR1: uBlox - TX
#define PB23 13 ///< NXR1: uBlox - RX

#define PA11 3 
#define PB11 5   
#define PA22 0
#define PA23 1 
#define PB03 17 
#define PA03 25 

// ###################### UBLOX Pin Table ####################
#define LED_GREEN 27 ///< MKR1010 uBlox Green LED 
#define LED_BLUE  25 ///< MKR1010 uBlox Blue LED
#define LED_RED   26 ///< MKR1010 uBlox Red LED


// ------------ Prototypes ----------------
bool set_board_revision(void);



#endif
