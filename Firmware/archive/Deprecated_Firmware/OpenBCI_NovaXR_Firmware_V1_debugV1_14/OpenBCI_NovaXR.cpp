/*
OpenBCI NovaXR Library
Place the containing folder into your libraries folder insdie the arduino folder in your Documents folder

This library will work with a single OpenBCI NovaXR Device
*/


#include "OpenBCI_NovaXR.h"
#include "ADS1299_lib.h"


boolean       isMultiCharCmd;       //  A multi char command is in progress
char          multiCharCommand;     //  The type of command
unsigned long multiCharCmdTimeout;  //  The timeout in millis of the current multi char command

#define POSITIVE 1
#define BIAS 1
#define ENABLE 1


/**
 * ADS1299 Gain Options
 */
byte Gain[7] {
      ADS_GAIN01,  // 0x00
      ADS_GAIN02,  // 0x10
      ADS_GAIN04,  // 0x20
      ADS_GAIN06,  // 0x30
      ADS_GAIN08,  // 0x40
      ADS_GAIN12,  // 0x50
      ADS_GAIN24   // 0x60
    };

/**
 * ADS1299 Input Type Options
 */
byte Input_Type[8] {
    ADSINPUT_NORMAL,     
    ADSINPUT_SHORTED,    
    ADSINPUT_BIAS_MEAS,  
    ADSINPUT_MVDD,       
    ADSINPUT_TEMP,      
    ADSINPUT_TESTSIG,    
    ADSINPUT_BIAS_DRP,   
    ADSINPUT_BIAL_DRN   
   };

byte SRB2_Type[2]={CH_SRB2_DISCONNECTED, CH_SRB2_CONNECTED};
byte SRB1_Type[2]={SRB1_OPEN, SRB1_CLOS};
byte BIAS_Type[2]={PD_REFBUF_OFF, PD_REFBUF_ON};

/**
 * ADS1299 on NovaXR Sister Board input channels command representations
 */
byte ADS_sister_input_ID[8]{
     OPENBCI_CHANNEL_ON_9,
     OPENBCI_CHANNEL_ON_10,
     OPENBCI_CHANNEL_ON_11,
     OPENBCI_CHANNEL_ON_12,
     OPENBCI_CHANNEL_ON_13,
     OPENBCI_CHANNEL_ON_14,
     OPENBCI_CHANNEL_ON_15,
     OPENBCI_CHANNEL_ON_16
   };


/**
 * ADS1299 Test Signals Options
 */
byte ADS_Test_Signals[6]{
    ADSTESTSIG_AMP_1X,      
    ADSTESTSIG_AMP_2X,       
    ADSTESTSIG_PULSE_SLOW,    
    ADSTESTSIG_PULSE_FAST,    
    ADSTESTSIG_DCSIG,         
    ADSTESTSIG_NOCHANGE    
 };

 

// LOFF 0b 000 0 00 00
// LEAD_OFF_P_COMP 3bit   0->8 95% -> 70%  0b
// LEAD_OFF_N_COMP 3bit   0->8 5%  -> 30%
// LEAD_OFF_MAG    2bit   0->3 6nA,24nA,6uA,24uA

/**
 * ADS1299 LEAD_OFF Frequency Options
 */
byte LEAD_OFF_FREQ[4]={
    LOFF_FREQ_DC,       
    LOFF_FREQ_7p8HZ,     
    LOFF_FREQ_31p2HZ,    
    LOFF_FREQ_FS_4    
  };


/**
 * @brief     Process incomming command character
 *
 * @param[in]  character  The given commnad character
 *
 * @return    return True if the command takes effect
 */
boolean processChar(char character){
    
  if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON) {
     //  Serial.print("curBoardMode: ");Serial.println(curBoardMode);
       Serial.print("Command: "); Serial.println(character);
      //Serial.print("String: ");Serial.println(Command_String);
      if(checkMultiCharCommand())processMultiChar();
   }


 if (checkMultiCharCmdTimer()){ // we are in a multi char command  
       
       switch (getMultiCharCommand()) {     
          case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_CHANNEL:
            Serial.println("processIncomingChannelSettings(character);");
            break;
          case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_LEADOFF:
            Serial.println("processIncomingLeadOffSettings(character);");
            break;
          case MULTI_CHAR_CMD_SETTINGS_BOARD_MODE:
            Serial.println("processIncomingBoardMode(character);");
            break;
          case MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE:
            Serial.println("processIncomingSampleRate(character);");
            break;
          case MULTI_CHAR_CMD_INSERT_MARKER:
            Serial.println("processInsertMarker(character);");
            break;          
          default:
            break;
        }
    
  }else{
    
    
   // Normal...
   switch (character){
      
    //TURN CHANNELS ON/OFF COMMANDS
    case OPENBCI_CHANNEL_OFF_1:
      stop_and_configInput(MAIN_BOARD, 0, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE); 
      break;
    case OPENBCI_CHANNEL_OFF_2:
      stop_and_configInput(MAIN_BOARD, 1, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_3:
      stop_and_configInput(MAIN_BOARD, 2, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_4:
      stop_and_configInput(MAIN_BOARD, 3, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_5:
      stop_and_configInput(MAIN_BOARD, 4, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_6:
      stop_and_configInput(MAIN_BOARD, 5, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE); 
      break;
    case OPENBCI_CHANNEL_OFF_7:
      stop_and_configInput(MAIN_BOARD, 6, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_8:
      stop_and_configInput(MAIN_BOARD, 7, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_9:
      stop_and_configInput(SISTER_BOARD, 0, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_10:
      stop_and_configInput(SISTER_BOARD, 1, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_11:
      stop_and_configInput(SISTER_BOARD, 2, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_12:
      stop_and_configInput(SISTER_BOARD, 3, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_13:
      stop_and_configInput(SISTER_BOARD, 4, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_14:
      stop_and_configInput(SISTER_BOARD, 5, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_15:
      stop_and_configInput(SISTER_BOARD, 6, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_OFF_16:
      stop_and_configInput(SISTER_BOARD, 7, CHANNEL_OFF | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;         
    case OPENBCI_CHANNEL_ON_1:
      stop_and_configInput(MAIN_BOARD, 0, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_2:
      stop_and_configInput(MAIN_BOARD, 1, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_3:
      stop_and_configInput(MAIN_BOARD, 2, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_4:
      stop_and_configInput(MAIN_BOARD, 3, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_5:
      stop_and_configInput(MAIN_BOARD, 4, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_6:
      stop_and_configInput(MAIN_BOARD, 5, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_7:
      stop_and_configInput(MAIN_BOARD, 6, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE); 
      break;
    case OPENBCI_CHANNEL_ON_8:
      stop_and_configInput(MAIN_BOARD, 7, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE); 
      break;
    case OPENBCI_CHANNEL_ON_9:
      stop_and_configInput(SISTER_BOARD, 0, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_10:
      stop_and_configInput(SISTER_BOARD, 1, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_11:
      stop_and_configInput(SISTER_BOARD, 2, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_12:
      stop_and_configInput(SISTER_BOARD, 3, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_13:
      stop_and_configInput(SISTER_BOARD, 4, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_14:
      stop_and_configInput(SISTER_BOARD, 5, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_15:
      stop_and_configInput(SISTER_BOARD, 6, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    case OPENBCI_CHANNEL_ON_16:
      stop_and_configInput(SISTER_BOARD, 7, CHANNEL_ON | ADS_GAIN04 | CH_SRB2_DISCONNECTED | CH_NORMAL_ELECTRODE);
      break;
    
    // TEST SIGNAL CONTROL COMMANDS
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_SHORTED, ADSTESTSIG_NOCHANGE, ADSTESTSIG_NOCHANGE);");
      Serial.println("OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND");
      Serial.println(OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_SLOW);");
      Serial.println("OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW");
      Serial.println(OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_FAST);");
      Serial.println("OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST");
      Serial.println(OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_DC:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_DCSIG);");
      Serial.println("OPENBCI_TEST_SIGNAL_CONNECT_TO_DC");
      Serial.println(OPENBCI_TEST_SIGNAL_CONNECT_TO_DC);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_SLOW);");
      Serial.println("OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW");
      Serial.println(OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_FAST);");
      Serial.println("OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST");
      Serial.println(OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST);
      break;

    // CHANNEL SETTING COMMANDS
    case OPENBCI_CHANNEL_CMD_SET: // This is a multi char command with a timeout
      Serial.println("OPENBCI_CHANNEL_CMD_SET");
      Serial.println(OPENBCI_CHANNEL_CMD_SET);
      
      break;

    // LEAD OFF IMPEDANCE DETECTION COMMANDS
    case OPENBCI_CHANNEL_IMPEDANCE_SET:
      Serial.println("OPENBCI_CHANNEL_IMPEDANCE_SET");
      Serial.println(OPENBCI_CHANNEL_IMPEDANCE_SET);
      break;

    case OPENBCI_CHANNEL_DEFAULT_ALL_SET: // reset all channel settings to default
      Serial.println(" if (!streaming)");
      Serial.println("OPENBCI_CHANNEL_DEFAULT_ALL_SET");
      Serial.println(OPENBCI_CHANNEL_DEFAULT_ALL_SET);
      Serial.println("sendEOT();");
      break;
      
    case OPENBCI_CHANNEL_DEFAULT_ALL_REPORT: // report the default settings
      Serial.println("OPENBCI_CHANNEL_DEFAULT_ALL_REPORT");
      Serial.println(OPENBCI_CHANNEL_DEFAULT_ALL_REPORT);
      break;

    // DAISY MODULE COMMANDS
    case OPENBCI_CHANNEL_MAX_NUMBER_8: // use 8 channel mode
      Serial.println("OPENBCI_CHANNEL_MAX_NUMBER_8");
      Serial.println(OPENBCI_CHANNEL_MAX_NUMBER_8);
      break;

    case OPENBCI_CHANNEL_MAX_NUMBER_16: // use 16 channel mode
      Serial.println("OPENBCI_CHANNEL_MAX_NUMBER_16");
      Serial.println(OPENBCI_CHANNEL_MAX_NUMBER_16);
      break;

    // STREAM DATA AND FILTER COMMANDS
    case OPENBCI_STREAM_START: // stream data
      Serial.println("start stream data");
      streamingFlag = START_STREAMING;
      break;
  
    case OPENBCI_STREAM_STOP: // stop streaming data
      Serial.println("stop streaming data");
      streamingFlag = STOP_STREAMING;
      break;

    //  INITIALIZE AND VERIFY
    case OPENBCI_MISC_SOFT_RESET:
      Serial.println("OPENBCI_MISC_SOFT_RESET");
      Serial.println(OPENBCI_MISC_SOFT_RESET); // initialize ADS and read device IDs 
      break;
      
    //  QUERY THE ADS AND ACCEL REGITSTERS
    case OPENBCI_MISC_QUERY_REGISTER_SETTINGS:
       Serial.println("OPENBCI_MISC_QUERY_REGISTER_SETTINGS");
       Serial.println(OPENBCI_MISC_QUERY_REGISTER_SETTINGS); // print the ADS and accelerometer register values
       display_Registers_settings();    
     break;

    // TIME SYNC
    case OPENBCI_TIME_SET:
      // Set flag to send time packet
      Serial.println(OPENBCI_TIME_SET);
      Serial.println("Set flag to send time packet");
      break;

    case OPENBCI_TIME_STOP:
      // Stop the Sync
      Serial.println(OPENBCI_TIME_STOP);
      Serial.println("Stop the Sync");
      break;

    // BOARD TYPE SET TYPE
    case OPENBCI_BOARD_MODE_SET:
      Serial.println(OPENBCI_BOARD_MODE_SET);
      Serial.println("Set Board Type");
      break;

    // Sample rate set
    case OPENBCI_SAMPLE_RATE_SET:
      Serial.println(OPENBCI_SAMPLE_RATE_SET);
      Serial.println("startMultiCharCmdTimer(MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE);");
      break;

    // Insert Marker into the EEG data stream
    case OPENBCI_INSERT_MARKER:
      Serial.println(OPENBCI_INSERT_MARKER);
      Serial.println("startMultiCharCmdTimer(MULTI_CHAR_CMD_INSERT_MARKER);");
      break;

    case OPENBCI_WIFI_ATTACH:
      Serial.println(OPENBCI_WIFI_ATTACH);
      Serial.println("WIFI_ATTACH");
      break;

    case OPENBCI_WIFI_REMOVE:
      Serial.println("WIFI_REMOVE");
      break;

    case OPENBCI_WIFI_STATUS:
      Serial.println("WIFI_STATUS");
      break;

    case OPENBCI_WIFI_RESET:
      Serial.println("WIFI_RESET");
      break;
      
    case OPENBCI_GET_VERSION:
      Serial.println("GET_VERSION");
      break;
      
    default:
      return false;
    }
  }
   
   select_ADC(MAIN_BOARD);
   display_Registers_settings();
   select_ADC(SISTER_BOARD);
   display_Registers_settings(); 
  return true;
}




/**
 * @brief Check for valid on multi char commands
 * @param None
 * @returns {boolean} true if a multi char commands is active and the timer is running, otherwise False
 */
boolean checkMultiCharCmdTimer(void)
{
  if (isMultiCharCmd)
  {
    if (millis() < multiCharCmdTimeout)
      return true;
    else
    { // the timer has timed out - reset the multi char timeout
      endMultiCharCmdTimer();
      Serial.println("Timeout processing multi byte");
      Serial.println(" message - please send all");
      Serial.println(" commands at once as of v2");
      Serial.println("sendEOT();");
    }
  }
  return false;
}



/**
 * @brief End the timer on multi char commands
 * @param None
 * @returns void
 */
void endMultiCharCmdTimer(void){
  isMultiCharCmd = false;
  multiCharCommand = MULTI_CHAR_CMD_NONE;
}



/**
 * @brief Gets the active multi char command
 * @param None
 * @returns {char} multiCharCommand
 */
char getMultiCharCommand(void){
  return multiCharCommand;
}


/**
 * @brief    Check if the incoming command is a multichar command
 *
 * @return   returns True if it is a multichar Command
 */
bool checkMultiCharCommand(void){
  bool confirm = false;
  
  if((Command_String[0] == OPENBCI_CHANNEL_CMD_SET) && (Command_String[8] == OPENBCI_CHANNEL_CMD_LATCH)){  // Multi char mode
    Serial.println("Processing  Multichar Command");
    confirm=true;
  }
  return confirm;
}



/**
 * @brief      Process a Multi-Char incoming Command
 */
void processMultiChar(void){
    
    uint8_t Board_ID_num = 0;
    uint8_t config_channel = 0;  
    byte Input_Option =0x0;
    byte Gain_Option =0x0;
    byte BIAS_Option =0x0;
    byte SRB2_Option =0x0;
    byte SRB1_Option =0x0;
                        
    int ch_Input = ASCIIchar_To_Int(Command_String[1]);
   
   if(ch_Input<9){ // if main board input is requested to be changed
      Board_ID_num=MAIN_BOARD;
      config_channel = ch_Input-1;
    
   }else{          // if sister board input is requested to be changed
      Board_ID_num=SISTER_BOARD;
      config_channel =  get_sisterADS_Input(Command_String[1]);        // Channel Config      #1 
   }
   
    Serial.print("Board:  ");Serial.println(Board_ID_num);
    Serial.print("ADS in:  ");Serial.println(config_channel); 
    select_ADC(Board_ID_num);  
   
    Gain_Option = Gain[ASCIIchar_To_Int(Command_String[3])];         // Gain Config         #3
    Input_Option = Input_Type[ASCIIchar_To_Int(Command_String[4])];  // Input Type Config   #4          
    BIAS_Option = BIAS_Type[ASCIIchar_To_Int(Command_String[5])];    // BIAS Config         #5
    SRB2_Option = SRB2_Type[ASCIIchar_To_Int(Command_String[6])];    // SRB2 Config         #6
    SRB1_Option = SRB1_Type[ASCIIchar_To_Int(Command_String[7])];    // SRB1 Config         #7
    
    Serial.print("CMD: ");Serial.print(Command_String[1]);Serial.print(" --> Channel:    ");Serial.print("0b"); Serial.println(config_channel,BIN);
    Serial.print("CMD: ");Serial.print(Command_String[3]);Serial.print(" --> GAIN:       ");Serial.print("0b"); Serial.println(Gain_Option,BIN);
    Serial.print("CMD: ");Serial.print(Command_String[4]);Serial.print(" --> Input Tupe: ");Serial.print("0b"); Serial.println(Input_Option,BIN);      
    Serial.print("CMD: ");Serial.print(Command_String[5]);Serial.print(" --> BIAS:       ");Serial.print("0b"); Serial.println(BIAS_Option,BIN);           
    Serial.print("CMD: ");Serial.print(Command_String[6]);Serial.print(" --> SRB2:       ");Serial.print("0b"); Serial.println(SRB2_Option,BIN);            
    Serial.print("CMD: ");Serial.print(Command_String[7]);Serial.print(" --> SRB1:       ");Serial.print("0b"); Serial.println(SRB1_Option,BIN);
    Serial.print("INPUT Config COMMAND: ");Serial.println((config_channel, CHANNEL_ON | Gain_Option | SRB2_Option |Input_Option),BIN);
  
    //display_Registers_settings();
    stop_and_configInput(Board_ID_num, config_channel, CHANNEL_ON | Gain_Option | SRB2_Option |Input_Option );
   //config_ADS_Register(LOFF,LOFF_COMP_P_92_5 | 0 | LOFF_MAG_6NA | LEAD_OFF_FREQ[0]);
   //config_BIAS_LOFF(POSITIVE, BIAS, ENABLE, 3);
    config_ADS_Register(CONFIG3, BIAS_Option);
    config_ADS_Register(MISC1, SRB1_Option);

   /*
    select_ADC(MAIN_BOARD);
    display_Registers_settings();
    select_ADC(SISTER_BOARD);
    display_Registers_settings(); 
  */
}



/**
 * @brief     Config BIAS or LOFF per channel
 *
 * @param[in]  positive          The positive channel
 * @param[in]  bias_loff_option  The bias lead-off enable '1' disable '0' option
 * @param[in]  enableFlag        The enable flag
 * @param[in]  Channel           The channel to set BIAS or LOFF
 */
void config_BIAS_LOFF(bool positive, bool bias_loff_option, bool enableFlag, uint8_t Channel){
    byte reg_state,reg_SENS =0x00;
   
    if(bias_loff_option){ // ----- BIAS SENSE Config -------------
       if(positive) reg_SENS = BIAS_SENSP; // ----- Positive     
       else         reg_SENS = BIAS_SENSN; // ----- Negative           
    }else{                // ----- LOFF SENSE Config -------------
       if(positive) reg_SENS = LOFF_SENSP; // ----- Positive 
       else         reg_SENS = LOFF_SENSN; // ----- Negative     
    }
  
   reg_state = readRegisterSettings(reg_SENS);
   reg_state = BIAS_LOFF_CH_Enable(Channel,enableFlag,reg_state);
   config_ADS_Register(reg_SENS, reg_state);  
      
}



/**
 * @brief     BIAS or LOFF channel Disable/ Enable action
 *
 * @param[in]  selectNum   The select number of the input channel to configure
 * @param[in]  enableFlag  The enable flag
 * @param[in]  Register    The register's configuration value
 *
 * @return    returns the configuration byte value
 */
byte BIAS_LOFF_CH_Enable(uint8_t selectNum, bool enableFlag, byte Register){
    byte SelectMask =0x00;
    if(enableFlag) SelectMask = 0x01; 
    else           SelectMask = 0xFE;
    SelectMask = SelectMask << selectNum;
    return Register = Register|SelectMask;
}



/**
 * @brief      Convert ASCII Characters to integers
 *
 * @param[in]  cmd_char  The given command character
 *
 * @return     returns the corresponding integer of the ASCII table
 */
int ASCIIchar_To_Int (char cmd_char){
   int integer = (int)cmd_char - 48;
  return integer;
}


/**
 * @brief      Translate the Incoming Channel Command to ADS input ID for Sisterboard
 *
 * @param[in]  input_code  The input command that corresponds to the input channel of the sister board
 *
 * @return     returns the corresponding ADS1299 input channel
 */
uint8_t get_sisterADS_Input(char input_code){

  for(uint8_t index=0; index<8; index++){
     if(input_code == ADS_sister_input_ID[index]){
       Serial.print("ADS Input: ");Serial.println(index);
       return index;
     }
  }
}
