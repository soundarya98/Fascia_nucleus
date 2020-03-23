 #ifndef DATAL_HANDLER_H
#define DATA_HANDLER_H

#include "Arduino.h"


/// NovaXR Data Packet Format
/// -------------------------------------------------------
/// Packet Byte | Field Name | Description 
/// ------------|------------|------------------------------
/// [0]         | Sample ID  | Data Packet/Point ID 0-254
/// [1:2]       | PPG        | Photoplethysmogram Signal
/// [3:4]       | EDA        | Electrodermal Activity signal 
/// [5:7]       | EEG_FC 0   | EEG Frontal Cortex signal #0
/// [8:10]      | EEG_FC 1   | EEG Frontal Cortex signal #1
/// [11:13]     | EEG_OL 0   | EEG Occipital Lobe signal #0
/// [14:16]     | EEG_OL 1   | EEG Occipital Lobe signal #1
/// [17:19]     | EEG_OL 2   | EEG Occipital Lobe signal #2
/// [20:22]     | EEG_OL 3   | EEG Occipital Lobe signal #3
/// [23:25]     | EEG_OL 4   | EEG Occipital Lobe signal #4
/// [26:28]     | EEG_OL 5   | EEG Occipital Lobe signal #5
/// [29:31]     | EEG_OL 6   | EEG Occipital Lobe signal #6
/// [32:34]     | EEG_OL 7   | EEG Occipital Lobe signal #7
/// [35:37]     | EOG 0      | Electrooculography signal #0
/// [38:40]     | EOG 1      | Electrooculography signal #1
/// [41:43]     | EMG 0      | Electromyography signal #0
/// [44:46]     | EMG 1      | Electromyography signal #1
/// [47:49]     | EMG 2      | Electromyography signal #2
/// [50:52]     | EMG 3      | Electromyography signal #3
/// [53]        | BATT       | Battery Level range: 0-100
/// [54:55]     | SKIN_T     | Skin Temperature
/// [56]        | FW_CODE    | Firmware Error Code
/// [57:58]     | AUX0       | Reserved But Not Used
/// [59:60]     | AUX1       | Reserved But Not Used
/// [61:62]     | AUX2       | Reserved But Not Used
/// [63]        | AUX3       | Reserved But Not Used
/// [64:71]     | Timestamp  | Timestamp in Microseconds
/// -------------------------------------------------------
/// Total 72 Bytes per Data Packet
/// -------------------------------------------------------





// ----------- Size of a UDP Packet Buffer ----------------------- //
#define UDP_TCP_PKT_SIZE 19  ///<  19 * 72bytes of a data-point = 1368 Bytes per UDP/TCP Packet
#define BUFFPOOL_SIZE    2




// ----------- Buffers and Data structures ----------------------- //
#pragma pack (push, 1)///< Keep Precise Byte Count 


/**
 * @brief      Bio-signals data 
 */
struct BIO_Data{
   uint16_t  PPG;   ///< PPG Data - IR Ray Reflection Data
   uint16_t  EDA;   ///< EDA - Electrodermal Activity Data
}; 


/**
 * @brief      ADS1299 EEG and EMG Data
 */
struct ADS_Data{
  uint8_t EEG_FC[2][3];   ///< EEG Frontal Cortex Signals Data - 2 Channels
  uint8_t EEG_OL[8][3];   ///< EEG Occipital Lobe Signals Data - 8 Channels 
  uint8_t    EOG[2][3];   ///< Electrooculography signals Data - 2 channels
  uint8_t    EMG[4][3];   ///< Electroencephalography signals Data - 4 Channels
};


/**
 * @brief      Auxilary Data Structure 
 */
struct AUX_Data{
  int8_t  BATT=0;    ///< Percentile Battery Charge Level
  int16_t SKIN_T=0;  ///< Skin Temperature in Celsius
  int8_t  FW_CODE=0; ///< Firmware Feedback Messages 
  int16_t AUX0=0;    ///< RESERVED FOR FUTURE USE
  int16_t AUX1=0;    ///< RESERVED FOR FUTURE USE
  int16_t AUX2=0;    ///< RESERVED FOR FUTURE USE
  int8_t  AUX3=0;    ///< RESERVED FOR FUTURE USE
  
}; 


/**
 * @brief     Aggregated All Sensory Data
 */
struct Sensors_Data {
  BIO_Data  BIO;  ///< Bio-signals Data Structure 
  ADS_Data  ADS;  ///< ADS1299 signals Data Structure 
  AUX_Data  AUX;  ///< Auxiliary Data Structure  
};
  

/**
 * @brief     Single Data Packet/Point Data Structure
 */
struct DataPacket {
  uint8_t PacketNum;    ///< Datapoint Number 0 - 254
  Sensors_Data Payload; ///< Paylod of the Data Packet include all sensory and auxiliary data we send
  double timeStamp;     ///< Local Time Stamp in Microseconds (uS)
};


/**
 * @brief     Sending Data Buffer Structure
 */
struct switchOutBuffer{
  DataPacket oBuff[UDP_TCP_PKT_SIZE]; ///< Data packet
  bool fullBuff;                      ///< Full Data Bufferm Status Flag
  bool emptyBuff;                     ///< Empty Data Bufferm Status Flag
};

#pragma pack(pop) ///< End Pragma Here





// -----------  Output TCP/UDP Buffers --------------------------- //
extern DataPacket outBuffer[UDP_TCP_PKT_SIZE];
extern switchOutBuffer swOutBuffer[BUFFPOOL_SIZE];



// ================ Prototypes =================================== //
// ----------- Get Sensory Data Methods -------------------------- //
void getRAW_BIOdata(BIO_Data * data);
void getRAW_ADSdata(ADS_Data * data);
void getRAW_AUXdata(AUX_Data * data);
void getUpdatedSensoryData(Sensors_Data * data);

// ------------ Manage Send Buffers --------------------------- //
void reset_swBuffers(void);
uint8_t getFull_swBuffer_ID(void);
uint8_t getEmpty_swBuffer_ID(void);
bool markFull_swBuffer(uint8_t swBuff_ID);
bool markEmpty_swBuffer(uint8_t swBuff_ID);
void assemble_outBuffer( uint8_t dataPkt_ID);
void assemble_swBuffer( uint8_t dataPkt_ID, uint8_t swBuff_id);
void fill_SwitchBuffer(Sensors_Data *payload, uint8_t pkgID, uint8_t entry_id, uint8_t swBuff_id);

// ----------- Demo Functions ---------------------------------- //
double putTimeStamp(void);




#endif
