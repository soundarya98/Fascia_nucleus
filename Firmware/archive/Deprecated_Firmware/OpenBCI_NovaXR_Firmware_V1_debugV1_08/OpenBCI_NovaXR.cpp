/*
OpenBCI NovaXR Library
Place the containing folder into your libraries folder insdie the arduino folder in your Documents folder

This library will work with a single OpenBCI NovaXR Device
*/


#include "OpenBCI_NovaXR.h"




boolean isMultiCharCmd;  // A multi char command is in progress
char    multiCharCommand;  // The type of command
unsigned long multiCharCmdTimeout;  // the timeout in millis of the current multi char command





boolean processChar(char character){
  if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON) {
    Serial1.print("ProcessChar: ");
    Serial1.println(character);
  }
  
  if (checkMultiCharCmdTimer())
  { // we are in a multi char command
    switch (getMultiCharCommand())
    {
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
      Serial.println("streamSafeChannelDeactivate(1)");
      break;
    case OPENBCI_CHANNEL_OFF_2:
      Serial.println("streamSafeChannelDeactivate(2)");
      break;
    case OPENBCI_CHANNEL_OFF_3:
      Serial.println("streamSafeChannelDeactivate(3)");
      break;
    case OPENBCI_CHANNEL_OFF_4:
      Serial.println("streamSafeChannelDeactivate(4);");
      break;
    case OPENBCI_CHANNEL_OFF_5:
      Serial.println("streamSafeChannelDeactivate(5);");
      break;
    case OPENBCI_CHANNEL_OFF_6:
      Serial.println("streamSafeChannelDeactivate(6);");
      break;
    case OPENBCI_CHANNEL_OFF_7:
      Serial.println("streamSafeChannelDeactivate(7);");
      break;
    case OPENBCI_CHANNEL_OFF_8:
      Serial.println("streamSafeChannelDeactivate(8);");
      break;
    case OPENBCI_CHANNEL_OFF_9:
      Serial.println("streamSafeChannelDeactivate(9);");
      break;
    case OPENBCI_CHANNEL_OFF_10:
      Serial.println("streamSafeChannelDeactivate(10);");
      break;
    case OPENBCI_CHANNEL_OFF_11:
      Serial.println("streamSafeChannelDeactivate(11);");
      break;
    case OPENBCI_CHANNEL_OFF_12:
      Serial.println("streamSafeChannelDeactivate(12);");
      break;
    case OPENBCI_CHANNEL_OFF_13:
      Serial.println("streamSafeChannelDeactivate(13);");
      break;
    case OPENBCI_CHANNEL_OFF_14:
      Serial.println("streamSafeChannelDeactivate(14);");
      break;
    case OPENBCI_CHANNEL_OFF_15:
      Serial.println("streamSafeChannelDeactivate(15);");
      break;
    case OPENBCI_CHANNEL_OFF_16:
      Serial.println("streamSafeChannelDeactivate(16);");
      break;
    case OPENBCI_CHANNEL_ON_1:
      Serial.println("streamSafeChannelActivate(1);");
      break;
    case OPENBCI_CHANNEL_ON_2:
      Serial.println("streamSafeChannelActivate(2);");
      break;
    case OPENBCI_CHANNEL_ON_3:
      Serial.println("streamSafeChannelActivate(3);");
      break;
    case OPENBCI_CHANNEL_ON_4:
      Serial.println("streamSafeChannelActivate(4);");
      break;
    case OPENBCI_CHANNEL_ON_5:
      Serial.println("streamSafeChannelActivate(5);");
      break;
    case OPENBCI_CHANNEL_ON_6:
      Serial.println("streamSafeChannelActivate(6);");
      break;
    case OPENBCI_CHANNEL_ON_7:
      Serial.println("streamSafeChannelActivate(7);");
      break;
    case OPENBCI_CHANNEL_ON_8:
      Serial.println("streamSafeChannelActivate(8);");
      break;
    case OPENBCI_CHANNEL_ON_9:
      Serial.println("streamSafeChannelActivate(9);");
      break;
    case OPENBCI_CHANNEL_ON_10:
      Serial.println("streamSafeChannelActivate(10);");
      break;
    case OPENBCI_CHANNEL_ON_11:
      Serial.println("streamSafeChannelActivate(11);");
      break;
    case OPENBCI_CHANNEL_ON_12:
      Serial.println("streamSafeChannelActivate(12);");
      break;
    case OPENBCI_CHANNEL_ON_13:
      Serial.println("streamSafeChannelActivate(13);");
      break;
    case OPENBCI_CHANNEL_ON_14:
      Serial.println("streamSafeChannelActivate(14);");
      break;
    case OPENBCI_CHANNEL_ON_15:
      Serial.println("streamSafeChannelActivate(15);");
      break;
    case OPENBCI_CHANNEL_ON_16:
      Serial.println("streamSafeChannelActivate(16);");
      break;

    // TEST SIGNAL CONTROL COMMANDS
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_SHORTED, ADSTESTSIG_NOCHANGE, ADSTESTSIG_NOCHANGE);");
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_SLOW);");
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_FAST);");
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_DC:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_DCSIG);");
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_SLOW);");
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST:
      Serial.println("activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_FAST);");
      break;

    // CHANNEL SETTING COMMANDS
    case OPENBCI_CHANNEL_CMD_SET: // This is a multi char command with a timeout
      Serial.println("startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_CHANNEL);");
      Serial.println("numberOfIncomingSettingsProcessedChannel = 1;");
      break;

    // LEAD OFF IMPEDANCE DETECTION COMMANDS
    case OPENBCI_CHANNEL_IMPEDANCE_SET:
      Serial.println("startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_LEADOFF);");
      Serial.println("numberOfIncomingSettingsProcessedLeadOff = 1;");
      break;

    case OPENBCI_CHANNEL_DEFAULT_ALL_SET: // reset all channel settings to default
     Serial.println(" if (!streaming)");
     // {
        Serial.println("updating channel settings to");
        Serial.println(" default");
        Serial.println("sendEOT();");
     // }
      Serial.println("streamSafeSetAllChannelsToDefault();");
      break;
      
    case OPENBCI_CHANNEL_DEFAULT_ALL_REPORT: // report the default settings
      Serial.println("reportDefaultChannelSettings();");
      break;

    // DAISY MODULE COMMANDS
    case OPENBCI_CHANNEL_MAX_NUMBER_8: // use 8 channel mode
      Serial.println("use 8 channel mode");
      break;

    case OPENBCI_CHANNEL_MAX_NUMBER_16: // use 16 channel mode
      Serial.println("use 16 channel mode");
      break;

    // STREAM DATA AND FILTER COMMANDS
    case OPENBCI_STREAM_START: // stream data
      Serial.println("start stream data");
      streamingFlag =START_STREAMING;
      break;
  
    case OPENBCI_STREAM_STOP: // stop streaming data
      Serial.println("stop streaming data");
      streamingFlag =STOP_STREAMING;
      break;

    //  INITIALIZE AND VERIFY
    case OPENBCI_MISC_SOFT_RESET:
      Serial.println("boardReset();"); // initialize ADS and read device IDs
      break;
      
    //  QUERY THE ADS AND ACCEL REGITSTERS
    case OPENBCI_MISC_QUERY_REGISTER_SETTINGS:
       Serial.println(" printAllRegisters();"); // print the ADS and accelerometer register values
      break;

    // TIME SYNC
    case OPENBCI_TIME_SET:
      // Set flag to send time packet
      Serial.println("Set flag to send time packet");
      break;

    case OPENBCI_TIME_STOP:
      // Stop the Sync
      Serial.println("Stop the Sync");
      break;

    // BOARD TYPE SET TYPE
    case OPENBCI_BOARD_MODE_SET:
      Serial.println("Set Board Type");
      break;

    // Sample rate set
    case OPENBCI_SAMPLE_RATE_SET:
      Serial.println("startMultiCharCmdTimer(MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE);");
      break;

    // Insert Marker into the EEG data stream
    case OPENBCI_INSERT_MARKER:
      Serial.println("startMultiCharCmdTimer(MULTI_CHAR_CMD_INSERT_MARKER);");
      break;

    case OPENBCI_WIFI_ATTACH:
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
  return true;
}




/**
 * Check for valid on multi char commands
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
 * End the timer on multi char commands
 * @param None
 * @returns void
 */
void endMultiCharCmdTimer(void)
{
  isMultiCharCmd = false;
  multiCharCommand = MULTI_CHAR_CMD_NONE;
}

/**
 * Gets the active multi char command
 * @param None
 * @returns {char} multiCharCommand
 */
char getMultiCharCommand(void)
{
  return multiCharCommand;
}
