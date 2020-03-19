/*
 * Author:       Ioannis Smanis
 * Library Ver.: 0.5
 * Library for:  PPG sensor
 * Project:      NovaXR
 * Date:         11/28/2019
 * Module:       NovaXR_PPG_Sensor.cpp
 */
 
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "NovaXR_PPG_Sensor.h"


//#define debug_presence
//#define debug_temp
//#define debug_HR
#define plotIR


// ----- Object Initialization -------------------------
MAX30105 particleSensor;

// ----- Global variables and defines ------------------
long samplesTaken = 0; // Counter for calculating the Hz or read rate
long unblockedValue;   // Average IR at power up
long startTime;        // Used to calculate measurement rate

// -- HeartRate variables
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte  rates[RATE_SIZE]; //Array of heart rates
byte  rateSpot = 0;
long  lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int   beatAvg;

 // Setup to sense stable HR values
 byte ledBrightness =   0; //Options: 0=Off to 255=50mA
 byte sampleAverage =   0; //Options: 1, 2, 4, 8, 16, 32
 byte ledMode =         0; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
 int sampleRate=        0; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
 int pulseWidth =       0; //Options: 69, 118, 215, 411
 int adcRange =         0; //Options: 2048, 4096, 8192, 16384




// --------------- Configure PPG sensor for HR and Skin Temp Sense ----------------------
void PPG_Init(void){

    // Initialize sensor
   if (particleSensor.begin(Wire, I2C_SUPER_FAST ) == false){ //Use default I2C port, 800kHz speed
        #ifdef debug_presence  
          Serial.println("MAX30105 was not found. Please check wiring/power. ");
        #endif 
        while (1);
    }
    // Apply Sensoer Configuration Set #4
    particleSensor.setPulseAmplitudeRed(0x0A);
    delay(500);
    HR_config(); 
}



// --------------- Configure PPG sensor for Presence Sense ---------------------------
void PPG_Presence_Sense_Init(void){

    // Initialize sensor
    if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false){ //Use default I2C port, 400kHz speed
        #ifdef debug_presence  
          Serial.println("MAX30105 was not found. Please check wiring/power. ");
        #endif      
        while (1);
    }
    // Set Sensor Control Registers for Presence Sense
    HR_config();
    
    //Take an average of IR readings at power up
    unblockedValue = 0;
    for (byte x = 0 ; x < 32 ; x++){
       unblockedValue += particleSensor.getIR(); //Read the IR value
    }
    unblockedValue /= 32;
    startTime = millis();

}



// --------------- Get NovaXR headset "worn" or "not worn"  status ---------------------------
bool getPresence(void){
      bool presence = false;
       
      #ifdef debug_presence  // IR Reflection Values and Sampling Rate
          samplesTaken++;
          Serial.print("IR["); Serial.print(particleSensor.getIR());Serial.print("] Hz[");
          Serial.print((float)samplesTaken / ((millis() - startTime) / 1000.0), 2);
          Serial.print("]");
      #endif

      // Calculate the current Delta signal difference from unblocked to blocked sensor
      long currentDelta = particleSensor.getIR() - unblockedValue;

       #ifdef debug_presence 
          Serial.print(" delta[");Serial.print(currentDelta);Serial.print("]");
       #endif

      // Check If soemthing is blocking the PPG sensor
      if (currentDelta > (long)50000) {
        #ifdef debug_presence     
           Serial.print(" User is Present !!");
        #endif
        presence = true;
      }

    return presence;
}



// --------------- Get User Skin Temperature while he worns the NovaXR headset --------------------
float get_Skin_Temp(bool CelciousFormat){
   
     if(CelciousFormat){ // -- If user selected Celcious temperature format
         float C_temp = particleSensor.readTemperature();
         #ifdef debug_temp
            Serial.print(" temp_Celcious= ");Serial.print(C_temp, 4);
         #endif
         return C_temp;
         
     }else{              // -- If user selected Fahrenheit temperature format
         float F_temp = particleSensor.readTemperatureF(); 
         #ifdef debug_temp
            Serial.print(" temp_Fahrenheit= ");Serial.print(F_temp, 4);
         #endif
         return F_temp;
     }    
}



// --------------- Get Heart Rate ------------------------------------------------------------------
uint8_t get_Heart_Rate(void) {
    long RAW_Sensor =0;
    uint16_t RAW16=0;
    
    // get IR value
    RAW_Sensor = particleSensor.getIR(); // 19ms delay
    RAW16 = RAW16|RAW_Sensor;
    
    // calculate the average HR
    uint8_t avg_beats_min = get_calculated_HR(RAW16);
    return avg_beats_min;
}


// --------------- Get RAW IR Reflection -----------------------------------------------------------
uint16_t get_RAW_IR_Reflection(void) {
    uint16_t RAW_IR=0;
    
    // get IR value
    long RAW_Sensor = particleSensor.getIR(); // 19ms delay
    RAW_IR = RAW_IR|RAW_Sensor;
    return RAW_IR;
}



// -------------- Calcualte the average Heart Beat based on reflective IR Value --------------------
uint8_t get_calculated_HR(long IR_Value){

    if (checkForBeat(IR_Value) == true) {
       //We sensed a beat!
       long delta = millis() - lastBeat;
       lastBeat = millis();
       beatsPerMinute = 60/(delta/1000.0);
  
      if(beatsPerMinute < 255 && beatsPerMinute > 20){
          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
          rateSpot %= RATE_SIZE; //Wrap variable

          //Take average of readings
          beatAvg = 0;
          for (byte x = 0 ; x < RATE_SIZE ; x++) beatAvg += rates[x];
          beatAvg /= RATE_SIZE;
      }
    }
         
    #ifdef debug_HR
      Serial.print("RAW_IR= ");Serial.print(IR_Value);
      Serial.print(" ,BPM= ");Serial.print(beatsPerMinute);
      Serial.print(" ,Avg BPM= ");Serial.println(beatAvg);
    #endif
  
    if (IR_Value < 50000){
       #ifndef plotIR
          Serial.println(" No finger?");
       #endif
    }

    return beatAvg; // beatAvg;
  
}



// --------------- Default MAX30105 Control Registers Configuration --------------------------------
void HR_config_Default(void){
    particleSensor.setup(); //Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  
}



// --------------- Predefined MAX30105 Control Registers Configuration 1--------------------------------
void HR_config(void){

    // Setup to sense stable HR values
    ledBrightness =  30; //Options: 0=Off to 255=50mA
    sampleAverage =   1; //Options: 1, 2, 4, 8, 16, 32
    ledMode =         2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    sampleRate=     800; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    pulseWidth =    118; //Options: 69, 118, 215, 411
    adcRange =     2048; //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
    particleSensor.enableDIETEMPRDY();      //Enable the temp ready interrupt. This is required.
    particleSensor.setPulseAmplitudeRed(0); //Turn off Red LED
}



// --------------- Predefined MAX30105 Control Registers Configuration 2--------------------------------
void HR_config_4(void){

      // Setup to sense stable HR values
   byte ledBrightness =  33; //Options: 0=Off to 255=50mA
   byte sampleAverage =   8; //Options: 1, 2, 4, 8, 16, 32
   byte ledMode =         2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
   int sampleRate=      400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
   int pulseWidth =     411; //Options: 69, 118, 215, 411
   int adcRange =      8192; //Options: 2048, 4096, 8192, 16384
  
}



// --------------- MAX30105 Control Registers Configuration for Presence sense -------------------------
void PPG_Presence_Config(void){

    //Setup to sense up to 18 inches, max LED brightness
    ledBrightness = 0xFF; //Options: 0=Off to 255=50mA
    sampleAverage = 4;    //Options: 1, 2, 4, 8, 16, 32
    ledMode       = 2;    //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    sampleRate     = 400;  //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    pulseWidth     = 411;  //Options: 69, 118, 215, 411
    adcRange       = 2048; //Options: 2048, 4096, 8192, 16384

    //Configure sensor with these settings
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
    particleSensor.setPulseAmplitudeRed(0); //Turn off Red LED
    // particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
    
}



// --------------- MAX30105 Control Registers Configuration: Enable Skin Temperature Sense ------------
void PPG_Temp_Config(void){

    //The LEDs are very low power and won't affect the temp reading much but
    //you may want to turn off the LEDs to avoid any local heating
    particleSensor.setup(0);  //Configure sensor. Turn off LEDs  
    particleSensor.enableDIETEMPRDY(); //Enable the temp ready interrupt. This is required.

}



// --------------- MAX30105 Control Registers Configuration: Enable FIFO and Register Interrupt ------------
void En_Int_FIFO_config(void){

    particleSensor.enableAFULL(); //Enable the almost full interrupt (default is 32 samples)
    particleSensor.setFIFOAlmostFull(3); //Set almost full int to fire at 29 samples

}



// --------------- Configure PPG sensor for Temperature Sense ---------------------------
void PPG_Temp_Sense_Init(void){

      // Initialize sensor
      if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false){ //Use default I2C port, 400kHz speed
      
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1);
      }
      // Set Sensor Control Registers for Temperature Sense
      PPG_Temp_Config();
}



// --------------- Configure PPG sensor for HeartRate Sense ---------------------------
void PPG_HR_Sense_Init(void){

    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)){ //Use default I2C port, 400kHz speed
      Serial.println("MAX30105 was not found. Please check wiring/power. ");
      while (1);
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");
    // Set Sensor Control Registers for HR sense
    HR_config_Default();
    
}



// ------------- Configure PPG Sensor in Real Time -------------------------------------
void adjust_configs(uint16_t configVal, char factor){

   switch(factor){
      case 'A':
        ledBrightness = configVal;
        break;
        
      case 'B':
        sampleAverage = configVal;
        break;
        
      case 'C':
        ledMode  = configVal;
        break;
        
      case 'D':
        sampleRate = configVal;
        break;
        
      case 'E':
        pulseWidth = configVal;
        break;
        
      case 'F':
        adcRange = configVal;
        break;
        
      default:
        break;
   }

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
    printPPG_Configs();
   
}



// ------------- Print PPG Current Configs -------------------------------------------
void printPPG_Configs(void){

    Serial.println(" ----- PPG Configs ------- ");
    Serial.print("  A. ledBrightness: ");Serial.println(ledBrightness);
    Serial.print("  B. sampleAverage: ");Serial.println(sampleAverage);
    Serial.print("  C. ledMode: ");      Serial.println(ledMode);
    Serial.print("  D. sampleRate: ");   Serial.println(sampleRate);
    Serial.print("  E. pulseWidth: ");   Serial.println(pulseWidth);
    Serial.print("  F. adcRange: ");     Serial.println(adcRange);
    Serial.println(" ------------------------- ");
    
}


// ------------- Read Incomming String from serial monitor and return integer ------------
uint16_t read_return_int(void){
      int number =0;
      String inString = "";
      int inChar =0;
      while(Serial.available()> 0){
        char cmd = inChar = Serial.read();
        if(cmd=='z'){
            printPPG_Configs();
            delay(1500);
        }
        
        if (isDigit(inChar)){
          inString += (char)inChar;                 
         }      
             
        if (inChar =='\n'){      
            number = inString.toInt();
            inString = "";
        }
     }
     
    return number;
}



// ------------- Sensor Calibration Process: adjust PPG configs in real time -----------
void RealTime_Sensor_Calibration(){

    char inCMD=0;
    if(Serial.available() > 0) {
        
       // read the incoming byte:
       inCMD = Serial.read();
       if(inCMD =='A'){
          // ---- Brightness -------------------
          Serial.println("\nA >>> Led Brightness"); 
          Serial.println("Options: 0=Off to 255=50mA"); 
          inCMD=0; // Reset incoming Command 
          Config_and_Calibrate(1,255,'A');
    
       }else if(inCMD =='B'){
          Serial.println("\n B >>> SampleAverage");
          Serial.println("Options: 1, 2, 4, 8, 16, 32");
          inCMD=0; // Reset incoming Command      
          Config_and_Calibrate(1,32,'B');

          
       }else if(inCMD =='C'){
          Serial.println("\nC >>> ledMode");
          Serial.println("Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green");
          inCMD=0; // Reset incoming Command      
          Config_and_Calibrate(1,3,'C');
              
       }else if(inCMD =='D'){
         Serial.println("\nD >>> sampleRate");
         Serial.println("Options: 50, 100, 200, 400, 800, 1000, 1600, 3200");
         inCMD=0; // Reset incoming Command      
         Config_and_Calibrate(50,3200,'D');
          
       }else if(inCMD =='E'){
         Serial.println("\nE >>> pulseWidth");
         Serial.println("Options: 69, 118, 215, 411");
         inCMD=0; // Reset incoming Command      
         Config_and_Calibrate(69,411,'E');
          
       }else if(inCMD =='F'){
         Serial.println("\nF >>> adcRange");
         Serial.println("Options: 2048, 4096, 8192, 16384");
         inCMD=0; // Reset incoming Command      
         Config_and_Calibrate(2048,16384,'F');
      }
       
  }
     
}



// --------- Print PPG Config Options ------------------------------------
void printInfo(void){
        Serial.println(" |============== Adjust Configuration Factor for PPG Sensor ======================|\n");
        Serial.println("    A. ledBrightness - Options: 0=Off to 255=50mA");
        Serial.println("    B. sampleAverage - Options: 1, 2, 4, 8, 16, 32");
        Serial.println("    C. ledMode =     - Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green");
        Serial.println("    D. sampleRate    - Options: 50, 100, 200, 400, 800, 1000, 1600, 3200");
        Serial.println("    E. pulseWidth    - Options: 69, 118, 215, 411");
        Serial.println("    F. adcRange      - Options: 2048, 4096, 8192, 16384\n");
        Serial.println(" |================================================================================|");  
        Serial.println("  1. --- Type a Letter for Configuration Factor and then a integer Configuration value ");    
        Serial.println("  2. --- Type a 'z' to print current configs - Type '99999' to stop reading"); 
}



// ---------- Apply Config values to the PPG Sensor -----------------------
void Config_and_Calibrate(uint16_t Min, uint16_t Max, char command){
     
    while(1){     
         if (Serial.available() > 0){
            uint16_t Config = read_return_int();   // trnaslate incoming Configuration value to a rela integer   
            Serial.println(Config); 
            if(Config){
               if((Config<=Max)&&(Config>=Min)){                               
                  adjust_configs(Config,command);           
               }else{
                  Serial.println("Stop");
                  break;
               }  
            }                                  
         }
         get_Heart_Rate();       
    }
    printPPG_Configs();
    
}
