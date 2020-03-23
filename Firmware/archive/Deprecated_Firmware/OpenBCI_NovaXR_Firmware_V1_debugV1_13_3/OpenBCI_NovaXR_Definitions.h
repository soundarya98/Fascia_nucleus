//
//  Definitions_32.h
//
//  Created by Conor Russomanno, Luke Travis, and Joel Murphy. Summer 2013.
//	Modified by Joel Murphy, Summer 2014
//	Joel Added Daisy Functionality Fall 2014
//
//	Modified by Ioannis Smanis 			- Summer 2019
//	Ioannis Smanis added NovaXR support - September 2019
//

#ifndef _____OpenBCI_NovaXR_Definitions_h
#define _____OpenBCI_NovaXR_Definitions_h


#define MKR1010
/* ------ NovaXR packet format --------
 * Packet Byte [0]:     Packet Number
 * Packet Byte [1]:     PPG
 * Packet Byte [2:3]:   EDA
 * Packet Byte [4:6]:   EEG_FC 0
 * Packet Byte [7:9]:   EEG_FC 1
 * Packet Byte [10:12]: EEG_OL 0
 * Packet Byte [13:15]: EEG_OL 1
 * Packet Byte [16:18]: EEG_OL 2
 * Packet Byte [19:21]: EEG_OL 3
 * Packet Byte [22:24]: EEG_OL 4
 * Packet Byte [25:27]: EEG_OL 5
 * Packet Byte [28:30]: EEG_OL 6
 * Packet Byte [31:33]: EEG_OL 7 
 * Packet Byte [34:36]: EOG 0 
 * Packet Byte [37:39]: EOG 1 
 * Packet Byte [40:42]: EMG 0 
 * Packet Byte [43:45]: EMG 1 
 * Packet Byte [46:48]: EMG 2 
 * Packet Byte [49:51]: EMG 3 
 * Packet Byte [52:53]: AXL X  
 * Packet Byte [54:55]: AXL Y
 * Packet Byte [56:57]: AXL Z 
 * Packet Byte [58:59]: GYR X 
 * Packet Byte [60:61]: GYR Y 
 * Packet Byte [62:63]: GYR Z 
 * Packet Byte [64:71]: Timestamp 
 */


// ---------- PIN Assignemts ----------------------------------//
#define ADS_DRDY 	9       // ADS data ready pin
#define ADS_RST		4       // ADS reset pin
#define BOARD_ADS	8       // ADS chip select
#define DAISY_ADS	3       // ADS Daisy chip select
#define BOTH_ADS 	5	    // Slave Select Both ADS chips

// #define WIFI_SS 13          // Wifi Chip Select
#define OPENBCI_PIN_LED 11
#define OPENBCI_PIN_PGC 12
// #define WIFI_RESET 18

// ----------------------//----------------------//-------------//

// ###################### SAMD21 Pin Table ####################
#define PA02 15 // =========== NXR2: A0 - EDA Signal
#define PA05 19 // =========== NXR2: A4 - VBATT Level
#define PA06 20 // =========== NXR2: A5 - DVDD_ISO - EDA circuit VDD
#define PB09 32 // =========== NXR2: A2 - DGND_ISO - EDA Circuite GND

#define PB02 16 // =========== NXR2: A1 - HW EXT interrupt DRDY 2
#define PB10 4  // =========== NXR2: D4 - HW EXT interrupt DRDY 1
#define PA20 6  // =========== NXR2: ADS1299 #1 - SPI CS2
#define PA21 7  // =========== NXR2: ADS1299 #0 - SPI CS1


#define PA07 21 // =========== NXR1: A6 - ADS1299 #1 - DRDY Sister Board
#define PA10 2  // =========== NXR1: ADS1299 #0 - DRDY
#define PA04 18 // =========== NXR1: LED Red
#define PA18 24 // =========== NXR1: USB OTG sense

#define PA16 8  // =========== NXR1: ADS1299 #0/#1 - SPI MOSI
#define PA17 9  // =========== NXR1: ADS1299 #0/#1 - SPI SCK
#define PA19 10 // =========== NXR1: ADS1299 #0/#1 - SPI MISO


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

#define PA11 3 
#define PB11 5   
#define PA22 0
#define PA23 1 
#define PB02 16 
#define PB03 17 
#define PA03 25 


// ----------------------//----------------------//-------------//

// ###################### UBLOX Pin Table ####################

#ifdef MKR1010
  //---- MKR1010 --------
  #define LED_GREEN 27 // =========== MKR1010: uBlox
  #define LED_BLUE  25 // =========== MKR1010: uBlox
  #define LED_RED   26 // =========== MKR1010: uBlox
#else
  //---- NovaXR V1 -------- 
  #define LED_BLUE  18 // =========== NXR: uBlox
  #define LED_RED   17 // =========== NXR: uBlox
#endif


//******************** ADS1299 Configs and Registers ********************
// ################### ADS1299 Definitions #################
#define ADS1299_ID  0x1E
#define MASKADC_ADR 0x1F
// ################### ADS1299 Definitions #################


//ADS1299 SPI Command Definition Byte Assignments
#define _WAKEUP 	0x02 // Wake-up from standby mode
#define _STANDBY 	0x04 // Enter Standby mode
#define _RESET 		0x06 // Reset the device registers to default
#define _START 		0x08 // Start and restart (synchronize) conversions
#define _STOP 		0x0A // Stop conversion
#define _RDATAC 	0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 	0x11 // Stop Read Data Continuous mode
#define _RDATA 		0x12 // Read data by command supports multiple read back

//ASD1299 Register Addresses
#define ADS_ID	0x3E	// product ID for ADS1299
#define ID_REG  0x00	// this register contains ADS_ID
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 	0x04
#define CH1SET	0x05
#define CH2SET 	0x06
#define CH3SET 	0x07
#define CH4SET 	0x08
#define CH5SET 	0x09
#define CH6SET 	0x0A
#define CH7SET 	0x0B
#define CH8SET 	0x0C
#define BIAS_SENSP 	0x0D
#define BIAS_SENSN 	0x0E
#define LOFF_SENSP 	0x0F
#define LOFF_SENSN 	0x10
#define LOFF_FLIP	0x11
#define LOFF_STATP 	0x12
#define LOFF_STATN 	0x13
#define GPIO 		0x14
#define MISC1 		0x15
#define MISC2 		0x16
#define CONFIG4 	0x17

#define OUTPUT_NOTHING	(0) // quiet
#define OUTPUT_8_CHAN 	(1) // not using Daisy module
#define OUTPUT_16_CHAN 	(2) // using Daisy module
#define ON_BOARD		(8)	// slave address for on board ADS
#define ON_DAISY		(3)	// slave address for daisy ADS

// CHANNEL SETTINGS
#define POWER_DOWN      (0)
#define GAIN_SET        (1)
#define INPUT_TYPE_SET  (2)
#define BIAS_SET        (3)
#define SRB2_SET        (4)
#define SRB1_SET        (5)
#define YES      	(0x01)
#define NO      	(0x00)

//gainCode choices
#define ADS_GAIN01 (0b00000000)	// 0x00
#define ADS_GAIN02 (0b00010000)	// 0x10
#define ADS_GAIN04 (0b00100000)	// 0x20
#define ADS_GAIN06 (0b00110000)	// 0x30
#define ADS_GAIN08 (0b01000000)	// 0x40
#define ADS_GAIN12 (0b01010000)	// 0x50
#define ADS_GAIN24 (0b01100000)	// 0x60

//inputType choices
#define ADSINPUT_NORMAL     (0b00000000)
#define ADSINPUT_SHORTED    (0b00000001)
#define ADSINPUT_BIAS_MEAS  (0b00000010)
#define ADSINPUT_MVDD       (0b00000011)
#define ADSINPUT_TEMP       (0b00000100)
#define ADSINPUT_TESTSIG    (0b00000101)
#define ADSINPUT_BIAS_DRP   (0b00000110)
#define ADSINPUT_BIAL_DRN   (0b00000111)

//test signal choices...ADS1299 datasheet page 41
#define ADSTESTSIG_AMP_1X 		  	(0b00000000)
#define ADSTESTSIG_AMP_2X 		  	(0b00000100)
#define ADSTESTSIG_PULSE_SLOW 		(0b00000000)
#define ADSTESTSIG_PULSE_FAST 		(0b00000001)
#define ADSTESTSIG_DCSIG 		    	(0b00000011)
#define ADSTESTSIG_NOCHANGE 		  (0b11111111)

#define ADS1299_CONFIG1_DAISY 		(0b10110000)
#define ADS1299_CONFIG1_DAISY_NOT (0b10010000)

//Lead-off signal choices
#define LOFF_MAG_6NA        (0b00000000)
#define LOFF_MAG_24NA       (0b00000100)
#define LOFF_MAG_6UA        (0b00001000)
#define LOFF_MAG_24UA       (0b00001100)
#define LOFF_FREQ_DC        (0b00000000)
#define LOFF_FREQ_7p8HZ     (0b00000001)
#define LOFF_FREQ_31p2HZ    (0b00000010)
#define LOFF_FREQ_FS_4      (0b00000011)
#define PCHAN 	            (0)
#define NCHAN 	            (1)
#define OFF 	              (0)
#define ON 		              (1)

// used for channel settings
#define ACTIVATE_SHORTED 	(2)
#define ACTIVATE 			    (1)
#define DEACTIVATE 		  	(0)

//**********************//**********************//**********************//**********************
#define CH_NORMAL_ELECTRODE     0           ///< Channel is connected to the corresponding positive and negative input pins.
#define CH_INPUT_SHORTED        1           ///< Channel inputs are shorted together. Used for offset and noise measurements.
#define CH_BIAS_MEASUREMENT     2           ///< Used with CONFIG3.BIAS_MEAS for bias measurement. See ADS1299 datasheet, pp. 53-54.
#define CH_MVDD_SUPPLY          3           ///< Used for measuring analog and digital supplies. See ADS1299 datasheet, p. 17.
#define CH_TEMPERATURE_SENSOR   4           ///< Measures device temperature. See ADS1299 datasheet, p. 17.
#define CH_TEST_SIGNAL          5           ///< Measures calibration signal. See ADS1299 datasheet, pp. 17 and 41.
#define CH_BIAS_DRIVE_P         6           ///< Connects positive side of channel to bias drive output.
#define CH_BIAS_DRIVE_N         7           ///< Connects negative side of channel to bias drive output.
#define CHANNEL_ON             (0<<7)
#define CHANNEL_OFF            (1<<7)
#define CH_SRB2_DISCONNECTED   (0<<3)
#define CH_SRB2_CONNECTED      (1<<3)



//CHANNEL_OFF| ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE
 
#define PCKT_START	0xA0	// prefix for data packet error checking
#define PCKT_END	0xC0	// postfix for data packet error checking

// daisy module
#define CLK_EN	5




//LIS3DH --- Replaced with IMU MPU6050
/*
#define READ_REG		0x80
#define READ_MULTI		0x40

#define STATUS_REG_AUX	0x07	// axis over-run and data available flags (see 0x27)
#define OUT_ADC1_L		0x08	//
#define OUT_ADC1_H		0x09	//
#define OUT_ADC2_L		0x0A	//	ADC input values (check DS)
#define OUT_ADC2_H		0x0B	//
#define OUT_ADC3_L		0x0C	//
#define OUT_ADC3_H		0x0D	//
#define INT_COUNTER_REG	0x0E	// ??
#define WHO_AM_I		0x0F	// DEVICE ID 0x33
#define TMP_CFG_REG		0x1F	// ADC enable (0x80) Temperature sensor enable (0x40)
#define CTRL_REG1		0x20	// Data Rate Power Mode X enable Y enable Z enable (on >0x10)
#define CTRL_REG2		0x21	// High Pass Filter Stuph
#define CTRL_REG3		0x22	// INT1 select register
#define CTRL_REG4		0x23	// Block update timing endian G-force resolution self test SPI pins
#define CTRL_REG5		0x24	// reboot FIFO enable latch 4D detection
#define CTRL_REG6		0x25	// ??
#define REFERENCE		0x26	// interrupt reference
#define STATUS_REG2		0x27	// axis overrun and availale flags (see 0x07)
#define OUT_X_L			0x28	//
#define OUT_X_H			0x29	//
#define OUT_Y_L			0x2A	//	tripple axis values (see 0x0A)
#define OUT_Y_H			0x2B	//
#define OUT_Z_L			0x2C	//
#define OUT_Z_H			0x2D	//
#define FIFO_CTRL_REG	0x2E	// FIFO mode trigger output pin select (?)
#define FIFO_SRC_REG	0x2F	// ??
#define INT1_CFG		0x30	// 6 degree control register
#define INT1_SOURCE		0x31	// axis threshold interrupt control
#define INT1_THS		0x32	// INT1 threshold
#define INT1_DURATION	0x33	// INT1 duration
#define CLICK_CFG		0x38	// click on axis
#define CLICK_SRC		0x39	// other click
#define CLICK_THS		0x3A	// more click
#define TIME_LIMIT		0x3B	// click related
#define TIME_LATENCY	0x3C	// and so on
#define TIME_WINDOW		0x3D	// contined click

#define SCALE_2G		0x00  //(b00000000)	// +/- 2G sensitivity
#define SCALE_4G		0x10  //(b00010000)	// +/- 4G sensitivity
#define SCALE_8G		0x20  //(b00100000)	// +/- 8G sensitivity
#define SCALE_16G		0x30  //(b00110000)	// +/- 16G sensitivity
#define RATE_1HZ		0x10  //(b00010000)	// 1Hz sample rate in normal or low-power mode
#define RATE_10HZ		0x20  //(b00100000)	// 10Hz sample rate in normal or low-power mode
#define RATE_25HZ		0x30  //(b00110000)	// 25Hz sample rate in normal or low-power mode
#define RATE_50HZ		0x40  //(b01000000)	// 50Hz sample rate in normal or low-power mode
#define RATE_100HZ		0x50  //(b01010000)	// 100Hz sample rate in normal or low-power mode
#define RATE_200HZ		0x60  //(b01100000)	// 200Hz sample rate in normal or low-power mode
#define RATE_400HZ		0x70  //(b01110000)	// 400Hz sample rate in normal or low-power mode
#define RATE_1600HZ_LP	0x80  //(b10000000)	// 1600Hz sample rate in low-power mode
#define RATE_1250HZ_N	0x90  //(b10010000)	// 1250Hz sample rate in normal mode
#define RATE_5000HZ_LP	0x90  //(b10010000)	// 5000Hz sample rate in low-power mode
#define ACCEL_AXIS_X    0x07 // x axis
#define ACCEL_AXIS_Y    0x08 // y axis
#define ACCEL_AXIS_Z    0x09 // z axis
*/









// ================= OPENBCI_COMMANDS ==================================

#define STOP_STREAMING  0
#define START_STREAMING 1


// Baud rates
#define OPENBCI_BAUD_RATE 115200
#define OPENBCI_BAUD_RATE_MIN_NO_AVG 200000

// File transmissions
#define OPENBCI_BOP 'A' // Begining of stream packet

/** Turning channels off */
#define OPENBCI_CHANNEL_OFF_1 '1'
#define OPENBCI_CHANNEL_OFF_2 '2'
#define OPENBCI_CHANNEL_OFF_3 '3'
#define OPENBCI_CHANNEL_OFF_4 '4'
#define OPENBCI_CHANNEL_OFF_5 '5'
#define OPENBCI_CHANNEL_OFF_6 '6'
#define OPENBCI_CHANNEL_OFF_7 '7'
#define OPENBCI_CHANNEL_OFF_8 '8'
#define OPENBCI_CHANNEL_OFF_9 'q'
#define OPENBCI_CHANNEL_OFF_10 'w'
#define OPENBCI_CHANNEL_OFF_11 'e'
#define OPENBCI_CHANNEL_OFF_12 'r'
#define OPENBCI_CHANNEL_OFF_13 't'
#define OPENBCI_CHANNEL_OFF_14 'y'
#define OPENBCI_CHANNEL_OFF_15 'u'
#define OPENBCI_CHANNEL_OFF_16 'i'

/** Turn channels on */
#define OPENBCI_CHANNEL_ON_1 '!'
#define OPENBCI_CHANNEL_ON_2 '@'
#define OPENBCI_CHANNEL_ON_3 '#'
#define OPENBCI_CHANNEL_ON_4 '$'
#define OPENBCI_CHANNEL_ON_5 '%'
#define OPENBCI_CHANNEL_ON_6 '^'
#define OPENBCI_CHANNEL_ON_7 '&'
#define OPENBCI_CHANNEL_ON_8 '*'
#define OPENBCI_CHANNEL_ON_9 'Q'
#define OPENBCI_CHANNEL_ON_10 'W'
#define OPENBCI_CHANNEL_ON_11 'E'
#define OPENBCI_CHANNEL_ON_12 'R'
#define OPENBCI_CHANNEL_ON_13 'T'
#define OPENBCI_CHANNEL_ON_14 'Y'
#define OPENBCI_CHANNEL_ON_15 'U'
#define OPENBCI_CHANNEL_ON_16 'I'

/** Test Signal Control Commands
 * 1x - Voltage will be 1 * (VREFP - VREFN) / 2.4 mV
 * 2x - Voltage will be 2 * (VREFP - VREFN) / 2.4 mV
 */
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_DC            'p'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND        '0'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST '='
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW '-'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST ']'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW '['

/** Channel Setting Commands */
#define OPENBCI_CHANNEL_CMD_ADC_Normal      '0'
#define OPENBCI_CHANNEL_CMD_ADC_Shorted     '1'
#define OPENBCI_CHANNEL_CMD_ADC_BiasDRP     '6'
#define OPENBCI_CHANNEL_CMD_ADC_BiasDRN     '7'
#define OPENBCI_CHANNEL_CMD_ADC_BiasMethod  '2'
#define OPENBCI_CHANNEL_CMD_ADC_MVDD        '3'
#define OPENBCI_CHANNEL_CMD_ADC_Temp        '4'
#define OPENBCI_CHANNEL_CMD_ADC_TestSig     '5'
#define OPENBCI_CHANNEL_CMD_BIAS_INCLUDE    '1'
#define OPENBCI_CHANNEL_CMD_BIAS_REMOVE     '0'
#define OPENBCI_CHANNEL_CMD_CHANNEL_1       '1'
#define OPENBCI_CHANNEL_CMD_CHANNEL_2       '2'
#define OPENBCI_CHANNEL_CMD_CHANNEL_3       '3'
#define OPENBCI_CHANNEL_CMD_CHANNEL_4       '4'
#define OPENBCI_CHANNEL_CMD_CHANNEL_5       '5'
#define OPENBCI_CHANNEL_CMD_CHANNEL_6       '6'
#define OPENBCI_CHANNEL_CMD_CHANNEL_7       '7'
#define OPENBCI_CHANNEL_CMD_CHANNEL_8       '8'
#define OPENBCI_CHANNEL_CMD_CHANNEL_9       'Q'
#define OPENBCI_CHANNEL_CMD_CHANNEL_10      'W'
#define OPENBCI_CHANNEL_CMD_CHANNEL_11      'E'
#define OPENBCI_CHANNEL_CMD_CHANNEL_12      'R'
#define OPENBCI_CHANNEL_CMD_CHANNEL_13      'T'
#define OPENBCI_CHANNEL_CMD_CHANNEL_14      'Y'
#define OPENBCI_CHANNEL_CMD_CHANNEL_15      'U'
#define OPENBCI_CHANNEL_CMD_CHANNEL_16      'I'
#define OPENBCI_CHANNEL_CMD_GAIN_1          '0'
#define OPENBCI_CHANNEL_CMD_GAIN_2          '1'
#define OPENBCI_CHANNEL_CMD_GAIN_4          '2'
#define OPENBCI_CHANNEL_CMD_GAIN_6          '3'
#define OPENBCI_CHANNEL_CMD_GAIN_8          '4'
#define OPENBCI_CHANNEL_CMD_GAIN_12         '5'
#define OPENBCI_CHANNEL_CMD_GAIN_24         '6'
#define OPENBCI_CHANNEL_CMD_LATCH           'X'
#define OPENBCI_CHANNEL_CMD_POWER_OFF       '1'
#define OPENBCI_CHANNEL_CMD_POWER_ON        '0'
#define OPENBCI_CHANNEL_CMD_SET             'x'
#define OPENBCI_CHANNEL_CMD_SRB1_CONNECT    '1'
#define OPENBCI_CHANNEL_CMD_SRB1_DISCONNECT '0'
#define OPENBCI_CHANNEL_CMD_SRB2_CONNECT    '1'
#define OPENBCI_CHANNEL_CMD_SRB2_DISCONNECT '0'

/** Default Channel Settings */
#define OPENBCI_CHANNEL_DEFAULT_ALL_SET 	'd'
#define OPENBCI_CHANNEL_DEFAULT_ALL_REPORT 	'D'

/** LeadOff Impedance Commands */
#define OPENBCI_CHANNEL_IMPEDANCE_LATCH               		'Z'
#define OPENBCI_CHANNEL_IMPEDANCE_SET                 		'z'
#define OPENBCI_CHANNEL_IMPEDANCE_TEST_SIGNAL_APPLIED  		'1'
#define OPENBCI_CHANNEL_IMPEDANCE_TEST_SIGNAL_APPLIED_NOT 	'0'

/** SD card Commands - reserver for cyton */
/*
#define OPENBCI_SD_LOG_FOR_HOUR_1    'G'
#define OPENBCI_SD_LOG_FOR_HOUR_2    'H'
#define OPENBCI_SD_LOG_FOR_HOUR_4    'J'
#define OPENBCI_SD_LOG_FOR_HOUR_12   'K'
#define OPENBCI_SD_LOG_FOR_HOUR_24   'L'
#define OPENBCI_SD_LOG_FOR_MIN_5     'A'
#define OPENBCI_SD_LOG_FOR_MIN_15    'S'
#define OPENBCI_SD_LOG_FOR_MIN_30    'F'
#define OPENBCI_SD_LOG_FOR_SEC_14    'a'
#define OPENBCI_SD_LOG_STOP			 'j'
*/


/** Stream Data Commands */
#define OPENBCI_STREAM_START  'b'
#define OPENBCI_STREAM_STOP   's'

/** Miscellaneous */
#define OPENBCI_MISC_QUERY_REGISTER_SETTINGS '?'
#define OPENBCI_MISC_SOFT_RESET              'v'

/** 16 Channel Commands */
#define OPENBCI_CHANNEL_MAX_NUMBER_8    'c'
#define OPENBCI_CHANNEL_MAX_NUMBER_16   'C'
#define OPENBCI_BOARD_MODE_SET 			'/'
#define OPENBCI_GET_VERSION 			'V'

/** Set sample rate */
#define OPENBCI_SAMPLE_RATE_SET '~'

/** Insert marker into the stream */
#define OPENBCI_INSERT_MARKER '`'

/** Sync Clocks */
#define OPENBCI_TIME_SET 	'<'
#define OPENBCI_TIME_STOP 	'>'


/** Wifi Stuff reserved for Cyton */
#define OPENBCI_WIFI_ATTACH '{'
#define OPENBCI_WIFI_REMOVE '}'
#define OPENBCI_WIFI_STATUS ':'
#define OPENBCI_WIFI_RESET 	';'



/** Possible number of channels */
#define OPENBCI_NUMBER_OF_CHANNELS_DAISY 16
#define OPENBCI_NUMBER_OF_CHANNELS_DEFAULT 8

/** Helpful numbers */
#define OPENBCI_NUMBER_OF_BOARD_SETTINGS 	1
#define OPENBCI_NUMBER_OF_CHANNEL_SETTINGS 	6
#define OPENBCI_NUMBER_OF_LEAD_OFF_SETTINGS 2


/** Possible Sample Rates*/
#define OPENBCI_SAMPLE_RATE_125 125
#define OPENBCI_SAMPLE_RATE_250 250


/** Time out for multi char commands **/
#define MULTI_CHAR_COMMAND_TIMEOUT_MS 1000

/** Packet Size */
#define OPENBCI_PACKET_SIZE 					33
#define OPENBCI_NUMBER_BYTES_PER_ADS_SAMPLE 	24
#define OPENBCI_NUMBER_CHANNELS_PER_ADS_SAMPLE 	24

/** Impedance Calculation Variables */
#define OPENBCI_LEAD_OFF_DRIVE_AMPS 0.000000006
#define OPENBCI_LEAD_OFF_FREQUENCY_HZ 31
#define OPENBCI_TIME_OUT_MS_1 1
#define OPENBCI_TIME_OUT_MS_3 3
#define OPENBCI_NUMBER_OF_BYTES_SETTINGS_CHANNEL 	9
#define OPENBCI_NUMBER_OF_BYTES_SETTINGS_LEAD_OFF 	5
#define OPENBCI_NUMBER_OF_BYTES_AUX 6
#define OPENBCI_FIRMWARE_VERSION_V1 1
#define OPENBCI_FIRMWARE_VERSION_V2 1
#define OPENBCI_ADS_BYTES_PER_CHAN 	3
#define OPENBCI_ADS_CHANS_PER_BOARD 8

/** UDP Packet Information */
#define UDP_BYTES_PER_PACKET 	20
#define UDP_BYTES_PER_SAMPLE 	6
#define UDP_SAMPLES_PER_PACKET 	3
#define UDP_TOTAL_DATA_BYTES 	18
#define UDP_RING_BUFFER_SIZE 	50

#endif
