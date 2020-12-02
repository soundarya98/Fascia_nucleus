
// ###################### SAMD21 Pin Table ####################

#define PA02 15 // =========== NXR1: A0 - EDA Signal
#define PA04 18 // =========== NXR1: LED Red
#define PA05 19 // =========== NXR1: A4 - VBATT Level
#define PA06 20 // =========== NXR1: D8 
#define PA07 21 // =========== NXR1: ADS1299 #0 - CLK
#define PB09 32 // =========== NXR1: A2
#define PB10 4  // =========== NXR1: DRDY
#define PA16 8  // =========== NXR1: ADS1299 #0/#1 - SPI MOSI
#define PA17 9  // =========== NXR1: ADS1299 #0/#1 - SPI SCK
#define PA18 24 // =========== NXR1: USB OTG sense
#define PA19 10 // =========== NXR1: ADS1299 #0/#1 - SPI MISO
#define PA20 6  // =========== NXR1: ADS1299 #1 - SPI CS2
#define PA21 7  // =========== NXR1: ADS1299 #0 - SPI CS1
#define PA22 0  // =========== NXR1: ADS1299 #0 - ext CLK trigger
#define PA23 1  // =========== NXR1: ADS1299 #0 - Reset
#define PB11 5  // =========== NXR1: CLK_SEL
  
#define PB08 31 // =========== NXR1: uBlox RST
#define PA08 11 // =========== NXR1: uBlox & IMU & Crypto - I2C 
#define PA09 12 // =========== NXR1: uBlox & IMU & Crypto - I2C 
#define PA12 26 // =========== NXR1: uBlox - TX_MOSI
#define PA13 27 // =========== NXR1: uBlox - RX_MISO
#define PA14 28 // =========== NXR1: uBlox - RTS_CS
#define PA15 29 // =========== NXR1: uBlox - CTS_SCK
#define PA27 30 // =========== NXR1: uBlox - GPIO0
#define PA28 35 // =========== NXR1: uBlox - ACK
#define PB22 14 // =========== NXR1: uBlox - TX
#define PB23 13 // =========== NXR1: uBlox - RX
#define PA10 2

#define PA11 3  
#define PB02 16 
#define PB03 17 
#define PB10 4
#define PA03 25 


// // ###################### UBLOX Pin Table ####################


//   //---- MKR1010 / NovaXR v1 --------
//   #define LED_GREEN 25 // =========== MKR1010: uBlox
//   #define LED_BLUE  27 // =========== MKR1010: uBlox
//   #define LED_RED   26 // =========== MKR1010: uBlox

//   //---- NovaXR V1 -------- 
//  // #define LED_BLUE  18 // =========== NXR: uBlox
//  // #define LED_RED   17 // =========== NXR: uBlox



// // #################### SAMD21 ADC Definitions #############
// #define ADC_READS_SHIFT      8
// #define ADC_READS_COUNT      (1 << ADC_READS_SHIFT)
// #define ADC_MIN_GAIN         0x0400
// #define ADC_UNITY_GAIN       0x0800
// #define ADC_MAX_GAIN         (0x1000 - 1)
// #define ADC_RESOLUTION_BITS  12
// #define ADC_RANGE            (1 << ADC_RESOLUTION_BITS)
// #define ADC_TOP_VALUE        (ADC_RANGE - 1)
// #define MAX_TOP_VALUE_READS  10


// // ################### ADS1299 Definitions #################
// #define ADS1299_ID  0x1E
// #define MASKADC_ADR 0x1F