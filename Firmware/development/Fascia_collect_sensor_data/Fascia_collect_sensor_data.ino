// library includes
#include "SAMD_AnalogCorrection.h"
#include <SPI.h>
#include "wiring_private.h"
// #include <MPU6050.h>
#include <Wire.h>
#include "MAX30105.h"
//#include "heartRate.h"
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

#define BOARD_V FASCIA_V0_0
#define DATA_MODE RDATA_SS_MODE
#define RUN_MODE NORMAL_ELECTRODES
// v for verbose: lots of prints
#define v 0
// debug: serial reads and writes
#define debug 0

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
const int pMPUint = PB03;
// PPG & temperature pins
MAX30105 particleSensor;

// define variables
int print_ch = -1;
bool LEDval = LOW;

int cnt = 0;

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
  ADS_WREG(ADS1299_REGADDR_CH1SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH2SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH3SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH4SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH5SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH6SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode | 
                                   ADS1299_REG_CHNSET_CHANNEL_ON|
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH7SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH8SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   channel_mode |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
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

void setup_MAX30105() {
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {//Use default I2C port, 400kHz speed
    while (1){Serial.println("MAX30105 was not found. Please check wiring/power. ");}
  }
  //The LEDs are very low power and won't affect the temp reading much but
  //you may want to turn off the LEDs to avoid any local heating
  // can try setting data output rate (currently (default) close to the slowest)
  particleSensor.setup(/*byte powerLevel = */0x1F, /*byte sampleAverage = */4, /*byte ledMode =*/ 3, /*int sampleRate =*/1000); //Configure sensor. Turn off LEDs
  // TODO increase sampling rate here!!!!
  
  //particleSensor.setup(); //Configure sensor. Use 25mA for LED drive
  //TODO seems like line below is not actually necessary
  particleSensor.enableDIETEMPRDY(); //Enable the temp ready interrupt. This is required.
  // particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
}

void setup() {
  delay(2000);
  // initialize communications: spi, I2C, serial, and wifi if applicable
  mySPI.begin();
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

  // initialize MAX30105 PPG sensor (we will also be getting temperature data from it)
  setup_MAX30105();

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
  Serial.println("type B#0 to deactivate bias for channel # and B#1 to activate it");
  Serial.println("type S#0 to deactivate SRB2 for channel # and B#1 to activate it");
  Serial.println("type G#N to set the gain for channel # to N=0:1, N=1:2, N=2:4, N=3:6, N=4:8, N=5:12, N=6:24");
  Serial.println("type T#0 to toggle channel # off, and T#1 to toggle channel # on");
  Serial.println("type P to print these instructions again");

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
  get_IMU_data(packet);
  get_EDA_data(packet);
  if (!(cnt%10)) {
    get_PPG_temp_data(packet);
  } else {
    // TODO make indicator bits reflect not valid data in those points  
//    packet[i_PPG] = 0;
//    packet[i_TEM] = 0;
//    packet[i_BAT] = 0;
    packet[i_VALID] |= (1<<i_PPG);
    packet[i_VALID] |= (1<<i_TEM);
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

    // TODO use loff-p and loff-n to figure out which channels might be invalid
    
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
      float cd = convert_ADC_volts(ed, 1);
      packet[i_ADS + (i/3-1)] = ed;
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
    Serial.println(temp, HEX);
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
    c = Serial.read();
    if (c >= 0x31 && c <= 0x38) {
      change_channel_bias(c-0x30-1);
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

void change_channel_bias(int chan){
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

void change_channel_short(int chan){}

inline void get_IMU_data(long* packet){
  // Wire.requestFrom(MPU_ADDR, #, bool);
  for (int i = 0; i < 6; i++) {
    packet[i_IMU+i] = i;
    #if v
      Serial.println("IMU "+String(i)+" "+String(i));
//      Serial.println("IMU "+String(i)+" "+String(packet[i_IMU+i]));    
    #endif
  }
}

inline void get_EDA_data(long* packet) {
  int vEDA = analogRead(pEDA);
  packet[i_EDA] = vEDA;
  #if v
    Serial.print("EDA "); Serial.println(vEDA);
//    Serial.print("EDA "); Serial.println(packet[i_EDA]);
  #endif
}

//inline void get_battery_v(long* packet) {
//    packet[i_BAT] = analogRead(pBAT);
//}

//const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
//byte rates[RATE_SIZE]; //Array of heart rates
//byte rateSpot = 0;
//long lastBeat = 0; //Time at which the last beat occurred
//
//float beatsPerMinute;
//int beatAvg;

inline void get_PPG_temp_data(long* packet) {
  particleSensor.requestTemperature();
  // maybe include both here: heart rate and raw ppg data
  long irValue = particleSensor.getIR(5); // ms to wait TODO figure out smallest good
                                          // TODO MATCH THIS WITH DATA SAMPLE RATE IN .SETUP()
  packet[i_PPG] = irValue;
//  Serial.println(irValue);
  if (irValue < 50000){
     #if v
      Serial.println("No contact with sensor");
    #endif
    packet[i_VALID] |= (1<<i_PPG);
    packet[i_VALID] |= (1<<i_TEM); // TODO keep this here????
//    return;
  }

//  // TODO calculate heart rate
//  bool beat = false;
//  if (checkForBeat(irValue) == true){
//    // Serial.println("we sensed a beat!");
//    beat = true;
//    //We sensed a beat!
//    long delta = millis() - lastBeat;
//    lastBeat = millis();
//
//    beatsPerMinute = 60 / (delta / 1000.0);
//
//    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
//      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
//      rateSpot %= RATE_SIZE; //Wrap variable
//
//      //Take average of readings
//      beatAvg = 0;
//      for (byte x = 0 ; x < RATE_SIZE ; x++)
//        beatAvg += rates[x];
//      beatAvg /= RATE_SIZE;
//    }
//  }
//  packet[i_PPG+1] = /**(char*)(&*/beatAvg;//);

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
