/*
 * Author:       ioannis Smanis
 * Library for:  ADS1299 ADC chip
 * Project:      NovaXR - MKR1010
 * Date:         10/3/2019
 * 
 */


#include "ADS1299_lib.h"
#include "wiring_private.h"
#include <SPI.h>
#include "NovaXR_Library.h"

#define DEBUG
#define SISTER_V1


// ------------------ Custom SPI Module for SAMD21XXX -------------------------
#define PIN_SPI_MISO PA19 
#define PIN_SPI_SCK  PA17 
#define PIN_SPI_MOSI PA16

SPIClass NovaXR_SPI (&sercom1, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_MOSI, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);  

// --- for any ADS1299 -----------
uint8_t ADS_Reset_pin;             ///< ADS1299 Reset Pin External Variable initialization 
uint8_t ADS_CS_pin;                ///< ADS1299 Chip Select Pin External Variable initialization 
uint8_t ADS_DRDY_pin;              ///< ADS1299 DRDY External Variable initialization 
boolean continuousRead = false;    ///< Flag warning for continuous conversion is happening

// --- Store Captured Data here ---
adc_data MBdataADC;
adc_data SBdataADC;
adc_data rawADC;
//extern adc_data dataBuffer[130];  // for interrupt use

// --- Internal declarations for Benchmarking Purpose-------
uint16_t SampleID1=0; 
uint16_t SampleID2=0; 
long time1[10];   ///< for testing purpose 





// ======================== Constructors ==============================
/**
 * @brief      Begins NovaXR Custom SPI Module.
 */
void Begin_ADS(void){
   NovaXR_SPI.begin();
   SPI_pinPeriphernal_init();
   digitalWrite(SisCS, HIGH);
   digitalWrite(MainCS, HIGH);
}


/**
 * @brief      Initialize SPI Pins for SERCOM.
 */
void SPI_pinPeriphernal_init(void){
    pinMode(MainDRDY, INPUT);
    pinMode(SisDRDY , INPUT);
    pinMode(SisCS   ,OUTPUT);
    pinMode(MainCS  ,OUTPUT);
    pinMode(ADS_Reset_pin ,OUTPUT);
    pinPeripheral(PIN_SPI_MISO, PIO_SERCOM);
    pinPeripheral(PIN_SPI_SCK,  PIO_SERCOM);
    pinPeripheral(PIN_SPI_MOSI, PIO_SERCOM);
}


/**
 * @brief      Send Reset Command to an active ADS1299
 */
void Reset_ADC(void){
    digitalWrite(ADS_CS_pin, HIGH);
    delay(500);  //delay to ensure connection
    digitalWrite(ADS_CS_pin, LOW); 
      NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
      NovaXR_SPI.transfer(RESET); 
      NovaXR_SPI.endTransaction();
      delay(100);
    digitalWrite(ADS_CS_pin, HIGH);
}



/**
 * Select ADS1299 to Control
 * 
 * @param  adc_id 
 *               
 */
void select_ADC( uint8_t adc_id){
    if (adc_id == MAIN_BOARD ) {
       ADS_CS_pin = MainCS;
       ADS_DRDY_pin = MainDRDY;  
          
    } else if (adc_id == SISTER_BOARD ) {
       ADS_CS_pin = SisCS;
       ADS_DRDY_pin = SisDRDY;
     
    }else{
       SerialUSB.println("ERROR: Invalid ADS ID");  
       SerialUSB.println("NOTE: Default ADS ID = 1 (MainBoard) ADS ID = 2 (SisterBoard)");   
       ADS_CS_pin = MainCS;
       ADS_DRDY_pin = MainDRDY;
    }
    digitalWrite(ADS_CS_pin, HIGH);
}


/**
 * @brief      Enable ADS1299 Hardware.
 *
 * @param[in]  adc_id  The ADS1299 identifier
 */
void enable_ADS1299(uint8_t adc_id){
    if (adc_id == MAIN_BOARD ) {
        ADS_Reset_pin = ResetMB_pin;
    } else if (adc_id == SISTER_BOARD ) {
        ADS_Reset_pin = ResetSB_pin;     
    }else{
       SerialUSB.println("ERROR: Invalid ADS ID");  
       SerialUSB.println("NOTE: Default ADS ID = 1 (MainBoard) ADS ID = 2 (SisterBoard)");   
       ADS_Reset_pin = ResetMB_pin; // Default NovaXR Option
    }  
    digitalWrite(ADS_Reset_pin, HIGH);
}




/**
 * @brief      Gets the current ADS1299 id.
 *
 * @return     The current ads id.
 */
int8_t getCurrent_ADS_ID(void){
     #define ERROR_BOARD_ID -1
     int8_t board_id = ERROR_BOARD_ID;
     
     if((ADS_CS_pin == MainCS)&&(ADS_DRDY_pin == MainDRDY)){
        board_id = MAIN_BOARD;
     }else if ((ADS_CS_pin == SisCS)&&(ADS_DRDY_pin == SisDRDY)){
        board_id = SISTER_BOARD;
     }else {
        Serial.println("ERROR: UNKNOWN ADS1299 BOARD ID");
     }
     return board_id;
}


/**
 * @brief  Read Settings of a single register
 * 
 * @param  RegAddr is the address of the register we target to read
 *               
 * @return returnedByte byte holds the register value
 * 
 */
byte readRegisterSettings(byte RegAddr){
     byte returnedByte =0x00;  
     digitalWrite(ADS_CS_pin, LOW); 
       NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
       NovaXR_SPI.transfer(SDATAC);       // Stop Continuous Conversion
       do_RREG ( RegAddr, 1);             // get 1 byte data     
       returnedByte = NovaXR_SPI.transfer(0x00);
       NovaXR_SPI.endTransaction();
     digitalWrite(ADS_CS_pin, HIGH);
     return returnedByte;
 }


/**
 * @brief      Display Current Registers Settings.
 */
void display_Registers_settings(void){
   String Registers_NTable[24]={"ID","CONFIG1","CONFIG2","CONFIG3","LOFF","CH1SET","CH2SET","CH3SET","CH4SET","CH5SET","CH6SET","CH7SET","CH8SET",
                               "BIAS_SENSP","BIAS_SENSN","LOFF_SENSP","LOFF_SENSN","LOFF_FLIP","LOFF_STATP","LOFF_STATN","GPIO","MISC1","MISC2","CONFIG4"};
   //Serial.print("ADS1299 -> DRDY: "); Serial.println(ADS_DRDY_pin);
   //Serial.print("ADS1299 ->   CS: "); Serial.println(ADS_CS_pin);
   Serial.print("-------------------- ADS1299 Registers Settings: ");
   if(getCurrent_ADS_ID() == MAIN_BOARD){
      Serial.print("MAIN BOARD");  
   }
   else if (getCurrent_ADS_ID() == SISTER_BOARD){
      Serial.print("SISTER BOARD"); 
   }
   Serial.println(" --------------------");
   for(uint8_t address =0; address<24; address++){  
       Serial.print("Register #: 0x");  SerialUSB.print(address,HEX); 
       Serial.print(" \t");
       Serial.print(Registers_NTable[address]);
       Serial.print("\t");
       if(!(address>12 && address <20))Serial.print("\t");
       Serial.print("Config Bits: 0x"); SerialUSB.print(readRegisterSettings(address),HEX);
       Serial.print(" \t");
       Serial.print("Bin: 0b");         SerialUSB.print(readRegisterSettings(address),BIN);
       Serial.println();
   }
   Serial.println("-------------------------------------------------------------------------------");
}



/**
 * @brief      Configure an ADS1299 Register.
 *
 * @param[in]  R_Address  The ADS1299 Register address
 * @param[in]  data       The configuration value data byte
 */
void config_ADS_Register(byte R_Address, byte data){
    if(R_Address >=0 && R_Address<24){                     // Protect for out of band register value
        if(R_Address==0||R_Address==18||R_Address==19){    // Protect for Read-Only Registers
            SerialUSB.println("ERROR: Read-Only Register"); 
        }else{
          digitalWrite(ADS_CS_pin, LOW);
            NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
            NovaXR_SPI.transfer(SDATAC);
            do_WREG (R_Address,data);
            NovaXR_SPI.endTransaction();
          digitalWrite(ADS_CS_pin, HIGH);
        }
    }else{
       SerialUSB.println("Error: Non existed Register"); 
    }
}



/**
 * @brief      Configure all ADC Inputs at once.
 *
 * @param[in]  data  The configuration value data byte
 */
void config_All_ADS_Inputs(byte data){
      digitalWrite(ADS_CS_pin, LOW);
        NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
        NovaXR_SPI.transfer(SDATAC);
        
        for(int i=5; i<13; i++){   // All Analog Input Channels 
          do_WREG (i,data);
        }  
        NovaXR_SPI.endTransaction();
      digitalWrite(ADS_CS_pin, HIGH);
}



/**
 * @brief      Check id ADS1299 device is present on the board.
 *
 * @return     True if ADS1299 is connected to the system
 */
bool ADS_connected(void){
   bool error=false;
   byte data_id = readRegisterSettings(ID_REG);  
   if (data_id == 0xFF){
     error=true;
   }else if (data_id == ADS_ID){
     error=false;
   }else{
     error=true;
     Serial.print("UNKNOWN ERROR related to:");
     print_boardID(getCurrent_ADS_ID());
     Serial.print(" --> Register Data: 0x");Serial.println(data_id ,HEX);
   }
  return error;
}



/**
 * @brief      Configure a single ADS1299 Input.
 *
 * @param[in]  channel  The inout channel selection
 * @param[in]  data     The configuration value data byte
 */
void config_ADS_Input(uint8_t channel, byte data){
      uint8_t Input[] = {5,6,7,8,9,10,11,12};
      digitalWrite(ADS_CS_pin, LOW);
        NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
        NovaXR_SPI.transfer(SDATAC);
        do_WREG(Input[channel],data);
        NovaXR_SPI.endTransaction();
      digitalWrite(ADS_CS_pin, HIGH);      
}



/**
 * @brief      Start ADS1299 in Continuous Conversion Mode.
 */
void startADC_Continuous_Mode(void){
      if(!continuousRead){
         continuousRead = true ; 
         digitalWrite(ADS_CS_pin, LOW);
           NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1)); 
           NovaXR_SPI.transfer(START);
           NovaXR_SPI.transfer(RDATAC);
         digitalWrite(ADS_CS_pin, HIGH);
         Serial.println(digitalRead(MainDRDY));
         // -- enable Hardware Interrupt 
         attachInterrupt(digitalPinToInterrupt(ADS_DRDY_pin), getADC1_Data_CM, FALLING); // ADC1 DATA - Change it when you have 2 ADS1299 connected
      } else{
        Serial.println("WARNING: you are already in Continuous Mode");
      }
}



/**
 * @brief      For ADC1 on mainbaord - Interrupt Routine to Get Data in shared Union Structure: "ADC_data".
 */
void getADC1_Data_CM(void){  // Continuous Mode get data via interrupt
     time1[SampleID1]=micros(); // for Benchmarking purpose
     for(uint8_t i=0;i<27;i++){   // get all pushed 216Bits data from ADS1299
       byte temp = NovaXR_SPI.transfer(0x00);
       MBdataADC.rawData[i]= temp;
     }
     digitalWrite(ADS_CS_pin,HIGH); 
     //fill_PktBuff(&ADC1_data,SampleID1);
     SampleID1++; 
}



/**
 * @brief       For ADC2 on mainbaord - Interrupt Routine to Get Data in shared Union Structure: "ADC_data".
 */
void get_ADC2_Data_CM(void){  // Continuous Mode get data via interrupt
     time1[SampleID1]=micros(); // for Benchmarking purpose
    
     digitalWrite(ADS_CS_pin, LOW);
       for(uint8_t i=0;i<27;i++){   // get all pushed 216Bits data from ADS1299
         byte temp = NovaXR_SPI.transfer(0x00);
         SBdataADC.rawData[i]= temp;
       }
     digitalWrite(ADS_CS_pin,HIGH);  
     //fill_PktBuff(&ADC2_data,SampleID2);
     SampleID2++; 
}



/**
 * @brief      Stop Being interrupted by ADS1299.
 */
void stop_ADC_interrupt(void){
   //  --- Disable Hardware Interrupt
   disable_ContninuousConversion();  // -- stop continuous converion
   detachInterrupt(digitalPinToInterrupt(ADS_DRDY_pin)) ;
 
 #ifdef DEBUG   
   Serial.print("Samples: ");
   Serial.println(SampleID1); // test  purpose 

   for(int w=0; w<10; w++ ){
    Serial.print("Time Stamps: ");
    Serial.println(time1[w]); // test  purpose 
   }
   Serial.println();
   for(int z=9; z>0; z-- ){
    Serial.print("Interrupt Diffs: ");Serial.println(time1[z]-time1[z-1]);
   }
 #endif
 
}



/**
 * @brief      Disables the contninuous conversion.
 */
void disable_ContninuousConversion(void){
  if(continuousRead){
      digitalWrite(ADS_CS_pin, LOW);
      NovaXR_SPI.transfer(SDATAC);
      digitalWrite(ADS_CS_pin, HIGH);
      continuousRead = false ;
   }else{
      Serial.println("ERROR: You are Not in Continuous Mode");  
   }
}



/**
 * @brief      Stop ADC Conversion Operation.
 */
void ADS_Stop_Conversion(void){
      digitalWrite(ADS_CS_pin, LOW);
      NovaXR_SPI.transfer(STOP);
      digitalWrite(ADS_CS_pin, HIGH);   
}



/**
 * @brief      Stops and Sets configurations for selected input channel and board.
 *
 * @param[in]  board_id  The board identifier
 * @param[in]  channel   The ADS1299 input channel num
 * @param[in]  data      The configuration value data byte
 */
void stop_and_configInput(uint8_t board_id, uint8_t channel, byte data){ 
      select_ADC(board_id);
      ADS_Stop_Conversion();
      config_ADS_Input(channel,0x00);
      config_ADS_Input(channel,data);
      Serial.print(">>>>>> Channel: "); Serial.print(channel); Serial.print(" -> Board: "); Serial.println(board_id);
      //display_Registers_settings();
}



/**
 * @brief      Single Shot Mode - Get Data in shared Union Structure: "ADC_data".
 *
 * @param      adc  The adc_data structure
 */
void get_ADC_Data_SS(adc_data * adc){  // Single Shot mode get data via interrupt   
     digitalWrite(ADS_CS_pin, LOW); 
     NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));        
     NovaXR_SPI.transfer(START);
     NovaXR_SPI.transfer(RDATA);  // set single shot data read
     
     #ifndef SISTER_V1
       while(digitalRead(ADS_DRDY_pin));
     #endif
     
     for(uint8_t i=0;i<27;i++){   // get all pushed 216Bits data from ADS1299
       byte rawByte = NovaXR_SPI.transfer(0x00);
        adc->rawData[i]= rawByte;
     }
     digitalWrite(ADS_CS_pin,HIGH);
}




/**
 * @brief      Single Shot Mode - Get Data in shared Union Structure: "ADC_data".
 */
void get_ADC_Data_SSAlt(void){  // Single Shot mode get data via interrupt   
     digitalWrite(ADS_CS_pin, LOW); 
     NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));        
     NovaXR_SPI.transfer(START);
     NovaXR_SPI.transfer(RDATA);  // set single shot data read
     
     #ifndef SISTER_V1
       while(digitalRead(ADS_DRDY_pin));
     #else
       if(getCurrent_ADS_ID() == 1) {
         while(digitalRead(ADS_DRDY_pin));
       }
     #endif
     
     for(uint8_t i=0;i<27;i++){   // get all pushed 216Bits data from ADS1299
       byte rawByte = NovaXR_SPI.transfer(0x00);
       rawADC.rawData[i]= rawByte;
     }
     digitalWrite(ADS_CS_pin,HIGH);
}



/**
 * @brief      Performs a WREG ADS1299 command - Writes a Register .
 *
 * @param[in]  Register  The register address to write
 * @param[in]  Data      The configuration value data-byte
 */
void do_WREG (byte Register,byte Data){
      NovaXR_SPI.transfer(WREG|Register);  // WREG
      NovaXR_SPI.transfer(0x00); 
      NovaXR_SPI.transfer(Data);
}



/**
 * @brief      Performs a RREG ADS1299 command - Reads a Register.
 *
 * @param[in]  register_Address  The register address to read
 * @param[in]  returned_bytes    The returned configuration value data-byte that the target register holds
 */
void do_RREG ( byte register_Address, byte returned_bytes){
     NovaXR_SPI.transfer(RREG|register_Address); // RREG | Register Address
     NovaXR_SPI.transfer(returned_bytes);        // returned bytes 
}



/**
 * @brief      Prints a board id.
 *
 * @param[in]  id    The NovaXR board identifier
 */
void print_boardID(uint8_t id){
    if (id == MAIN_BOARD ){
       Serial.println(" Main Board");
    }else if(id == SISTER_BOARD){
       Serial.println(" Sister Board");
    }else{
       Serial.println("ERROR: Unknown Board");
    }
}



/**
 * @brief      Apply Configuration to an ADS1299 Register.
 *
 * @param[in]  index       The index of the 8bit Register to change
 * @param[in]  target_Reg  The values of the target ADS1299 register
 *
 * @return     the changed Configuration Byte 
 */
byte apply_bit_Config(uint8_t index, byte target_Reg){
     byte Config = flipBit(index,readRegisterSettings(target_Reg));
     config_ADS_Register(target_Reg,Config);
     return Config;
}



/**
 * @brief      Sets to 1 a bit in a given 8bit register.
 *
 * @param[in]  index     The index of the 8bit Register to change
 * @param[in]  Register  The values of the target ADS1299 register
 *
 * @return     outPut -> returns the output
 */
byte setBit1(uint8_t index, byte Register){
    byte outPut = Register | (1<<index);
    return outPut;
}



/**
 * @brief      Set to 0 a bit in a given register.
 *
 * @param[in]  index     The index of the 8bit Register to change
 * @param[in]  Register  The values of the target ADS1299 register
 *
 * @return     outPut -> returns the output
 */
byte setBit0(uint8_t index, byte Register){
    byte outPut = Register & ~(1<<index);
    return outPut;
}



/**
 * @brief      Flips a bit state in a given register.
 *
 * @param[in]  index     The index of the 8bit Register to change
 * @param[in]  Register  The values of the target ADS1299 register
 *
 * @return     outPut -> returns the changed byte
 */
byte flipBit(uint8_t index, byte Register){
    byte outPut = Register ^ (1<<index);
    return outPut;
}



/**
 * @brief      Build your ASD1299 Configuration List and Apply it
 *
 * @param      ads_id  The ADS1299 Board identifier
 */
void ADS_Init_Config(uint8_t ads_id){
   
   enable_ADS1299(ads_id); // Enable ADS1299 Device
   delay(500);
   
   select_ADC(ads_id);  // Switch to the corresponed ADS1299 Device
   delay(500);

   // ---- Reset ADS1299 if it is present on the baord ------
   Reset_ADC();
   delay(1000);  //delay to ensure connection again
   
   if(!ADS_connected()){ // Check if device is attached to the board correctly 
        
        // ---- Perform ADS1299 Initial config ----        
        config_ADS_Register(CONFIG1,0xB0);   /// CONFIG1 Sampling Frequency: F0 16K, F1 8K, F2 4K,F3 2K,F4 1K, F5 500 250 ,F6 250 2250ms// 8Khz  - > 5.2Khz -  125 micros period
        config_ADS_Register(CONFIG2,0x04);   /// CONFIG2 Register: 00000100
        config_ADS_Register(CONFIG3,0xEC);   /// CONFIG3 Register: 0xEC , F6 . 
        config_ADS_Register(CONFIG4,0x00);   /// CONFIG3 Register: 
        config_ADS_Register(MISC1,0x00);     /// MISC1   Register: 0x20 // close SRB1 swtich 
        config_ADS_Register(GPIO,0x00);      /// GPIO    Register:  
        delay(10);
              
        config_All_ADS_Inputs(CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);   // Active channels
        delay(10);
        
        config_ADS_Register(0x15,0x20);   //SRB1 To Negative Inputs
        delay(10);
        
        #ifdef DEBUG
          display_Registers_settings();  
        #endif
        //CHECK_Indication();
   }else{
     //ERROR_Indication();
     Serial.print("Problem with");
     print_boardID(ads_id);
   } 
    
}



/**
 * @brief      Interrupt Routine for ADS1299 of the Mainboard
 *
 * @param[in]  ads_id    The ADS1299m board identifier
 * @param[in]  duration  The duration of the sampling period using interrupts
 */
void ADS_MB_Interrupt(uint8_t ads_id, long duration){
  
    select_ADC(ads_id);
    startADC_Continuous_Mode();
    long timeStart  = micros();

    // ------ Print as many as possible results -------
    while(duration > (micros()-timeStart)){ 
        for(int i=0; i<27; i++) { 
          Serial.print(MBdataADC.rawData[i],HEX); 
          Serial.print(" ");
        }
        Serial.println();     
     }
    
    ADS_Stop_Conversion();
    stop_ADC_interrupt();
    long timeEnd = micros();
    Serial.print("Duration in Micros : ");Serial.println(timeEnd-timeStart);
    Serial.println("END");

    while(1); // end loop and wait here forever
 
}



/**
 * @brief     ADS1299 example that prints RAW values in  single conversion mode
 */
void ADS_Example_2_Single_Shot(void ){
  char ADS_num ='0';   
  Serial.println(" Type '1' for Main Board Data, '2' for Sister Board data");   
  while(1){
    
    if(Serial.available()>1){ 
          ADS_Stop_Conversion();
          ADS_num = Serial.read();
      }
    
    if(ADS_num=='1'){
       Serial.println("ADS1299 -  Main Board");  
       select_ADC(MAIN_BOARD);  
              
    }else if(ADS_num=='2'){
       Serial.println("ADS1299 -  Sister Board");
       select_ADC(SISTER_BOARD);
         
    }

   if((ADS_num=='1')||(ADS_num=='2')) {
      if(print_Single_Shot()){
        Serial.println("ERROR: ADS1299 Disconnected");
        break;
      }
       
   }
 
  delay(100);
}}





/**
 * @brief     ADS1299 example: Sampling Frequency Test 
 */
void ADS_Example_3_Single_Shot_Freq(void){
  char ADS_num ='0';   
  Serial.println(" Type '1' for Main Board Data, '2' for Sister Board data");   
  while(1){
    
    if(Serial.available()>1){ 
         ADS_Stop_Conversion();
         ADS_num = Serial.read();
      }
    
    if(ADS_num =='1'){
       Serial.println("ADS1299 -  Main Board");  
       select_ADC(MAIN_BOARD); 
              
    }else if(ADS_num=='2'){
       Serial.println("ADS1299 -  Sister Board");
       select_ADC(SISTER_BOARD);    
      
    }

   if((ADS_num=='1')||(ADS_num=='2')) {
       /*
        if(ADS_num =='a'){ ADS_Stop_Conversion(); config_ADS_Register(CONFIG1, 0xB0);  Serial.println("Sampling Frequency 16KHz");}// 16KHz
        if(ADS_num =='b'){ ADS_Stop_Conversion(); config_ADS_Register(CONFIG1, 0xB1);  Serial.println("Sampling Frequency 8KHz"); }// 8KHz
        if(ADS_num =='c'){ ADS_Stop_Conversion(); config_ADS_Register(CONFIG1, 0xB2);  Serial.println("Sampling Frequency 4KHz"); }// 4KHz
        if(ADS_num =='d'){ ADS_Stop_Conversion(); config_ADS_Register(CONFIG1, 0xB3);  Serial.println("Sampling Frequency 2KHz"); }// 2KHz
        if(ADS_num =='e'){ ADS_Stop_Conversion(); config_ADS_Register(CONFIG1, 0xB4);  Serial.println("Sampling Frequency 1KHz"); }// 1KHz
        if(ADS_num =='f'){ ADS_Stop_Conversion(); config_ADS_Register(CONFIG1, 0xB5);  Serial.println("Sampling Frequency 500Hz");}// 500Hz
        if(ADS_num =='g'){ ADS_Stop_Conversion(); config_ADS_Register(CONFIG1, 0xB6);  Serial.println("Sampling Frequency 250Hz");}// 250Hz
       */
      if(print_Single_Shot()){
        Serial.println("ERROR: ADS1299 Disconnected");
        break;
      }
       
   }
 
  delay(100);
}}



/**
 * @brief      Prints a single shot RAW data.
 *
 * @return     returns error if something goes wrong with ADS1299 connection
 */
bool print_Single_Shot(void){         
      bool error =false;
      long a = micros();
      get_ADC_Data_SS(&SBdataADC); // single readback            
      long b = micros();
      for(uint8_t r=0; r<27; r++){
       // Serial.print(SBdataADC.rawData[r],HEX);Serial.print(",");
        if(SBdataADC.rawData[0]==0xFF){
          error = true;
          break;
         }    
       }
      Serial.println();
      Serial.print("SS Duration: "); Serial.println(b-a);
      return error;    
}



// ------------------------------------- End ----------------------------------------
