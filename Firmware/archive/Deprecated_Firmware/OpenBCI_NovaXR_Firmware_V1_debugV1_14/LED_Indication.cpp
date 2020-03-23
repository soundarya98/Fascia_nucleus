
#include "LED_Indication.h"
#include <WiFiNINA.h>
 #include <utility/wifi_drv.h>

 
// ------------- Pin Assignment -----------------------
const uint8_t redLED_Pin    = LED_RED;
const uint8_t greenLED_Pin  = LED_GREEN;
const uint8_t blueLED_Pin   = LED_BLUE;

// ------- Declare WiFi UBlox Driver Class ------------
WiFiDrv uBLox; ///< Instance of the LED Driver Class 


// ========== Method Constractors ========================
/**
 * @brief      Initialize GPIOs of UBlox Module
 */
void uBlox_HW_Init(void){
    uBLox.pinMode(greenLED_Pin,   OUTPUT); 
    uBLox.pinMode(blueLED_Pin,    OUTPUT);
    uBLox.pinMode(redLED_Pin,     OUTPUT);
}


/**
 * @brief      Indicate a Hardware Issue or Stall
 */
void ERROR_Indication(void){
  while(1) blink_Red(2,1);
}


/**
 * @brief      Indicate a correct Operation
 */
void CHECK_Indication(void){
    BlinkLED(greenLED_Pin, 1);  // --- blink with 1ms period
    turnOffLEDs();
}


/**
 * @brief      Blink Blue Once as somehting is being in progress
 */
void inProgress_Indication(void){
    Blink_DualColor(blueLED_Pin, redLED_Pin, 1);
    turnOffLEDs(); 
}


/**
 * @brief      Blink Blue Once as somehting is being in progress
 */
void WiFi_conn_inProgress_Indication(void){
    Blink_DualColor(greenLED_Pin, redLED_Pin, 3);
    turnOffLEDs(); 
}


/**
 * @brief      Indicate Successful State
 */
void success_State(void){
     turnOffLEDs();
     uBLox.analogWrite(greenLED_Pin, 50);    
}


/**
 * @brief      Shows the stand by state.
 */
void show_standBy_State(void){
     turnOffLEDs();
     uBLox.analogWrite(blueLED_Pin, 50);    
}


/**
 * @brief      Shows the streamming state.
 */
void show_streamming_State(void){
     turnOffLEDs();
     uBLox.analogWrite(greenLED_Pin, 50);    
}


/**
 * @brief    Unexpected TCP Client Disonnection
 */
void diconnected_Client_Indication(void){
    turnOffLEDs();
    uBLox.analogWrite(redLED_Pin, 50); 
}


/**
 * @brief  Enter StandBy Mode
 */
void standBy_mode(void){
   turnOffLEDs();
   turnOnLED(blueLED_Pin,1);
}


/**
 * @brief   Power On Device Indication
 */
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


/**
 * @brief    Fade-in effect with Red Color LED Indicator
 *
 * @param[in]  blink_num  Sets the number of blinks the LED will perform
 * @param[in]  Period     Sets the period of blinking frequency
 */
void blink_Red(uint8_t blink_num, uint16_t Period){
    for(uint8_t t=0; t<blink_num; t++){
      BlinkLED(redLED_Pin, Period);
    }    
}


/**
 * @brief    Blink LED at certain frequency
 *
 * @param[in]  led_pin  The pin that is assigned to the LED indicator
 * @param[in]  period   Sets the period of blinking frequency
 */
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


/**
 * @brief    Blink bi-coloerd LED at certain frequency
 *
 * @param[in]  led1_pin  The pin 1 that is assigned to LED color 1
 * @param[in]  led2_pin  The pin 2 that is assigned to LED color 2
 * @param[in]  period    Sets the period of blinking frequency
 */
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


/**
 * @brief   Fade-in Turning On any LED at certain PWM speed
 *
 * @param[in]  lep_pin    The pin that is assigned to the LED indicator
 * @param[in]  stepDelay  The step delay that controls the speed of lighting up the LED
 */
void turnOnLED(const uint8_t lep_pin, uint8_t stepDelay){
    for(uint8_t i=0; i<20; i++){
      uBLox.analogWrite(lep_pin, i);
      delay(stepDelay);
    }
}


/**
 * @brief   Turn OFF all LEDs immidiately
 */
void turnOffLEDs(void){
      uBLox.analogWrite(redLED_Pin,   0);
      uBLox.analogWrite(greenLED_Pin, 0);
      uBLox.analogWrite(blueLED_Pin,  0);
}


/**
 * @brief    Fade-in Turning On Purple Color
 */
void turnOnPurple(void){
    for(uint8_t i=0; i<255; i++){
      uBLox.analogWrite(redLED_Pin, i);
      uBLox.analogWrite(blueLED_Pin, i);
      delay(5);
    }

}


/**
 * @brief   Fade-in Turning On Yellow Color
 */
void turnOnYellow(void){
    for(uint8_t i=0; i<255; i++){
      uBLox.analogWrite(redLED_Pin, i);
      uBLox.analogWrite(greenLED_Pin, i);
      delay(5);
    }
}


/**
 * @brief   Fade-in Turning On White Color
 */
void turnOnWhite(void){

    for(uint8_t i=0; i<255; i++){
      uBLox.analogWrite(redLED_Pin, i);
      uBLox.analogWrite(greenLED_Pin, i);
      uBLox.analogWrite(blueLED_Pin, i);
      delay(5);
    }
}
