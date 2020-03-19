 #ifndef DATAL_HANDLER_H
#define DATA_HANDLER_H

#include "Arduino.h"


// ----------- Size of a UDP Packet Buffer ----------------------- //
#define UDP_TCP_PKT_SIZE 19  ///<  X * 72bytes of data  ---> 1 * 72 Bytes = 72 Bytes Packet
#define BUFFPOOL_SIZE    2

/* ------ NovaXR packet format --------
 * Packet Byte [0]:     Packet Number
 * Packet Byte [1:2]:   PPG
 * Packet Byte [3:4]:   EDA
 * Packet Byte [5:7]:   EEG_FC 0
 * Packet Byte [8:10]:  EEG_FC 1
 * Packet Byte [11:13]: EEG_OL 0
 * Packet Byte [14:16]: EEG_OL 1
 * Packet Byte [17:19]: EEG_OL 2
 * Packet Byte [20:22]: EEG_OL 3
 * Packet Byte [23:25]: EEG_OL 4
 * Packet Byte [26:28]: EEG_OL 5
 * Packet Byte [29:31]: EEG_OL 6
 * Packet Byte [32:34]: EEG_OL 7 
 * Packet Byte [35:37]: EOG 0 
 * Packet Byte [38:40]: EOG 1 
 * Packet Byte [41:43]: EMG 0 
 * Packet Byte [44:46]: EMG 1 
 * Packet Byte [47:49]: EMG 2 
 * Packet Byte [50:52]: EMG 3 
 * Packet Byte [53]:    Battery Level range: 0-100
 * Packet Byte [54:55]: Skin Temperature
 * Packet Byte [56]:    Firmware Error Code
 * Packet Byte [57:58]: AXL Z - Not Used
 * Packet Byte [59:60]: GYR X - Not Used 
 * Packet Byte [61:62]: GYR Y - Not Used
 * Packet Byte [63]:    GYR Z - Not Used
 * Packet Byte [64:71]: Timestamp 
 * ----- Total 72 Bytes ------------
 */




// ----------- Buffers and Data structures ----------------------- //
#pragma pack (push, 1)

struct IMU_Data{
  int8_t  BATT=0;
  int16_t SKIN_T=0;
  int8_t  FW_CODE=0;
  int16_t AXL=0;
  int16_t GYR_X=0;
  int16_t GYR_Y=0;
  int8_t  GYR_Z=0;
  
}; 


struct ADS_Data{
  uint8_t EEG_FC[2][3];   // 2 Channels 
  uint8_t EEG_OL[8][3];   // 8 Channels 
  uint8_t    EOG[2][3];   // 2 channels
  uint8_t    EMG[4][3];   // 4 Channels
};


struct AuxSensor_Data{
   uint16_t  PPG;   // 1 Channel
   uint16_t  EDA;   // 1 Channel
}; 

  
struct Sensors_Data {
  AuxSensor_Data  AUX;
  ADS_Data        ADS; 
  IMU_Data        IMU; 
};
  

struct DataPacket {
  uint8_t PacketNum;
  Sensors_Data Payload;
  double timeStamp;
};


struct switchOutBuffer{
  DataPacket oBuff[UDP_TCP_PKT_SIZE];
  bool fullBuff;
  bool emptyBuff; 
};


#pragma pack(pop)



// -----------  Output TCP/UDP Buffers --------------------------- //
extern DataPacket outBuffer[UDP_TCP_PKT_SIZE];
extern switchOutBuffer swOutBuffer[BUFFPOOL_SIZE];



// ================ Prototypes =================================== //
// ----------- Get Sensory Data Methods -------------------------- //
void getADS_RAWdata(ADS_Data * data);
void getIMU_RAWdata(IMU_Data * data);
void getADS_RAWdata(ADS_Data * data);
void getUpdatedSensoryData(Sensors_Data * data);
void assemble_outBuffer( uint8_t dataPkt_ID);

// ------------ Manage Switch Buffers --------------------------- //
void reset_swBuffers(void);
uint8_t getFull_swBuffer_ID(void);
uint8_t getEmpty_swBuffer_ID(void);
bool markFull_swBuffer(uint8_t swBuff_ID);
bool markEmpty_swBuffer(uint8_t swBuff_ID);
void fill_SwitchBuffer(Sensors_Data *payload, uint8_t pkgID, uint8_t entry_id, uint8_t swBuff_id);
void assemble_swBuffer( uint8_t dataPkt_ID, uint8_t swBuff_id);

// ----------- Demo Functions ---------------------------------- //
double putTimeStamp(void);
void printBatt_level(void);
void getBatteryLevel(Sensors_Data * data);
void printEDA(void);


#endif
