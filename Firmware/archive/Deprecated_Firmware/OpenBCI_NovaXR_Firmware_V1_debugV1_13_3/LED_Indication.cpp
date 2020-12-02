
#include "LED_Indication.h"
#include <WiFiNINA.h>
 #include <utility/wifi_drv.h>

 
// ------------- Pin Assignment -----------------------
const uint8_t redLED_Pin    = LED_RED;
const uint8_t greenLED_Pin  = LED_GREEN;
const uint8_t blueLED_Pin   = LED_BLUE;

// ------- Declare WiFi UBlox Driver Class ------------
WiFiDrv uBLox;



// ========== Method Constractors ========================

// ---- Initialize GPIOs of UBlox Module -----------------
void uBlox_HW_Init(void){
    uBLox.pinMode(greenLED_Pin,   OUTPUT); 
    uBLox.pinMode(blueLED_Pin,    OUTPUT);
    uBLox.pinMode(redLED_Pin,     OUTPUT);
}


// ----- Indicate a Hardware Issue and Stall -------------
void ERROR_Indication(void){
  while(1) blink_Red(2,1);
}


// ----- Indicate a correct Operation   ------------------
void CHECK_Indication(void){
    BlinkLED(greenLED_Pin, 1);  // --- blink with 1ms period
    turnOffLEDs();
}


// ---- Blink Blue Once as somehting is being in progress -----
void inProgress_Indication(void){
    Blink_DualColor(blueLED_Pin, redLED_Pin, 1);
    turnOffLEDs(); 
}


// ---- Blink Blue Once as somehting is being in progress -----
void WiFi_conn_inProgress_Indication(void){
    Blink_DualColor(greenLED_Pin, redLED_Pin, 3);
    turnOffLEDs(); 
}


// -------- Indicate Successful State -------------------------
void success_State(void){
     turnOffLEDs();
     uBLox.analogWrite(greenLED_Pin, 50);    
}


// -------- StandBy State  ------------------------------------
void show_standBy_State(void){
     turnOffLEDs();
     uBLox.analogWrite(blueLED_Pin, 50);    
}


// -------- StandBy State  ------------------------------------
void show_streamming_State(void){
     turnOffLEDs();
     uBLox.analogWrite(greenLED_Pin, 50);    
}


// -------- Unexpected TCP Client Disonnection ----------------
void diconnected_Client_Indication(void){
    turnOffLEDs();
    uBLox.analogWrite(redLED_Pin, 50); 
}


// ----- Enter StandBy Mode ----------------------------------------
void standBy_mode(void){
   turnOffLEDs();
   turnOnLED(blueLED_Pin,1);
}


// --------- Power On Device Indication -----------------------
void powered_On_Indication(void){
 
    for(uint8_t t=0; t<1; t++){
      BlinkLED(redLED_Pin, 1);
    }
    turnOffLEDs();
    
    for(uint8_t t=0; t<1; t++){
      BlinkLED(greenLED_Pin, 1);
    }
    turnOffLEDs();
    
    for(uint8_t t=0; t<1; t++){
      BlinkLED(blueLED_Pin, 1);
    }
    turnOffLEDs();
}



// --------- fade-in Red Color LED Indicator ------------------
void blink_Red(uint8_t blink_num, uint16_t Period){
    for(uint8_t t=0; t<blink_num; t++){
      BlinkLED(redLED_Pin, Period);
    }    
}



// ------------- Blink LED at certain frequency -------------------
void BlinkLED(const uint8_t led_pin, uint16_t period){

    for(uint8_t i=0; i<80; i++){
      uBLox.analogWrite(led_pin, i);
      delay(period);
    } 
      
    for(uint8_t j=80; j>0; j--){
      uBLox.analogWrite(led_pin, j);
      delay(period);
    }
}


// ------------- Blink LED at certain frequency -------------------
void Blink_DualColor(const uint8_t led1_pin, const uint8_t led2_pin, uint16_t period){

    for(uint8_t i=0; i<50; i++){
      uBLox.analogWrite(led1_pin, i);
      uBLox.analogWrite(led2_pin, i);
      delay(period);
    } 
      
    for(uint8_t j=50; j>0; j--){
      uBLox.analogWrite(led1_pin, j);
      uBLox.analogWrite(led2_pin, j);
      delay(period);
    }
}


 
// ----- Fade-in Turning On any LED at certain PWM speed -----------
void turnOnLED(const uint8_t lep_pin, uint8_t stepDelay){
    for(uint8_t i=0; i<20; i++){
      uBLox.analogWrite(lep_pin, i);
      delay(stepDelay);
    }
}


// ----- Turn OFF all LEDs immidiately ---------------
void turnOffLEDs(void){
      uBLox.analogWrite(redLED_Pin,   0);
      uBLox.analogWrite(greenLED_Pin, 0);
      uBLox.analogWrite(blueLED_Pin,  0);
}


// ------- Fade-in Turning On Purple Color --------------
void turnOnPurple(void){
    for(uint8_t i=0; i<255; i++){
      uBLox.analogWrite(redLED_Pin, i);
      uBLox.analogWrite(blueLED_Pin, i);
      delay(5);
    }

}


// ------- Fade-in Turning On Yellow Color --------------
void turnOnYellow(void){
    for(uint8_t i=0; i<255; i++){
      uBLox.analogWrite(redLED_Pin, i);
      uBLox.analogWrite(greenLED_Pin, i);
      delay(5);
    }
}



// ------- Fade-in Turning On White Color --------------
void turnOnWhite(void){

    for(uint8_t i=0; i<255; i++){
      uBLox.analogWrite(redLED_Pin, i);
      uBLox.analogWrite(greenLED_Pin, i);
      uBLox.analogWrite(blueLED_Pin, i);
      delay(5);
    }
}
