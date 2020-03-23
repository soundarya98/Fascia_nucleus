#ifndef DATAL_HANDLER_H
#define DATA_HANDLER_H

#include "Arduino.h"


// ----------- Size of a UDP Packet Buffer ----------------------- //
#define UDP_TCP_PKT_SIZE 19  //  X * 72bytes of data  ---> 1 * 72 Bytes = 72 Bytes Packet
#define BUFFPOOL_SIZE  2

/* ------ NovaXR packet format --------
 * Packet Byte [0]:     Packet Number
 * Packet Byte [1]:     PPG
 * Packet Byte [2:3]:   EDA
 * Packet Byte [4:6]:   EEG_FC 0
 * Packet Byte [7:9]:   EEG_FC 1
 * Packet Byte [10:12]: EEG_OL 0
 * Packet Byte [13:15]: EEG_OL 1
 * Packet Byte [16:18]: EEG_OL 2
 * Packet Byte [19:21]: EEG_OL 3
 * Packet Byte [22:24]: EEG_OL 4
 * Packet Byte [25:27]: EEG_OL 5
 * Packet Byte [28:30]: EEG_OL 6
 * Packet Byte [31:33]: EEG_OL 7 
 * Packet Byte [34:36]: EOG 0 
 * Packet Byte [37:39]: EOG 1 
 * Packet Byte [40:42]: EMG 0 
 * Packet Byte [43:45]: EMG 1 
 * Packet Byte [46:48]: EMG 2 
 * Packet Byte [49:51]: EMG 3 
 * Packet Byte [52:53]: AXL X  
 * Packet Byte [54:55]: AXL Y
 * Packet Byte [56:57]: AXL Z 
 * Packet Byte [58:59]: GYR X 
 * Packet Byte [60:61]: GYR Y 
 * Packet Byte [62:63]: GYR Z 
 * Packet Byte [64:71]: Timestamp 
 * ----- Total 72 Bytes ------------
 */




// ----------- Buffers and Data structures ----------------------- //
#pragma pack (push, 1)

struct IMU_Data{
  int16_t AXL_X=0;
  int16_t AXL_Y=0;
  int16_t AXL_Z=0;
  int16_t GYR_X=0;
  int16_t GYR_Y=0;
  int16_t GYR_Z=0;
}; 

struct ADS_Data{
  uint8_t EEG_FC[2][3];   // 2 Channels 
  uint8_t EEG_OL[8][3];   // 8 Channels 
  uint8_t    EOG[2][3];   // 2 channels
  uint8_t    EMG[4][3];   // 4 Channels
};


  struct AuxSensor_Data{
     uint8_t   PPG;   // 1 Channel
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
