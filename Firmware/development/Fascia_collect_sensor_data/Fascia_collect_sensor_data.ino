// library includes
#include "SAMD_AnalogCorrection.h"
#include <SPI.h>
#include "wiring_private.h"
//#include "I2Cdev.h"//
#include "MPU6050.h"
#include "Wire.h"//
#include "MAX30105.h"
// header files
#include "Pin_Table_Defs.h"
#include "WiFi_Settings.h"
#include "ads1299.h"

// define enum for boards and data retrieval types
enum board_types {NOVA_XR_V1, NOVA_XR_V2_SISTER, NOVA_XR_V2_MAIN, FASCIA_V0_0, FASCIA_V0_1};
enum data_mode_t {RDATA_CC_MODE, RDATA_SS_MODE};
enum run_mode_t  {GEN_TEST_SIGNAL, NORMAL_ELECTRODES};

// settings
#define CONNECT_WIFI 1

#define BOARD_V FASCIA_V0_1
#define DATA_MODE RDATA_SS_MODE
#define RUN_MODE NORMAL_ELECTRODES
// v for verbose: lots of prints
#define v 0
// debug: serial reads and writes
#define debug 0
// REMEMBER: comment out lines in setup() and loop() for the sensors you do not have.

// Setup for SPI communications
SPIClass mySPI (&sercom1, PA19, PA17, PA16, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
const int SPI_CLK = 4*pow(10,6) ; //4MHz
// Setup for I2C communications
// PA08: SDA
// PA09: SCL
// MPU6050 I2C address: 0x110100X where X is the logic level in pin AD0
// and last bit is r/w
#define MPU_ADDR 0b11010000

// define pins depending on boards
int pLED;
int pBAT;
// ADS1299 ADC pins
const int pRESET = PA23;    // reset pin
int pCS;              // chip select pin
int pDRDY;            // data ready pin
// EDA
const int pEDA = PA02;
// MPU6050 IMU pins
MPU6050 accelgyro;
const int pMPUint = PB03;
// PPG & temperature pins
MAX30105 particleSensor;

// define variables
int print_ch = -1;
bool LEDval = LOW;

int cnt = 0;

// ADS channel gains 
byte ADS_CHANNEL_GAINS[8] = {/*chan 1 EMG Gain 4  */ ADS1299_REG_CHNSET_GAIN_4,
                             /*chan 2 EMG Gain 4  */ ADS1299_REG_CHNSET_GAIN_4,
                             /*chan 3 EOG Gain 2  */ ADS1299_REG_CHNSET_GAIN_4,
                             /*chan 4 EMG Gain 4  */ ADS1299_REG_CHNSET_GAIN_4,
                             /*chan 5 EEG Gain 12 */ ADS1299_REG_CHNSET_GAIN_12, // Passive Electrode
                             /*chan 6 EEG Gain 12 */ ADS1299_REG_CHNSET_GAIN_12, // Passive Electrode
                             /*chan 7 EEG Gain 1  */ ADS1299_REG_CHNSET_GAIN_1,  // Active  Electrode
                             /*chan 8 EEG Gain 1  */ ADS1299_REG_CHNSET_GAIN_1   // Active  Electrode
                             };
                     
void select_board_pins(void) {
    switch (BOARD_V){
    case NOVA_XR_V1: {
        Serial.println("initializing pins for Nova XR V1");
        pCS = PA21;
        pDRDY = PA10;
        break;
    }
    case NOVA_XR_V2_MAIN: {
        Serial.println("initializing pins for Nova XR V2 main board");
        pCS = PA21;
        pDRDY = PB10;
        // disable other ADS
        pinMode(PA20, OUTPUT);
        digitalWrite(PA20, HIGH);
        break;
    }
    case NOVA_XR_V2_SISTER: {
      Serial.println("initializing pins for Nova XR V2 sister board");
      pCS = PA20;
      pDRDY = PB02;
      // disable other ADS
      pinMode(PA21, OUTPUT);
      digitalWrite(PA21, HIGH);
      break;
    }
    case FASCIA_V0_0: {
        Serial.println("initializing pins for Fascia V0.0");
        pCS = PA20;
        pDRDY = PA07;
        pLED = PA04;
        pBAT = PA05;
        break;
    }
    case FASCIA_V0_1: {
        Serial.println("initializing pins for Fascia V0.1");
        pCS = PA20;
        pDRDY = PB10;
        pLED = PA04;
        break;
    }
  }
}

void initialize_pin_modes(void) {
  pinMode(pCS, OUTPUT);
  pinMode(pDRDY, INPUT);
  pinMode(pRESET, OUTPUT);
  pinMode(pEDA, INPUT);
  pinMode(pLED, OUTPUT);
  pinMode(pBAT, INPUT);
  pinPeripheral(PA19, PIO_SERCOM);
  pinPeripheral(PA17, PIO_SERCOM);
  pinPeripheral(PA16, PIO_SERCOM);
  if (DATA_MODE == RDATA_CC_MODE) {
    // attachInterrupt(pDRDY, DRDY_ISR, FALLING); // TODO figure out interrupt with input. malloc maybe?
  }
  // attachInterrupt(pMPUint, get_gyro_data, MODEDEODEOD);
}

void ADS_connect(void) {
  //enable ADS
  digitalWrite(pRESET, HIGH); 
  digitalWrite(pCS, HIGH);
  delay(500);
  // select and reset ADS
  digitalWrite(pCS, LOW);
    mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
    mySPI.transfer(RESET);
    mySPI.endTransaction();
    delay(100);
  digitalWrite(pCS, HIGH);
  delay(50);
  // read and print ADS device ID to ensure connection
  byte idval = ADS_RREG(0x0,1);
  Serial.print("connected to ADS device id "); Serial.println(idval,BIN);
  if (idval != 0x3E) {
    Serial.println("ADS device not properly connected");
    Serial.println("If the ADS ID number is 0xFF, you likely have an issue with power");
    while(1) {    Serial.println("ADS device not properly connected");}
  }
}

void ADS_init(void) {
  // register map on page 44 of the data sheet; 
  // pages expading on register descriptions follow in the next pages
  byte config2_data = ADS1299_REG_CONFIG2_RESERVED_VALUE | //0b11000000;
                      ADS1299_REG_CONFIG2_CAL_PULSE_FCLK_DIV_2_21;
  byte config3_data = ADS1299_REG_CONFIG3_REFBUF_ENABLED | //0b11101100;
                      ADS1299_REG_CONFIG3_RESERVED_VALUE |
                      ADS1299_REG_CONFIG3_BIAS_MEAS_DISABLED |
                      ADS1299_REG_CONFIG3_BIASREF_INT |
                      ADS1299_REG_CONFIG3_BIASBUF_ENABLED |
                      ADS1299_REG_CONFIG3_BIAS_LOFF_SENSE_DISABLED;
  byte channel_mode = ADS1299_REG_CHNSET_NORMAL_ELECTRODE;

  if (RUN_MODE == GEN_TEST_SIGNAL) {
    config2_data = ADS1299_REG_CONFIG2_RESERVED_VALUE | //0b11010000;
                   ADS1299_REG_CONFIG2_CAL_INT |
                   ADS1299_REG_CONFIG2_CAL_AMP_2VREF_DIV_2_4_MV |
                   ADS1299_REG_CONFIG2_CAL_PULSE_FCLK_DIV_2_21;
    config3_data = ADS1299_REG_CONFIG3_REFBUF_ENABLED | //0b11100000;
                   ADS1299_REG_CONFIG3_RESERVED_VALUE |
                   ADS1299_REG_CONFIG3_BIAS_MEAS_DISABLED |
                   ADS1299_REG_CONFIG3_BIASREF_EXT |
                   ADS1299_REG_CONFIG3_BIASBUF_DISABLED |
                   ADS1299_REG_CONFIG3_BIAS_LOFF_SENSE_DISABLED;
    channel_mode = ADS1299_REG_CHNSET_TEST_SIGNAL;
  }

  ADS_WREG(ADS1299_REGADDR_CONFIG1, ADS1299_REG_CONFIG1_RESERVED_VALUE |
                                    ADS1299_REG_CONFIG1_1kSPS); // last three bits is the data rate page 46 of data sheet
  ADS_WREG(ADS1299_REGADDR_CONFIG2, config2_data);
  ADS_WREG(ADS1299_REGADDR_CONFIG3, config3_data);
  ADS_WREG(ADS1299_REGADDR_CONFIG4, 0x00);//0b00001000);
  ADS_WREG(ADS1299_REGADDR_GPIO, 0x00);
  ADS_WREG(ADS1299_REGADDR_MISC1, 0x00);

  delay(10);
  ADS_WREG(ADS1299_REGADDR_CH1SET, ADS_CHANNEL_GAINS[0] | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH2SET, ADS_CHANNEL_GAINS[1] | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH3SET, ADS_CHANNEL_GAINS[2] | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH4SET, ADS_CHANNEL_GAINS[3] | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH5SET, ADS_CHANNEL_GAINS[4] | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_CONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH6SET, ADS_CHANNEL_GAINS[5] | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_CONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH7SET, ADS_CHANNEL_GAINS[6] | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_CONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH8SET, ADS_CHANNEL_GAINS[7] | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_CONNECTED);

 // turn on bias for all EEG Channels (5-8)
 ADS_WREG(ADS1299_REGADDR_BIAS_SENSN,  ADS1299_REG_BIAS_SENSN_BIASN8 |
                                       ADS1299_REG_BIAS_SENSN_BIASN7 |
                                       ADS1299_REG_BIAS_SENSN_BIASN6 |
                                       ADS1299_REG_BIAS_SENSN_BIASN5 );

  // set up Lead-Off detection
  /* from page 63 in the manual:
   *   10.1.2.1 Lead-Off
   *   Sample code to set dc lead-off with pull-up and pull-down resistors on all channels.
   *     WREG LOFF       0x13  // Comparator threshold at 95% and 5%, pullup or pulldown resistor dc lead-off
   *     WREG CONFIG4    0x02  // Turn on dc lead-off comparators
   *     WREG LOFF_SENSP 0xFF  // Turn on the P-side of all channels for lead-off sensing
   *     WREG LOFF_SENSN 0xFF  // Turn on the N-side of all channels for lead-off sensing
   *   Observe the status bits of the output data stream to monitor lead-off status.
  */
//  ADS_WREG(ADS1299_REGADDR_LOFF,       0x13); 
//  ADS_WREG(ADS1299_REGADDR_CONFIG4,    0x02); 
//  ADS_WREG(ADS1299_REGADDR_LOFF_SENSP, 0x0F); 
//  ADS_WREG(ADS1299_REGADDR_LOFF_SENSN, 0xFF); 
//  // TODO make sure this below works
//  ADS_WREG(ADS1299_REGADDR_LOFF_FLIP,  0xF0); // flip the EEG channels since we are connecting them to the N end 
}

void ADS_start(void) {
  digitalWrite(pCS, LOW);
  mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
  mySPI.transfer(START);
  if (DATA_MODE == RDATA_SS_MODE) {
    mySPI.transfer(RDATA);
  } else if (DATA_MODE == RDATA_CC_MODE) {
    mySPI.transfer(RDATAC);
  }
}

void Arduino_ADC_setup() {
  //https://forum.arduino.cc/index.php?topic=443173.0
  //http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html
  //https://forum.arduino.cc/index.php?topic=6549.0

    // original SAMD bootloader code set to:
    //  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |    // Divide Clock by 512.
    //                 ADC_CTRLB_RESSEL_10BIT;         // 10 bits resolution as default
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV16 | ADC_CTRLB_RESSEL_12BIT;
    Serial.println("done setting up ADC with lower prescaler value and higher bit resolution");
}

void setup_MAX30105() {
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {//Use default I2C port, 400kHz speed
    while (1){Serial.println("MAX30105 was not found. Please check wiring/power. ");}
  }
  //The LEDs are very low power and won't affect the temp reading much but
  //you may want to turn off the LEDs to avoid any local heating
  // can try setting data output rate (currently (default) close to the slowest)
  particleSensor.setup(/*byte powerLevel = */0x1F, /*byte sampleAverage = */4, /*byte ledMode =*/ 3, /*int sampleRate =*/3200); //Configure sensor. Turn off LEDs
  // TODO increase sampling rate here!!!!
  
  //particleSensor.setup(); //Configure sensor. Use 25mA for LED drive
  //TODO seems like line below is not actually necessary
  particleSensor.enableDIETEMPRDY(); //Enable the temp ready interrupt. This is required.
  // particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  Serial.println("done setting up MAX30105");
}

void setup_MPU6050() {
  accelgyro.initialize();
  // Test connection
  if (!accelgyro.testConnection()) {
    while (1){Serial.println("Failed to connect to MPU6050");}
  }
  
  // use the code below to print before/after and change accel/gyro offset values
  /*
  Serial.println("Updating internal sensor offsets...");
  accelgyro.setXGyroOffset(220);
  accelgyro.setYGyroOffset(76);
  accelgyro.setZGyroOffset(-85);
  */
  
  /* 
  // data rate change?
  // SMPLRT_DIV register
  uint8_t getRate();
  void setRate(uint8_t rate);*/

  /*    
    // Calibration Routines
    void CalibrateGyro(uint8_t Loops = 15); // Fine tune after setting offsets with less Loops.
    void CalibrateAccel(uint8_t Loops = 15);// Fine tune after setting offsets with less Loops.
    */
    Serial.println("Done setting up MPU6050");
}

void setup() {
  delay(1000);
  // initialize communications: spi, I2C, serial, and wifi if applicable
  mySPI.begin();
  Wire.begin();
  Serial.begin(115200);
  // set up indicator LED
  select_board_pins();
  initialize_pin_modes();
  digitalWrite(pLED, HIGH);

  #if (CONNECT_WIFI)
    setupWifi();
  #endif

  // initialize board and settings
  // make sure settings are compatible
  if (BOARD_V == FASCIA_V0_0 && DATA_MODE == RDATA_CC_MODE) {
    Serial.println("You cannot use Continuous Conversion mode on Fascia Version 0.0");
    Serial.println("The DRDY pin on this PCB (PA07) cannot be configured as an interrupt pin");
    while(1);
  }

  // speed up analog read speed
  Arduino_ADC_setup();
  
  // initialize MAX30105 PPG sensor (we will also be getting temperature data from it)
  setup_MAX30105();
  
  // initialize the IMU MPU6050
  setup_MPU6050();
  
  // initialize ads1299
  ADS_connect();
  ADS_init();
  ADS_start();
  Serial.println("Done with setup.");
  digitalWrite(pLED, LOW);
  #if debug
    print_serial_instructions();
  #endif
}

void print_serial_instructions() {
  Serial.println("Type the channel number to print that channel's data [1-8] (and plot, if you switch to Serial Plotter)");
  Serial.println("Or type '0' to stop printing the data.");
  Serial.println("type BN#0 to deactivate biasN for channel # and BN#1 to activate it");
  Serial.println("type BP#0 to deactivate biasP for channel # and BP#1 to activate it");
  Serial.println("type S#0 to deactivate SRB2 for channel # and B#1 to activate it");
  Serial.println("type G#N to set the gain for channel # to N=0:1, N=1:2, N=2:4, N=3:6, N=4:8, N=5:12, N=6:24");
  Serial.println("type T#0 to toggle channel # off, and T#1 to toggle channel # on");
  Serial.println("type 'R' or 'r' to print the current register settings of the ADS1299");
  Serial.println("type 'P' or 'p' to print these instructions again");
}

void loop() {
  #if debug
    if(Serial.available()>1){ 
      parse_serial_input();
    }
  #endif
  
  long packet[NUM_ELEMENTS];
  packet[i_VALID] = 0;
  
  #if v
    Serial.println("packet #"+String(cnt));
  #endif
  
  #if DATA_MODE == RDATA_SS_MODE
    DRDY_ISR(packet);
  #endif
  if (!(cnt%10)) {
    get_PPG_temp_data(packet);
    get_EDA_data(packet);
    get_IMU_data(packet);
  } else {
    packet[i_VALID] |= (1<<v_PPG);
    packet[i_VALID] |= (1<<v_TEM);
    packet[i_VALID] |= (1<<v_EDA);
    packet[i_VALID] |= (0b111111<<v_IMU);
  }

  #if CONNECT_WIFI
    pushToBuf((char *)packet);
    sendWiFiDataPacket();
  #endif
}

void DRDY_ISR(long* packet) {
  //get all data before sign extending etc
  digitalWrite(pCS, LOW);
  if (DATA_MODE == RDATA_SS_MODE) {
    while(digitalRead(pDRDY));
    // Serial.println("DRDY just went low!");
    mySPI.transfer(START);
    mySPI.transfer(RDATA);
  }

  // first, read status bytes
  byte b1 = mySPI.transfer(0x00);
  byte b2 = mySPI.transfer(0x00);
  byte b3 = mySPI.transfer(0x00);
//  Serial.println(b1,BIN);
//  Serial.println(b2,BIN);
//  Serial.println(b3,BIN);
  // figure out if the data is valid (if so, which)
  if ((b1 & 0xF0) == 0b11000000) {
    byte loff_p =  (b1<<4) | (b2>>4);
    byte loff_n =  (b2<<4) | (b3>>4);
//    Serial.print("loff_p: ");    Serial.println(loff_p,BIN);
//    Serial.print("loff_n: ");    Serial.println(loff_n,BIN);
    // TODO use loff-p and loff-n to figure out which channels might be invalid
    // the first 4 channels are EMG and EOG- they use both leads
    int i = 0;
    for (i; i < 4; i++) {
//      packet[i_VALID] |= (((loff_p | loff_n) >> i) & 1) << (v_ADS + i);
    }
//    Serial.print("packet[i_VALID]: ");    Serial.println(packet[i_VALID],BIN);
    // the last 4 channels are all EEG, they use only the N inputs
    for (i; i < 8; i++) {
    }
  } else {
    Serial.println("invalid ADS1299 packet");
    for (int i = 3; i < 27; i++){
      mySPI.transfer(0x00);
      packet[i_VALID] |= 1 << (i_ADS + (i/3-1));
    }
    return;
  }

  // read channel data and sign extend it if valid
  byte temp[4] = {0,0,0,0};
  for (int i = 3; i < 27; i++){
    temp[2-((i+3)%3)] = mySPI.transfer(0x00);//DOUT[i];
    if ((i+3)%3 == 2) {
      int32_t d = *((int32_t*)temp);
      int ed = SIGN_EXT_24(d);//SIGNEXTEND(d);
      float cd = convert_ADC_volts(ed, ADS_GAINS[((ADS_CHANNEL_GAINS[i/3-1])>>4)]);
//      Serial.println(String(i/3-1)+": "+String(ADS_GAINS[((ADS_CHANNEL_GAINS[i/3-1])>>4)]));
      packet[i_ADS + (i/3-1)] = *((long*)&cd);//ed;
      #if v 
        Serial.print("ADS ");Serial.print(i/3);Serial.print(" ");Serial.println(ed); 
//        Serial.print("ADS ");Serial.print(i/3);Serial.print(" ");Serial.println(packet[i_ADS + i/3]); 
      #endif
      if (i/3 == (print_ch)) {Serial.println(ed);}
    }
  }
}

float convert_ADC_volts(int raw_data, int gain) {
  float vref = 4.5;
  float fs = vref / gain;
  float converted_data = fs * raw_data / (pow(2,23)-1);
  // Serial.print(raw_data);Serial.print(" -> ");Serial.println(converted_data);
  return converted_data;
}

// returns the last byte read
byte ADS_RREG(byte r , int n) {
  if (r + n - 1 > 24)
    n = 24 - r ;
  digitalWrite(pCS, LOW);
  // Serial.print("Register "); Serial.print(r, HEX); Serial.println(" Data");
  mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
  mySPI.transfer(SDATAC);
  mySPI.transfer(RREG | r); //RREG
  mySPI.transfer(n-1); // 24 Registers
  byte to_ret;
  for (int i = 0; i < n; i++)
  { byte temp = mySPI.transfer(0x00);
//    Serial.println(temp, HEX);
    if ((n-i) == 1) to_ret = temp;
  }
  mySPI.endTransaction();
  digitalWrite(pCS, HIGH);
  return to_ret;
}

void ADS_WREG(byte r, byte d) { 
  if (r == 0 || r == 18 || r == 19)
    Serial.println("Error: Read-Only Register");
  else
  { digitalWrite(pCS, LOW);
    mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
    mySPI.transfer(SDATAC);
    mySPI.transfer(WREG | r); //RREG
    mySPI.transfer(0x00); // 24 Registers
    mySPI.transfer(d);
    mySPI.endTransaction();
    digitalWrite(pCS, HIGH);
    Serial.print("Wrote ");Serial.print(d, BIN); Serial.print(" to Register "); Serial.println(r, HEX);
  }
}

void parse_serial_input() {
  //TODO consider this
  // char* s = Serial.readStringUntil('\n');
  char c = Serial.read();
  char p;
  // if char is '0' - '8'
  if (c >= 0x30 && c <= 0x38) {
    print_ch = (c -0x30);
    //         ('#'-> #) -> #-1 to index channels
    Serial.print("changed printing channel to ");Serial.println(print_ch);
    return;
  }
  switch (c) {
  case 'B':
  case 'b':
    p = Serial.read();
    c = Serial.read();
    if (c >= 0x31 && c <= 0x38) {
      if (p == 'p' || p == 'P') change_channel_biasP(c-0x30-1);
      if (p == 'n' || p == 'N') change_channel_biasN(c-0x30-1);
    }
    break;
  case 'S':
  case 's':
    c = Serial.read();
    if (c >= 0x31 && c <= 0x38) {
      change_channel_SRB2(c-0x30-1);
    }
    break;
  case 'G':
  case 'g':
    c = Serial.read();
    if (c >= 0x31 && c <= 0x38) {
      change_channel_gain(c-0x30-1);
    }
    break;
  case 'T':
  case 't':
    c = Serial.read();
    if (c >= 0x31 && c <= 0x38) {
      toggle_channel(c-0x30-1);
    }
    break;
  case 'p':
  case 'P':
    print_serial_instructions();
    break;
  case 'r':
  case 'R':
    print_ADS_reg_settings();
    break;
  case 0xA:
    break;
  default:
    Serial.println(c, HEX);
    Serial.println("!!invalid input");
    break;
  }
}

void change_channel_SRB2(int chan){
  char c = Serial.read();
  Serial.print("CHANNELS[chan] ");Serial.println(CHANNELS[chan],HEX);
  byte old_val = ADS_RREG(CHANNELS[chan], 1);
  byte change = 0;
  byte new_val;
  if (c == '1') {
    change = ADS1299_REG_CHNSET_SRB2_CONNECTED;
    new_val = old_val | change;
  } else if (c == '0'){
    change = 0xFF ^ ADS1299_REG_CHNSET_SRB2_CONNECTED;
    new_val = old_val & change;
  } else {
    Serial.println("invalid input");return;
  }
  Serial.print("changing SRB2 of channel "); Serial.print(chan);Serial.print(" to be ");Serial.println(c);
  Serial.println(change,BIN);
  Serial.print(old_val, BIN);Serial.print(" -> ");Serial.println(new_val, BIN);
  ADS_WREG(CHANNELS[chan], new_val);
  // START CONVERSION AGAIN
  if (DATA_MODE == RDATA_CC_MODE) {
    digitalWrite(pCS, LOW);
    mySPI.transfer(START);
    mySPI.transfer(RDATAC);
  }
}

void change_channel_biasN(int chan){
  char c = Serial.read();
  // Serial.print("CHANNELS[chan] ");Serial.println(CHANNELS[chan],HEX);
  byte old_val = ADS_RREG(ADS1299_REGADDR_BIAS_SENSN, 1);
  byte change = 0;
  byte new_val;
  if (c == '1') {
    change = BIAS_SENSN[chan];
    new_val = old_val | change;
  } else if (c == '0'){
    change = 0xFF ^ BIAS_SENSN[chan];
    new_val = old_val & change;
  } else {
    Serial.println("invalid input");return;
  }
  Serial.print("changing biasN of channel "); Serial.print(chan);Serial.print(" to be ");Serial.println(c);
  Serial.println(change,BIN);
  Serial.print(old_val, BIN);Serial.print(" -> ");Serial.println(new_val, BIN);
  ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, new_val);
  // START CONVERSION AGAIN
  if (DATA_MODE == RDATA_CC_MODE) {
    digitalWrite(pCS, LOW);
    mySPI.transfer(START);
    mySPI.transfer(RDATAC);
  }
}

void change_channel_biasP(int chan){
  char c = Serial.read();
  // Serial.print("CHANNELS[chan] ");Serial.println(CHANNELS[chan],HEX);
  byte old_val = ADS_RREG(ADS1299_REGADDR_BIAS_SENSP, 1);
  byte change = 0;
  byte new_val;
  if (c == '1') {
    change = BIAS_SENSP[chan];
    new_val = old_val | change;
  } else if (c == '0'){
    change = 0xFF ^ BIAS_SENSP[chan];
    new_val = old_val & change;
  } else {
    Serial.println("invalid input");return;
  }
  Serial.print("changing biasP of channel "); Serial.print(chan);Serial.print(" to be ");Serial.println(c);
  Serial.println(change,BIN);
  Serial.print(old_val, BIN);Serial.print(" -> ");Serial.println(new_val, BIN);
  ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, new_val);
  // START CONVERSION AGAIN
  if (DATA_MODE == RDATA_CC_MODE) {
    digitalWrite(pCS, LOW);
    mySPI.transfer(START);
    mySPI.transfer(RDATAC);
  }
}

void change_channel_gain(int chan){
  Serial.print("CHANNELS[chan] ");Serial.println(CHANNELS[chan],HEX);
  char c = Serial.read();
  byte old_val = ADS_RREG(CHANNELS[chan], 1);
  byte gain = 0;
  byte new_val;
  if (c >= 0x30 && c <= 0x36) {
    gain = GAINS[c-0x30];
    new_val = (old_val & (~gain_mask)) | gain;
  } else {
    Serial.println("invalid input");return;
  }
  Serial.print("changing gain of channel "); Serial.print(chan);Serial.print(" to be ");Serial.println(c-0x30,BIN);
  Serial.println(gain,BIN);
  Serial.print(old_val, BIN);Serial.print(" -> ");Serial.println(new_val, BIN);
  ADS_WREG(CHANNELS[chan], new_val);
  // START CONVERSION AGAIN
  if (DATA_MODE == RDATA_CC_MODE) {
    digitalWrite(pCS, LOW);
    mySPI.transfer(START);
    mySPI.transfer(RDATAC);
  }
}

void toggle_channel(int chan){
  Serial.print("CHANNELS[chan] ");Serial.println(CHANNELS[chan],HEX);
  char c = Serial.read();
  byte old_val = ADS_RREG(CHANNELS[chan], 1);
  byte new_val;
  if (c == '1') {
    new_val = old_val & 0x7F;
  } else if (c == '0'){
    new_val = old_val | 0x80;
  } else {
    Serial.println("invalid input");return;
  }
  Serial.print("turning channel "); Serial.print(chan);Serial.print(" to be ");
  if(c=='1') Serial.println("on");
  else       Serial.println("off");
  Serial.print(old_val, BIN);Serial.print(" -> ");Serial.println(new_val, BIN);
  ADS_WREG(CHANNELS[chan], new_val);
  // START CONVERSION AGAIN
  if (DATA_MODE == RDATA_CC_MODE) {
    digitalWrite(pCS, LOW);
    mySPI.transfer(START);
    mySPI.transfer(RDATAC);
  }
}

void print_ADS_reg_settings() {
  for(uint8_t address =0; address<24; address++){
    Serial.print("Register Address: 0x"); Serial.print(address,HEX);
    Serial.print("\t");
    Serial.print(ADS_reg_names[address]);
    Serial.print("\t");
    if(!(address>12 && address <20)) Serial.print("\t");
    byte data = ADS_RREG(address,1);
    Serial.print("Register Data: 0x"); Serial.print(data, HEX);
    Serial.print("\t");
    Serial.print("0b"); Serial.print(data, BIN);
    Serial.println();
  }

//  // START CONVERSION AGAIN
//  if (DATA_MODE == RDATA_CC_MODE) {
//    digitalWrite(pCS, LOW);
//    mySPI.transfer(START);
//    mySPI.transfer(RDATAC);
//  }
}

inline void get_IMU_data(long* packet){
  int16_t ax, ay, az, gx, gy, gz;
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  // NEED to convert from raw data to some underestandable units
  
  // insert imu data into packet
  ((int16_t*)&(packet[i_IMU]))[0] = ax;
  ((int16_t*)&(packet[i_IMU]))[1] = ay;
  ((int16_t*)&(packet[i_IMU]))[2] = az;
  ((int16_t*)&(packet[i_IMU]))[3] = gx;
  ((int16_t*)&(packet[i_IMU]))[4] = gy;
  ((int16_t*)&(packet[i_IMU]))[5] = gz;

  #if v
    for (int i = 0; i < 6; i++) {
//      Serial.println("IMU "+String(i)+" "+String( ((int16_t*)packet)[(2*i_IMU)+i])); //wrong indexing
      Serial.println("IMU "+String(i)+" "+String( ((int16_t*)&(packet[i_IMU]))[i]));
    }
  #endif


  /*  From MPU6050 register maps (pg 31), Gyroscope:
   * 
   * FS_SEL     Full Scale Range      LSB Sensitivity
   *    0           ± 250 °/s           131 LSB/°/s      [DEFAULT]
   *    1           ± 500 °/s           65.5 LSB/°/s
   *    2           ± 1000 °/s          32.8 LSB/°/s
   *    3           ± 2000 °/s          16.4 LSB/°/s
   */
  // conversion: g/131 = # °/s
  // float g_x = gx / 131.;
  // float g_y = gy / 131.;
  // float g_z = gz / 131.;

  /*  From MPU6050 register maps (pg 29) , Accelerometer:
   * 
   * FS_SEL     Full Scale Range      LSB Sensitivity
   *    0           ± 2 g               16384 LSB/g       [DEFAULT]
   *    1           ± 4 g               8192 LSB/g
   *    2           ± 8 g               4096 LSB/g
   *    3           ± 16 g              2048 LSB/g
   */
  // conversion: a/16384 = #g *9.81 = # m/s^2
  // float a_x = ax / 16384. * 9.81;
  // float a_y = ay / 16384. * 9.81;
  // float a_z = az / 16384. * 9.81;

  // insert converted imu data into packet
  // ((float*)&(packet[i_IMU]))[0] = a_x;
  // ((float*)&(packet[i_IMU]))[1] = a_y;
  // ((float*)&(packet[i_IMU]))[2] = a_z;
  // ((float*)&(packet[i_IMU]))[3] = g_x;
  // ((float*)&(packet[i_IMU]))[4] = g_y;
  // ((float*)&(packet[i_IMU]))[5] = g_z;

/*
  // MOT_DETECT_STATUS register
  uint8_t getMotionStatus();
  bool getXNegMotionDetected();
  bool getXPosMotionDetected();
  bool getYNegMotionDetected();
  bool getYPosMotionDetected();
  bool getZNegMotionDetected();
  bool getZPosMotionDetected();
  bool getZeroMotionDetected();
*/
}

// number of eda data points to use to average
int eda_avg_size = 10;
// current index of the value being added
int eda_idx = 0;
// current total so far for the first eda_idx samples
int eda_total = 0;
//int eda_vals[eda_avg_size];

inline void get_EDA_data(long* packet) {
  int vEDA = analogRead(pEDA);
//  eda_vals[eda_idx] = vEDA;
  eda_total += vEDA;
  eda_idx = (eda_idx+1) % eda_avg_size;
  // if we have collected enough samples to average
  if ((eda_idx % eda_avg_size) == 0) {
//    for (int i = 0; i < eda_avg_size; i++)
    float avg_vEDA = eda_total / eda_avg_size;
    float Rskin = convert_eda_adc_to_Rskin(avg_vEDA);
    packet[i_EDA] = *((long*)(&Rskin));
    eda_total = 0;
  } else {
    // if not ready to average yet, mark EDA as invalid
    packet[i_VALID] |= (1<<v_EDA);  
  }
//  Serial.println(String(eda_idx)+" , "+String(eda_total));

  #if v
//    Serial.print("EDA "); Serial.println(vEDA);
    Serial.print("EDA "); Serial.println(packet[i_EDA]);
  #endif
}

inline float convert_eda_adc_to_Rskin(int sensorValue) {
  float Vout = (sensorValue * 3.3)/4095;

  // these are constants- should not change between iterations
  // values are from the PCB layout/schematic in Fascia Physio Board V0
  const int Rref = 820000; // reference resistor between - opamp and gnd
  // Rref might actually be 2M or 820K -- undocumented board build value
  const float Vref = 3.3 * 20./(20.+140.); // voltage divider output (virtual gnd)
  const float i = (float)Vref / (float)Rref;

  float Rskin = (Vout - Vref) / i;
//  Serial.println(String(Vref)+", "+ String(Vout)+", "+String(Rskin));
  //float Cskin = 1./Rskin;

  return Rskin;
}

//inline void get_battery_v(long* packet) {
//    packet[i_BAT] = analogRead(pBAT);
//}

inline void get_PPG_temp_data(long* packet) {
  particleSensor.requestTemperature();
  long irValue = particleSensor.getIR(1); // ms to wait TODO figure out smallest good
                                          // TODO MATCH THIS WITH DATA SAMPLE RATE IN .SETUP()
  packet[i_PPG] = irValue;
//  Serial.println(irValue);
  if (irValue < 50000){
    #if v
      Serial.println("No contact with sensor "+String(irValue));
    #endif
    packet[i_VALID] |= (1<<v_PPG);
    packet[i_VALID] |= (1<<v_TEM); // TODO keep this here????
    if (irValue != 0) packet[i_VALID] |= (1<<v_TIM);
//    return;
  }

  float temperature = particleSensor.readTemperature();
  packet[i_TEM] = *(long*)(&temperature); //TODO make sure this casting works properly

  #if v
    Serial.print("TMP ");Serial.println(temperature);
  //  Serial.print("TMP ");Serial.println(packet[i_TEM]);
  
    Serial.print("PPG ");Serial.println(irValue);
  //  Serial.print("PPG ");Serial.println(packet[i_PPG]);
  #endif
}

/////////////////////////////////// WIFI STUFF //////////////////////////////////////
//int cnt = 0;

//There are two buffer used for wifi data sending.
char sendBuf[2][PACKET_SIZE*SEND_SIZE];

//Indicating which buffer is being written
int wBufIndex = 0;

//Indicates how much data packets are written into the current buffer
int wCount = 0;
bool isBufReady = false;

//For WIFI
int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
unsigned int localPort = PORT_NUM;      // local port to listen on

WiFiUDP Udp;


inline char* getWriteBuf()
{
    return sendBuf[wBufIndex];
}

//Get the buffer for sending
//Have to be in non-interrupt context
inline char* getSendBuf()
{
    if(isBufReady == true)
    {
        isBufReady = false;
        return wBufIndex == 0 ? sendBuf[1] : sendBuf[0];
    }
    else
    {
        return 0;
    }
}

//Write one data packet into the buffer.
inline void pushToBuf(char* packet)
{
    ((int*)packet)[0] = cnt;
    ((unsigned long*)packet)[i_TIM] = micros();
//    Serial.print(((int*)packet)[0]); Serial.print(" : "); Serial.println(((int*)packet)[i_TIM]); 
    cnt++;
    //When current buffer is full
    if(wCount == SEND_SIZE)
    {
        //Switch buffer
        wBufIndex = (wBufIndex +1)%2;
        isBufReady = true;
        wCount = 0;
        //Serial.println("Switch Buffer");
    }

    //Write to buffer
    char* buf = getWriteBuf();
    memcpy(buf+wCount*PACKET_SIZE, packet, PACKET_SIZE);
    wCount++;
    
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.print("Data host IP: ");
  Serial.println(HOST_ID);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setupWifi()
{
    //Setup the wifi
    // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);

}


inline void sendWiFiDataPacket() {
  byte* sBuf = (byte*)getSendBuf();
  
  if(sBuf != 0){
    Udp.beginPacket(HOST_ID, PORT_NUM);
    int nbytes = Udp.write(sBuf, PACKET_SIZE*SEND_SIZE);
    Udp.endPacket();
    if (nbytes != PACKET_SIZE*SEND_SIZE) {
      Serial.println("Failed to send packet. Sent "+String(nbytes)+" bytes only");  
    } else { 
      #if v 
        Serial.println("Successfully sent full packet of "+String(nbytes)+" bytes");
      #endif
    }
  }
  LEDval = (cnt%10)? LEDval : !LEDval;
//  Serial.println("cnt = "+String(cnt)+" LEDVAL = "+String(LEDval));
  digitalWrite(pLED, LEDval);
}
