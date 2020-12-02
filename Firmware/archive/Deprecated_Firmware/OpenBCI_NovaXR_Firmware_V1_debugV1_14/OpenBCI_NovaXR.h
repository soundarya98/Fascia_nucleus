
/*
insert header here

*/
#ifndef _____OpenBCI_NovaXR__
#define _____OpenBCI_NovaXR__

#include "Arduino.h"
#include "OpenBCI_NovaXR_Definitions.h"
#include "NXR_Communication.h"
#include "LED_Indication.h"


#define curBoardMode 1 //enum
#define curDebugMode 0 //enum
#define BOARD_MODE_DEBUG 1
#define DEBUG_MODE_ON 1
#define CMD_SIZE      9  // 9 bytes 

extern char Command_String[CMD_SIZE];
extern boolean streamingFlag;


boolean processChar(char character);
boolean checkMultiCharCmdTimer(void);
void endMultiCharCmdTimer(void);
char getMultiCharCommand(void);
void processMultiChar(void);
bool checkMultiCharCommand(void);
int ASCIIchar_To_Int (char cmd_char);
uint8_t get_sisterADS_Input(char input_code);
byte BIAS_LOFF_CH_Enable(uint8_t selectNum, bool enableFlag, byte Register);
void config_BIAS_LOFF(bool positive, bool bias_loff_option, bool enableFlag, uint8_t Channel);


typedef enum MULTI_CHAR_COMMAND {
    MULTI_CHAR_CMD_NONE,
    MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_CHANNEL,
    MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_LEADOFF,
    MULTI_CHAR_CMD_SERIAL_PASSTHROUGH,
    MULTI_CHAR_CMD_SETTINGS_BOARD_MODE,
    MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE,
    MULTI_CHAR_CMD_INSERT_MARKER
  };





  
#endif
