/*
 * ADS Registers Handler
 * 
 * Config Options 
 */
#ifndef _____ADS1299_lib_h
#define _____ADS1299_lib_h

#include "Arduino.h"
#include "OpenBCI_NovaXR_Definitions.h"

// -- for main board ------------
extern const uint8_t MainCS;   // Chip Select pin  for main board
extern const uint8_t MainDRDY; // Data Ready pin for main board  - Hardware Interrupt Ready

// -- for sister board ----------
extern const uint8_t SisCS;   // Chip Select pin  for sister board
extern const uint8_t SisDRDY; // Data Ready pin for sister board 

  
// ---- SPI ----
#define SPI_CLK       20000000 // SPI Frequency 20MHz

#define SISTER_BOARD  1        // Set CS and DRDY pin for main board
#define MAIN_BOARD    2        // Set CS and DRDY pin for sister board


#define WAKEUP  0x02 // Wake-up from standby mode
#define STANDBY 0x04 // Enter Standby mode
#define RESET   0x06 // Reset the device registers to default
#define START   0x08 // Start and restart (synchronize) conversions
#define STOP    0x0A // Stop conversion
#define RDATAC  0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define SDATAC  0x11 // Stop Read Data Continuous mode
#define RDATA   0x12 // Read data by command supports multiple read back
#define RREG    0x20 // Command that Reads a specifeid ADS Register 
#define WREG    0x40 // Command that Writes a specifeid ADS Register


// ==== ADS1299 Register Addresses Table =================== 
#define ADS_ID  0x3E  // product ID for ADS1299
#define ID_REG  0x00  // this register contains ADS_ID
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF    0x04
#define CH1SET  0x05
#define CH2SET  0x06
#define CH3SET  0x07
#define CH4SET  0x08
#define CH5SET  0x09
#define CH6SET  0x0A
#define CH7SET  0x0B
#define CH8SET  0x0C
#define CHn     0xFF
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP  0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define GPIO    0x14
#define MISC1   0x15
#define MISC2   0x16
#define CONFIG4 0x17



//  ==== Rgister CONFIG1 Options ========
#define ADS1299_CONFIG1_DAISY     (0b10110000)  // Daisy-Chain Mode + CLK output enabled + CLKSEL = Vdd 
#define ADS1299_CONFIG1_DAISY_NOT (0b11010000)  // Multiple Readback Mode + CLK output disabled + CLKSEL = Vdd 

 typedef enum SAMPLE_RATE {
    SAMPLE_RATE_16000,
    SAMPLE_RATE_8000,
    SAMPLE_RATE_4000,
    SAMPLE_RATE_2000,
    SAMPLE_RATE_1000,
    SAMPLE_RATE_500,
    SAMPLE_RATE_250
  };


/*
 * 
11110000 - F0 - 16Κ
11110001 - F1 - 8Κ
11110010 - F2 - 4Κ
11110011 - F3 - 2Κ
11110100 - F4 - 1Κ
11110101 - F5 - 500
11110110 - F6 - 250
11110111 - F7 - don’t use it
 */


//  ==== Rgister CONFIG2 Options =================

#define ADS1299_CONFIG1_TEST_AMP1_INT (0b11010000) // internal test signal 1x amplified
#define ADS1299_CONFIG1_TEST_AMP2_INT (0b11010100) // internal test signal 2x amplified
#define ADS1299_CONFIG1_TEST_AMP1_EXT (0b11000000) // external test signal 1x amplified
#define ADS1299_CONFIG1_TEST_AMP2_EXT (0b11000100) // external test signal 2x amplified

 typedef enum TEST_SIGNAL_FREQ {  // datasheet page 47
    PULSED21,
    PULSED20,
    DONT_USE,
    AT_DC,
  };



//  ==== Rgister CONFIG3 Options =================
#define PD_REFBUF_OFF (0b01100000)
#define PD_REFBUF_ON  (0b11100000)

#define BIAS_MEAS_OPEN (0b01100000)  // Open
#define BIAS_MEAS_CONN (0b01110000)  //  BIAS_IN signal is routed to the channel that has the MUX_Setting 010 (VREF)

#define BIASREF_INT_FED_EXT  (0b01100000) 
#define BIASREF_INT_FED_INT  (0b01101000) 

#define PD_BIAS_OFF (0b01100000)
#define PD_BIAS_ON  (0b01100100)

#define BIAS_LOFF_SENS_OFF (0b01100000)
#define BIAS_LOFF_SENS_ON  (0b01100010)



// ======= LOFF REGISTER ========================= 
//Lead-off signal choices
#define LOFF_MAG_6NA        (0b00000000)
#define LOFF_MAG_24NA       (0b00000100)
#define LOFF_MAG_6UA        (0b00001000)
#define LOFF_MAG_24UA       (0b00001100)
#define LOFF_FREQ_DC        (0b00000000)
#define LOFF_FREQ_7p8HZ     (0b00000001)
#define LOFF_FREQ_31p2HZ    (0b00000010)
#define LOFF_FREQ_FS_4      (0b00000011)
#define PCHAN (0)
#define NCHAN (1)
#define OFF (0)
#define ON (1)


// ======== ChnSET Options =====================
//gainCode choices
#define ADS_GAIN01 (0b00000000)  // 0x00
#define ADS_GAIN02 (0b00010000) // 0x10
#define ADS_GAIN04 (0b00100000) // 0x20
#define ADS_GAIN06 (0b00110000) // 0x30
#define ADS_GAIN08 (0b01000000) // 0x40
#define ADS_GAIN12 (0b01010000) // 0x50
#define ADS_GAIN24 (0b01100000) // 0x60

// ========= CHANNEL SETTINGS ==================
#define POWER_DOWN      (0)
#define GAIN_SET        (1)
#define INPUT_TYPE_SET  (2)
#define BIAS_SET        (3)
#define SRB2_SET        (4)
#define SRB1_SET        (5)
#define YES          (0x01)
#define NO           (0x00)

// ======= Input Type choices ==================
#define ADSINPUT_NORMAL     (0b00000000)
#define ADSINPUT_SHORTED    (0b00000001)
#define ADSINPUT_BIAS_MEAS  (0b00000010)
#define ADSINPUT_MVDD       (0b00000011)
#define ADSINPUT_TEMP       (0b00000100)
#define ADSINPUT_TESTSIG    (0b00000101)
#define ADSINPUT_BIAS_DRP   (0b00000110)
#define ADSINPUT_BIAL_DRN   (0b00000111)

// ====== MISC1 Options =========================
#define SRB1_OPEN (0b00000000)
#define SRB1_CLOS (0b00100000) 

// ====== CONFIG4 Options =========================
#define SINGLE_SHOT_DIS  (0b00000000) // Continuous Conversion
#define SINGLE_SHOT_EN   (0b00001000) // Single Shot Conversion 
#define PD_LOFF_COMP_DIS (0b00000000) // Lead-Off Comparators Disabled
#define PD_LOFF_COMP_EN  (0b00000010) // Lead-Off Comparators Enabled


#pragma pack (push, 1)
union adc_data{  
    byte rawData[27];
    struct ADS{  
        byte StatReg[3];
        byte DataCH0[3];
        byte DataCH1[3];
        byte DataCH2[3];
        byte DataCH3[3];
        byte DataCH4[3];      
        byte DataCH5[3];
        byte DataCH6[3];    
        byte DataCH7[3];    
    }ADS_paylaod; 
};
#pragma pack(pop)


// +++++++++++++++++++ PROTOTYPES +++++++++++++++++++++
void pineriphernal_init(void);
void Reset_ADC(void);
byte readRegisterSettings(byte RegAddr);
void Begin_ADS(void);
void SPI_pinPeriphernal_init(void);
void display_Registers_settings(void);
void config_ADS_Register(byte R_Address, byte data);
void config_All_ADS_Inputs(byte data);
void config_ADS_Input(uint8_t channel, byte data);
void select_ADC( uint8_t adc_id);
void start_ADC_Continuous_Mode(void);
void get_ADC1_Data_CM(void);
void get_ADC2_Data_CM(void);
void get_ADC_Data_SS(adc_data * adc);
void disable_ContninuousConversion(void);
void ADS_Stop_Conversion(void);
void stop_ADC_interrupt(void);
void fill_PktBuff(adc_data * adc, uint8_t sample_id);
void do_WREG (byte Register,byte Data);
void do_RREG ( byte register_Address, byte returned_bytes);
byte apply_bit_Config(byte mask, byte target_Reg);
byte setBit1(uint8_t index, byte Register);
byte setBit0(uint8_t index, byte Register);
byte flipBit(uint8_t index, byte Register);
bool ADS_connected(void);
void print_boardID(uint8_t id);
void ADS_Init_Config(uint8_t ads_id);



// ---------- Evaluation Test Examples -------------
void ADS_Example_1_Interrupt(uint8_t ads_id, long duration);
void ADS_Example_2_Single_Shot(void );
bool print_Single_Shot(void);
void stop_and_configInput(uint8_t channel, byte data);



#endif 
