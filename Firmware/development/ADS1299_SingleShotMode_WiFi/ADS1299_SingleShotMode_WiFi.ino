// library includes
#include "SAMD_AnalogCorrection.h"
#include <SPI.h>
#include "wiring_private.h"
// header files
#include "Pin_Table_Defs.h"
#include "WiFi_Settings.h"
// #include "WiFi_Helpers.cpp"
#include "ads1299.h"

// define enum for boards and data retrieval types
enum board_types {NOVA_XR_V1, NOVA_XR_V2_SISTER, NOVA_XR_V2_MAIN, FASCIA_V0_0, FASCIA_V0_1};
enum data_mode_t {RDATA_CC_MODE, RDATA_SS_MODE};
enum run_mode_t  {GEN_TEST_SIGNAL, NORMAL_ELECTRODES};

// settings
#define CONNECT_WIFI 1
#define BOARD_V FASCIA_V0_0
#define DATA_MODE RDATA_SS_MODE
#define RUN_MODE GEN_TEST_SIGNAL

// Setup for SPI communications
SPIClass mySPI (&sercom1, PA19, PA17, PA16, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
const int SPI_CLK = 4*pow(10,6) ; //4MHz

// define pins depending on boards
const int pRESET = PA23;    // reset pin
int pCS;              // chip select pin
int pDRDY;            // data ready pin



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
        Serial.println("initializing pins for Fascia V1.0");
        pCS = PA20;
        pDRDY = PA07;
        break;
    }
    case FASCIA_V0_1: {
        Serial.println("initializing pins for Fascia V1.1");
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
  pinPeripheral(PA19, PIO_SERCOM);
  pinPeripheral(PA17, PIO_SERCOM);
  pinPeripheral(PA16, PIO_SERCOM);
  if (RUN_MODE == RDATA_CC_MODE) {
    attachInterrupt(pDRDY, DRDY_ISR, FALLING);
  }
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
    while(1);
  }
}

void ADS_init(void) {
  byte config2_data = 0b11000000;
  byte config3_data = 0b11101100;
  byte channel_mode = ADS1299_REG_CHNSET_NORMAL_ELECTRODE;

  if (RUN_MODE == GEN_TEST_SIGNAL) {
    config2_data = 0b11010000;
    config3_data = 0b11100000;
    channel_mode = ADS1299_REG_CHNSET_TEST_SIGNAL;
  }

  //initialize to single shot mode
  ADS_WREG(ADS1299_REGADDR_CONFIG1, 0b10010101);//0xB0);//0b10110000 //0b10010101);
  // /* test signal */ ADS_WREG(ADS1299_REGADDR_CONFIG2, 0b11010000);//0x04);//0b00000100 //0b11000000);
  // ADS_WREG(ADS1299_REGADDR_CONFIG2, 0b11000000);//0x04);//0b00000100 //0b11000000);
  // /* test signal */ ADS_WREG(ADS1299_REGADDR_CONFIG3, 0b11100000);//0xEC);//0b11101100 //0b01100000);
  // ADS_WREG(ADS1299_REGADDR_CONFIG3, 0b11101100);//0xEC);//0b11101100 //0b01100000);
  ADS_WREG(ADS1299_REGADDR_CONFIG2, config2_data);
  ADS_WREG(ADS1299_REGADDR_CONFIG3, config3_data);
  ADS_WREG(ADS1299_REGADDR_CONFIG4, 0x00);//0b00001000);
  ADS_WREG(ADS1299_REGADDR_GPIO, 0x00);
  ADS_WREG(ADS1299_REGADDR_MISC1, 0x00);

  delay(10);
  ADS_WREG(ADS1299_REGADDR_CH1SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                  channel_mode |
                                  //  /* test signal */ ADS1299_REG_CHNSET_TEST_SIGNAL |
                                  //  ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_ON |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH2SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   ADS1299_REG_CHNSET_TEST_SIGNAL | //ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH3SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   ADS1299_REG_CHNSET_TEST_SIGNAL | //ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH4SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   ADS1299_REG_CHNSET_TEST_SIGNAL | //ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH5SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   ADS1299_REG_CHNSET_TEST_SIGNAL | //ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH6SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   ADS1299_REG_CHNSET_TEST_SIGNAL | //ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH7SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   ADS1299_REG_CHNSET_TEST_SIGNAL | //ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
  ADS_WREG(ADS1299_REGADDR_CH8SET, ADS1299_REG_CHNSET_GAIN_1 | 
                                   ADS1299_REG_CHNSET_TEST_SIGNAL | //ADS1299_REG_CHNSET_NORMAL_ELECTRODE |
                                   ADS1299_REG_CHNSET_CHANNEL_OFF |
                                   ADS1299_REG_CHNSET_SRB2_DISCONNECTED);
}

void ADS_start(void) {
  digitalWrite(pCS, LOW);
  mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
  mySPI.transfer(START);
  if (RUN_MODE == RDATA_SS_MODE) {
    mySPI.transfer(RDATA);
  } else if (RUN_MODE == RDATA_CC_MODE) {
    mySPI.transfer(RDATAC);
  }
}

void setup() {
  delay(500);
  // initialize communications: spi, serial, and wifi if applicable
  mySPI.begin();
  Serial.begin(115200);
  #ifdef CONNECT_WIFI
    setupWifi();
  #endif

  // initialize board and settings
  // make sure settings are compatible
  if (BOARD_V == FASCIA_V0_0 && DATA_MODE == RDATA_CC_MODE) {
    Serial.println("You cannot use Continuous Conversion mode on Fascia Version 0.0");
    Serial.println("The DRDY pin on this PCB (PA07) cannot be configured as an interrupt pin");
    while(1);
  }
  select_board_pins();
  initialize_pin_modes();

  // initialize ads1299
  ADS_connect();
  ADS_init();
  ADS_start();
  Serial.println("Done with setup.");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (DATA_MODE == RDATA_SS_MODE) {
    DRDY_ISR();
  }

  #ifdef CONNECT_WIFI
    sendWiFiDataPacket();
  #endif

}

void DRDY_ISR(void) {
  //figure out where best to call START single shot data conversion
  while(digitalRead(pDRDY)) ;//{Serial.println("waiting for drdy to go low");}
  // Serial.println("DRDY just went low!");
  //get all data before sign extending etc
  digitalWrite(pCS, LOW);
  byte DOUT[27];
  int32_t conv_data[9];
  mySPI.transfer(START);
  mySPI.transfer(RDATA);

  for (int i = 0; i < 27; i++) {
    DOUT[i] = mySPI.transfer(0x00);
  }
  long packet[10];
  byte temp[4] = {0,0,0,0};
  for (int i = 0; i < 27; i++){
    temp[2-((i+3)%3)] = DOUT[i];
    if ((i+3)%3 == 2) {
      int32_t d = *((int32_t*)temp);
      int ed = (i/3 == 0)? d : SIGNEXTEND(d);
      float cd = convert_ADC_volts(ed, 1);
      conv_data[i/3] = d;
      packet[i/3+1] = ed;
      if (i/3 == 0) {
        // Serial.print("32 bit STAT: "); Serial.println(d,HEX);
        // STATUS Bits
      } else {
        if (i/3 == 1) {/*Serial.print(cd*1000, DEC); Serial.print(" , ");*/Serial.println(ed);}
        // Serial.print(ed);Serial.print(" , ");
        // Serial.print("32 bit CH#");Serial.print(i/3);Serial.print(": "); Serial.print(d,HEX);Serial.print("\t");Serial.print(ed,HEX);Serial.print("\t");Serial.println((int)ed);
        // channel data
      }
    }
  }
  //Push packet to buffer
  #ifdef CONNECT_WIFI
    pushToBuf((char *)packet);
  #endif
  // Serial.print("\n");
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
  // else if (n == 0xFF)
  // { digitalWrite(pCS, LOW);
  //   mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
  //   mySPI.transfer(SDATAC);
  //   for (int i = 5; i < 13; i++)
  //   { mySPI.transfer(WREG | i); //RREG
  //     mySPI.transfer(0x00);
  //     mySPI.transfer(t);
  //   }
  //   mySPI.endTransaction();
  //   digitalWrite(pCS, HIGH);
  //   Serial.println("Written All Channels");
  // }
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

// long ch[8];
int cnt = 0;

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
unsigned int localPort = 8899;      // local port to listen on


WiFiUDP Udp;


char* getWriteBuf()
{
    return sendBuf[wBufIndex];
}

//Get the buffer for sending
//Have to be in non-interrupt context
char* getSendBuf()
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
void pushToBuf(char* packet)
{
    packet[0] = cnt;
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
    memcpy(buf+wCount*PACKET_SIZE, packet, PACKET_SIZE  );
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


void sendWiFiDataPacket() {
    byte* sendBuf = (byte*)getSendBuf();
    if( sendBuf != 0){
        // Serial.println("Sending:"+ String(cnt));
        //No interrupt
        //noInterrupts();

        Udp.beginPacket(HOST_ID, PORT_NUM);

        Udp.write(sendBuf, PACKET_SIZE*SEND_SIZE);
        Udp.endPacket();

        //interrupts();
        // Serial.println(((long int*)(sendBuf))[0]);
        // Serial.println(((long int*)(sendBuf))[4]);

        // Serial.println("Sent:"+ String(cnt));
    }
}