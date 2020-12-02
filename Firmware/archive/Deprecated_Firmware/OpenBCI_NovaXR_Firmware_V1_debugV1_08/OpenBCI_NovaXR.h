
/*
insert header here

*/
#ifndef _____OpenBCI_NovaXR__
#define _____OpenBCI_NovaXR__

#include "Arduino.h"
#include "OpenBCI_NovaXR_Definitions.h"
#include "NXR_Utility_Lib.h"

#define curBoardMode 1 //enum
#define curDebugMode 0 //enum
#define BOARD_MODE_DEBUG 1
#define DEBUG_MODE_ON 1


extern boolean streamingFlag;


boolean processChar(char character);
boolean checkMultiCharCmdTimer(void);
void endMultiCharCmdTimer(void);
char getMultiCharCommand(void);


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
