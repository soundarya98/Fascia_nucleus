/*
 * 
 * Author:       ioannis Smanis
 * Library for:  ADS1299 ADC chip
 * Project:      NovaXR - MKR1010
 * Date:         10/3/2019
 * 
 */

#include "ADS1299_lib.h"
#include "wiring_private.h"
#include <SPI.h>

#define DEBUG
#define BOARD_V1

// ------------------ Custom SPI Module for SAMD21XXX -------------------------
#define PIN_SPI_MISO PA19 
#define PIN_SPI_SCK  PA17 
#define PIN_SPI_MOSI PA16

SPIClass NovaXR_SPI (&sercom1, PIN_SPI_MISO, PIN_SPI_SCK, PIN_SPI_MOSI, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);  

// --- for any ADS1299 -----------
uint8_t ADS_CS_pin;                // Pin Variable initialization 
uint8_t ADS_DRDY_pin;              // Pin Variable initialization 
boolean continuousRead = false;    // Flag warning for continuous conversion is happening



// --- Store Captured Data here ---
extern adc_data ADC1_data;
extern adc_data ADC2_data;
extern adc_data dataBuffer[130];

// --- Internal declarations -------
uint16_t SampleID1=0; 
uint16_t SampleID2=0; 

long time1[10];  // for testing purpose 





// ======================== Constructors =============================

// -------- Start SPI protocol ---------------------------------------
void Begin_ADS(void){
   NovaXR_SPI.begin();
   SPI_pinPeriphernal_init();
   digitalWrite(SisCS, HIGH);
   digitalWrite(MainCS, HIGH);
}

// -------- initialize Pin for SERCOM --------------
void SPI_pinPeriphernal_init(void){
    pinMode(MainDRDY, INPUT);
    pinMode(SisDRDY , INPUT);
    pinMode(SisCS   ,OUTPUT);
    pinMode(MainCS  ,OUTPUT);
    pinPeripheral(PIN_SPI_MISO, PIO_SERCOM);
    pinPeripheral(PIN_SPI_SCK,  PIO_SERCOM);
    pinPeripheral(PIN_SPI_MOSI, PIO_SERCOM);
}


// ------- Reset ADS1299 chip ---------------------- 
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


// ------------- Select ADC to handle -----------------------------------
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






// ------ Read Settings of a Single Register --------------------------
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


// ---------- Display Current Registers Settings -----------------------------
void display_Registers_settings(void){
   Serial.print("ADS1299 -> DRDY: "); Serial.println(ADS_DRDY_pin);
   Serial.print("ADS1299 ->   CS: "); Serial.println(ADS_CS_pin);
   Serial.println("----------------- ADS1299 Registers Settings ------------------");
   for(uint8_t address =0; address<24; address++){  
       Serial.print("Register #: 0x");  SerialUSB.print(address,HEX); 
       Serial.print(" \t");
       Serial.print("Config Bits: 0x"); SerialUSB.print(readRegisterSettings(address),HEX);
       Serial.print(" \t");
       Serial.print("Bin: 0b");         SerialUSB.print(readRegisterSettings(address),BIN);
       Serial.println();
   }
   Serial.println("----------------------------------------------------------------");
}



// ----------- configure an ADS Register -------------------------------------------------------
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



// ----------- Configure all ADC Inputs at once -------------------------------------------------
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


// ---------- Check id ADS1299 device is present on the board -----------------------------------
bool ADS_connected(void){
   bool error=false;
   byte data_id = readRegisterSettings(ID_REG);  
   //Serial.print("data: ");Serial.println(data_id ,HEX);
   if (data_id == 0xFF){
     error=true;
   }else if (data_id == ADS_ID){
     error=false;
   }else{
     error=true;
     Serial.println("UKNOWN ERROR");
   }
  return error;
}



// ----------- Configure a single ADC Input  ----------------------------------------------------
void config_ADS_Input(uint8_t channel, byte data){
      uint8_t Input[] = {5,6,7,8,9,10,11,12};
      digitalWrite(ADS_CS_pin, LOW);
        NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
        NovaXR_SPI.transfer(SDATAC);
        do_WREG(Input[channel],data);
        NovaXR_SPI.endTransaction();
      digitalWrite(ADS_CS_pin, HIGH);      
}


// ------------ Start ADS1299 in Continuous Conversion Mode ---------------------
void start_ADC_Continuous_Mode(void){

  if(!continuousRead){
     continuousRead = true ; 
     digitalWrite(ADS_CS_pin, LOW);
       NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1)); 
       NovaXR_SPI.transfer(START);
       NovaXR_SPI.transfer(RDATAC);
     digitalWrite(ADS_CS_pin, HIGH);
 
     // -- enable Hardware Interrupt 
     attachInterrupt(digitalPinToInterrupt(MainDRDY), get_ADC1_Data_CM, LOW); // ADC1 DATA - Change it when you have 2 ADS1299 connected
  } else{
    Serial.println("WARNING: you are already in Continuous Mode");
  }
}




// ------------ Stop Being interrupted by ADS1299 -----------------------------
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



// ------------ Stop Continuous Conversion ------------------------------------
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


// ------------ Stop ADC Conversion Operation ---------------------------------
void ADS_Stop_Conversion(void){
      digitalWrite(ADS_CS_pin, LOW);
      NovaXR_SPI.transfer(STOP);
      digitalWrite(ADS_CS_pin, HIGH);   
}

// ------------ Stop Streaming, Config and Restart ----------------------------
void stop_and_configInput(uint8_t channel, byte data){ 
      ADS_Stop_Conversion();
      continuousRead=false;
      config_ADS_Input(channel,data);
      continuousRead=true;
}

// ---test----- For ADC1 on mainbaord - Interrupt Routine to Get Data in shared Union Structure: "ADC_data"  ----------------------
void get_ADC1_Data_CM(void){  // Continuous Mode get data via interrupt
   time1[SampleID1]=micros(); // for Benchmarking purpose
  
   digitalWrite(ADS_CS_pin, LOW);
     for(uint8_t i=0;i<27;i++){   // get all pushed 216Bits data from ADS1299
       byte temp = NovaXR_SPI.transfer(0x00);
       ADC1_data.rawData[i]= temp;
     }
   digitalWrite(ADS_CS_pin,HIGH); 
   //fill_PktBuff(&ADC1_data,SampleID1);
   SampleID1++; 
}


// ---test-----  For ADC2 on mainbaord - Interrupt Routine to Get Data in shared Union Structure: "ADC_data"  ----------------------
void get_ADC2_Data_CM(void){  // Continuous Mode get data via interrupt
   time1[SampleID1]=micros(); // for Benchmarking purpose
  
   digitalWrite(ADS_CS_pin, LOW);
     for(uint8_t i=0;i<27;i++){   // get all pushed 216Bits data from ADS1299
       byte temp = NovaXR_SPI.transfer(0x00);
       ADC2_data.rawData[i]= temp;
     }
   digitalWrite(ADS_CS_pin,HIGH);  
   //fill_PktBuff(&ADC2_data,SampleID2);
   SampleID2++; 
}



// ---------- Single Shot Mode - Get Data in shared Union Structure: "ADC_data"  ----------------------
void get_ADC_Data_SS(adc_data * adc){  // Single Shot mode get data via interrupt   
   digitalWrite(ADS_CS_pin, LOW); 
     NovaXR_SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));        
     NovaXR_SPI.transfer(START);
     NovaXR_SPI.transfer(RDATA);  // set single shot data read
     
     #ifndef BOARD_V1
        while(digitalRead(ADS_DRDY_pin));
     #endif  
     
     for(uint8_t i=0;i<27;i++){   // get all pushed 216Bits data from ADS1299
       byte rawByte = NovaXR_SPI.transfer(0x00);
        adc->rawData[i]= rawByte;
     }
   digitalWrite(ADS_CS_pin,HIGH);
}




// -------- Perform WREG command -------------------------------------
void do_WREG (byte Register,byte Data){
      NovaXR_SPI.transfer(WREG|Register);  // WREG
      NovaXR_SPI.transfer(0x00); 
      NovaXR_SPI.transfer(Data);
}

// -------- Perform RREG command -------------------------------------
void do_RREG ( byte register_Address, byte returned_bytes){
      NovaXR_SPI.transfer(RREG|register_Address); // RREG | Register Address
      NovaXR_SPI.transfer(returned_bytes);        // returned bytes 
}

// -------- Print Board ID -----------------------------------------
void print_boardID(uint8_t id){
  if (id == MAIN_BOARD ){
     Serial.println(" Main Board");
  }else if(id == SISTER_BOARD){
     Serial.println(" Sister Board");
  }
}

// ---------- Get Configured Register ----------------------------------
byte apply_bit_Config(uint8_t index, byte target_Reg){
   byte Config = flipBit(index,readRegisterSettings(target_Reg));
   config_ADS_Register(target_Reg,Config);
   return Config;
}


// ---------- set to 1 a bit in a given register ---------------------
byte setBit1(uint8_t index, byte Register){
  byte outPut = Register | (1<<index);
  return outPut;
}

// ---------- set to 0 a bit in a given register ---------------------
byte setBit0(uint8_t index, byte Register){
  byte outPut = Register & ~(1<<index);
  return outPut;
}

// ---------- flip a bit in a given register ---------------------
byte flipBit(uint8_t index, byte Register){
  byte outPut = Register ^ (1<<index);
  return outPut;
}



// -------- Build your ASD1299 Configuration List -------
void ADS_Init_Config(uint8_t ads_id){
  
   select_ADC(ads_id);
   delay(1000);
   
   // ---- Reset ADS1299 if it is present omn the baord ------
   Reset_ADC();
   delay(1000);  //delay to ensure connection again
   
   if(!ADS_connected()){ // Check if device is attached to the board correctly 
        
        // ---- Perform ADS config ----
        config_ADS_Register(0x03,0xE8);   // 0xE0 Enable Internal Reference Buffer 6 -OFF , E - ON 
        delay(100);
        config_All_ADS_Inputs(0x01);
        delay(10);
        //config_ADS_Input(1,0x02);       // Config ADC Input #1  
        config_ADS_Register(0x01,0xF0);   // Sample Rate F0 16K,F1 8K,F2 4K,F3 2K,F4 1K, F5 500 ,F6 250 // 8Khz  - > 5.2Khz -  125 micros period 
        delay(10);
              
        // config_ADS_Register(0x02,0xD1);  // Test Signal 2Hz Square Wave 
        // config_ADS_Register(CHn,0x00);   // CHn on Test Signals with Gain 0 
        config_All_ADS_Inputs(0x10);        // Active channels
        delay(10);
        
        config_ADS_Register(0x15,0x20);   //SRB1 To Negative Inputs
        delay(10);
        
        #ifdef DEBUG
          display_Registers_settings();  
        #endif
   }else{
    Serial.print("Problem with");
    print_boardID(ads_id);
   } 
    
}




void ADS_Example_1_Interrupt(uint8_t ads_id, long duration){
  
    select_ADC(ads_id);
    start_ADC_Continuous_Mode();
 
    long timeStart = micros();

   //  ----- Print as many as possible results --------
    while(duration > (micros()-timeStart)){ 
        for(int i=0; i<27; i++) { 
          Serial.print(ADC2_data.rawData[i],HEX); 
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





// ------ ADS1299 exaple that prints RAW values in  single conversion mode --------
void ADS_Example_2_Single_Shot(void ){
  char ADS_num ='0';      
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





// ------ Print RAW data ------------
bool print_Single_Shot(void){
      bool error =false;
      long a = micros();
      //Serial.println(a);
      get_ADC_Data_SS(&ADC2_data); // single readback            
      long b = micros();
      for(uint8_t r=0; r<27; r++){
        Serial.print(ADC2_data.rawData[r],HEX);Serial.print(",");
        if(ADC2_data.rawData[0]==0xFF){
          error = true;
          break;
         }    
       }
      Serial.println();
      // Serial.print("SS Duration: "); Serial.println(b-a);
      return error;    
}



// ------------------------------------- End ----------------------------------------
