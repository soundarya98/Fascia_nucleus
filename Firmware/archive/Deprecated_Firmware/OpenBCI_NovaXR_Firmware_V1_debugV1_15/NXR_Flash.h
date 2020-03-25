#ifndef NXR_FLASH_H
#define NXR_FLASH_H

 
#define TIMEST_SIZE 20  ///< Date and Time String Size 
#define WIFI_SIZE   20  ///< WiFi SSID or Password Strings size
#define CRED_SIZE   10  ///< Credentials List Size
#define SN_SIZE     10  ///< Serial Number Sring Size
#define VN_SIZE     8   ///< Vendor Name String Size
#define REV_SIZE    5   ///< Hardware Revision Sting Size
#define FWUPDATE    115 ///< Firmware Version String  

#define VENDORNM    "OpenBCI" ///< Vendor Name String
#define DEV_NAME    "Nova_XR" ///< Device Name String
#define HWREV       "V1.1/V2" ///< Hardware Revision String

#include <string.h>


//#pragma pack (push, 1)
/**
 * Device Information Data Structure
 */
typedef struct {
  bool writtenFlag;
  uint8_t last_Cred_Pos;  // Last updated WiFi Position
  uint16_t FW_version;
  char VendorName[VN_SIZE];
  char Device_Name[SN_SIZE];
  char FW_UpdateTime[TIMEST_SIZE];
  char NXR_BoardRev[REV_SIZE];
  char NXR_SerialNumber[SN_SIZE];
}Device_Info;

/**
 * WiFi Credentials Data Structure
 */
typedef struct {
  bool CredState;
  char wifiSSID[WIFI_SIZE];
  char wifiPASS[WIFI_SIZE];
}NXR_WiFi;

/**
 * Device All Info
 */
typedef struct {
  Device_Info boardInfo;
  NXR_WiFi wifiCredentials[CRED_SIZE];
}NXR_Rom;
//#pragma pack(pop)

 

// ------------------------ Prototypes --------------------------------------
void ROM_Init(void);
void displayHardwareInfo(void);
void displaySaved_WiFiCredentails(void);
void update_DeviceInfo(char *Dev_Name, char *UpdateTime, uint16_t fw_Version, char *HWrev, char * Vendor_Name);
bool update_WifiCred(char *wifi_ssid, char *Password);
uint8_t find_WiFiSpace(void);
void getSaved_WiFiCredentials(NXR_Rom *flash_data, uint8_t id);
void eraseDev_Info(void);
void eraseWiFi_Info(void);
void writeDev_StandardInfo(NXR_Rom *flash_data);
uint32_t getSerialNumber_ID(void);
int8_t lastWiFi_ID(void);
void eraseAllFlashContnent(void);
uint8_t count_SSIDsInList(void);
char * CreateSSID(void);
char * CreatePASS(void);
void getCurrent_ROMdata(NXR_Rom *flash_data);
uint8_t previous_WiFi_ID(void);
bool WiFi_ID_exists(int8_t id);
bool connect_lastWiFi( uint8_t last_wifi);

#endif