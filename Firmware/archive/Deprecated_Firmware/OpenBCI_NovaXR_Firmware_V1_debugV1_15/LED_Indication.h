/*
 * ADS Registers Handler
 * 
 * Config Options 
 */
#ifndef _____LED_Indication_h
#define _____LED_Indication_h

#include "Arduino.h"
#include "NovaXR_Comm_Protocol_Definitions.h"



// =================== Prototypes =========================
void uBlox_HW_Init(void);
void Valid_CMD_Indication(void);
void Invalid_CMD_Indication(void);
void blink_Red(uint8_t blink_num, uint16_t Period);
void BlinkLED(const uint8_t led_pin, uint16_t period);
void turnOnLED(const uint8_t lep_pin, uint8_t stepDelay);
void turnOffLEDs(void);
void turnOnWhite(void);
void turnOnYellow(void);
void turnOnPurple(void);
void ERROR_Indication(void);
void CHECK_Indication(void);
void lightUP_Green(void);
void success_State(void);
void inProgress_Indication(void);
void show_standBy_State(void);
void show_streamming_State(void);
void noClient_Indication(void);
void WiFi_conn_inProgress_Indication(void);
void diconnected_Client_Indication(void);
void powered_On_Indication(void);
void Blink_DualColor(const uint8_t led1_pin, const uint8_t led2_pin, uint16_t period);
void standBy_mode(void);
#endif
