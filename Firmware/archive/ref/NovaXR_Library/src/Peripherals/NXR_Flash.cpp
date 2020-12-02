
/*
* Ioannis Smanis
* Flash Memory Management Module
*
*/

#include "Arduino.h"
#include "NXR_Flash.h"
#include "NXR_DataExchange.h"
#include "WiFi_Management.h"
#include <FlashStorage.h>


// --- Declarations ----
FlashStorage(Flash_handler, NXR_Rom); 




/**
 * @brief      EEPROM - Device ROM Initialization
 */
void ROM_Init(void) {
 
      // eraseAllFlashContnent(&nxrRom);
      // Flash_handler.write(nxrRom);
      NXR_Rom novaXR_Rom;
      novaXR_Rom = Flash_handler.read();

     if(!novaXR_Rom.boardInfo.writtenFlag){ // If Flash is NOT written 
        Serial.println(" -- Fresh Factory Board");
        writeDev_StandardInfo(&novaXR_Rom);
        
     }else{ // If Flash is written 
        Serial.println(" -- PreProgrammed Board");
        
     }
       
      displayHardwareInfo();
      displaySaved_WiFiCredentails();
                         
}



/**
 * @brief      Counts the number of SSIDs in the stored wifi list.
 *
 * @return     Number of stored network SSIDs
 */
uint8_t count_SSIDsInList(void){
  NXR_Rom novaXR_Rom;
  novaXR_Rom = Flash_handler.read();
  
  uint8_t a,cred_count=0;
  for( a=0; a<CRED_SIZE; a++){
    if(!novaXR_Rom.wifiCredentials[a].CredState){
      if(a=0)cred_count=0;
      break;
    }else{
      cred_count++;
     }  
  }
   return cred_count;
}


/**
 * @brief      Connects to the last wifi network listed in the EEPROM MEMORY of the device
 *
 * @param[in]  last_wifi  The last wifi position
 *
 * @return     True if connection is ssuccessful 
 */
bool connect_lastWiFi( uint8_t last_wifi) {
   NXR_Rom novaXR_Rom;
   novaXR_Rom = Flash_handler.read();
   return Connect_to_AP(novaXR_Rom.wifiCredentials[last_wifi].wifiSSID, novaXR_Rom.wifiCredentials[last_wifi].wifiPASS); 
}


/**
 * @brief   Erase all EEPROM Content from the NovaXR device
 */
void eraseAllFlashContnent(void){
    eraseDev_Info();
    eraseWiFi_Info();
}



/**
 * @brief   Erase Device information , Initialize all data to Zero.
 */
void eraseDev_Info(void){
  NXR_Rom novaXR_Rom;
  novaXR_Rom = Flash_handler.read();
  novaXR_Rom.boardInfo.writtenFlag = false; // Mark memory are erased
  novaXR_Rom.boardInfo= {0};
  Flash_handler.write(novaXR_Rom);
  
}



/**
 * @brief    Erase all stored WiFi network credentials
 */
void eraseWiFi_Info(void){
  NXR_Rom novaXR_Rom;
  novaXR_Rom = Flash_handler.read();
  novaXR_Rom.wifiCredentials[CRED_SIZE]={0};
  Flash_handler.write(novaXR_Rom);
  
}


/**
 * @brief      Write the Flash Memory with standard information about the current device.
 *
 * @param      flash_data  The flash data structure instance 
 */
void writeDev_StandardInfo(NXR_Rom *flash_data){

      NXR_Rom novaXR_Rom;
      novaXR_Rom = Flash_handler.read();
      
      int EntrySize = sizeof(novaXR_Rom.boardInfo);
      int BankSize  = sizeof(NXR_Rom);
      Serial.print("NovaXR ROM Size:  ");
      Serial.print(BankSize);
      Serial.println(" Bytes\n");

      char * date       = "09/15/2019-23_22_06";
      char * devName    =  DEV_NAME;
      char * devRev     =  HWREV; 
      char * vendor     =  VENDORNM;
      uint16_t FWupdate =  FWUPDATE;
      
      novaXR_Rom.boardInfo.writtenFlag = true; // Mark memory are written  
      Flash_handler.write(novaXR_Rom); 
 
      update_DeviceInfo(devName, date, FWupdate, devRev, vendor);
}




/**
 * @brief      Get the memory position of the last Stored WiFi network.
 *
 * @return     lastWiFi_id   The last WiFi network position that is stored in the wifi list
 */
int8_t lastWiFi_ID(void){   

    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
    
    int8_t lastWiFi_id = novaXR_Rom.boardInfo.last_Cred_Pos;
    
    if ((lastWiFi_id == 0)&&(!novaXR_Rom.wifiCredentials[lastWiFi_id].CredState)){
      lastWiFi_id=-1;
    }

    return lastWiFi_id;
}



/**
 * @brief      Determines if a specific WiFi id exists in the wifi list.
 *
 * @param[in]  id     The identifier of a specific WiFi position of the list.
 *
 * @return     True if wifi id exists, False otherwise.
 */
bool WiFi_ID_exists(int8_t id){

    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
    if(novaXR_Rom.wifiCredentials[id].CredState){
      return true;
    }else{
      return false;
    }
  
}


/**
 * @brief      Get the previous WiFi ID counting from the last wifi location in the WiFi list
 *
 * @return     WiFI id position in the list
 */
uint8_t previous_WiFi_ID(void){

     int8_t id = lastWiFi_ID() - 1;
     
     if (id<0){       
        id = CRED_SIZE-1;
        if(!WiFi_ID_exists(id)){
          id=0;
       }
     }

    return id;
}




/**
 * @brief      Finds empty space position in the WiFi list
 *
 * @return     new_location Find the new WiFi list location for any use
 */
uint8_t find_WiFiSpace(void){
    
    uint8_t new_location=0;
    int8_t last_id =  lastWiFi_ID();
     
    if(last_id<CRED_SIZE-1){
      new_location=last_id+1;
     }else{
      new_location=0;
     }

   return new_location;
 }



/**
 * @brief      Update NovaXR Device Information.
 *
 * @param      Dev_Name     The dev name
 * @param      UpdateTime   The last update time
 * @param[in]  fw_Version   The firmware version
 * @param      HWrev        The hardware revision
 * @param      Vendor_Name  The vendor name
 */
void update_DeviceInfo(char *Dev_Name, char *UpdateTime, uint16_t fw_Version, char *HWrev, char * Vendor_Name){
      
      NXR_Rom novaXR_Rom;
      novaXR_Rom = Flash_handler.read();
      
      String D_Name = String(Dev_Name);
      D_Name.toCharArray(novaXR_Rom.boardInfo.Device_Name, SN_SIZE);

      String FW_date = String(UpdateTime);
      FW_date.toCharArray(novaXR_Rom.boardInfo.FW_UpdateTime, TIMEST_SIZE);

      uint32_t serial_number = getSerialNumber_ID();
      sprintf(novaXR_Rom.boardInfo.NXR_SerialNumber, "%8X", serial_number);

      String HardwareRev = String(HWrev);
      HardwareRev.toCharArray(novaXR_Rom.boardInfo.NXR_BoardRev, REV_SIZE);

      String vendorName = String(Vendor_Name);
      vendorName.toCharArray(novaXR_Rom.boardInfo.VendorName, VN_SIZE);

      novaXR_Rom.boardInfo.FW_version = fw_Version; 
      novaXR_Rom.boardInfo.writtenFlag = true; // Flash memory is now written
      
      Flash_handler.write(novaXR_Rom); 
}



/**
 * @brief      Add a new Wifi in the Credentials List
 *
 * @param      wifi_ssid  The wifi ssid
 * @param      Password   The password
 *
 * @return     flagCred returns True if the wifilist is upadted successfuly 
 */
bool update_WifiCred(char *wifi_ssid, char *Password){
  
      bool flagCred=false;
      NXR_Rom novaXR_Rom;
      novaXR_Rom = Flash_handler.read();

      uint8_t new_pos = find_WiFiSpace();
       
      String WifiSSID = String(wifi_ssid);
      WifiSSID.toCharArray(novaXR_Rom.wifiCredentials[new_pos].wifiSSID, WIFI_SIZE);

      String WifiPASS = String(Password);
      WifiPASS.toCharArray(novaXR_Rom.wifiCredentials[new_pos].wifiPASS, WIFI_SIZE);
      
      flagCred = novaXR_Rom.wifiCredentials[new_pos].CredState = true;

      if(new_pos<CRED_SIZE){
        novaXR_Rom.boardInfo.last_Cred_Pos = new_pos;
      }else{
        novaXR_Rom.boardInfo.last_Cred_Pos = 0;
      }
      
      Flash_handler.write(novaXR_Rom);  // Write data to the Memory     
      return flagCred;
}




/**
 * @brief      Print the entire WiFi list 
 */
void displaySaved_WiFiCredentails(void){

     NXR_Rom novaXR_Rom;
     novaXR_Rom = Flash_handler.read();
      
    Serial.println(" =====  List of Saved WiFi Credentials  ======");
    for(int i=0; i<CRED_SIZE; i++){
      Serial.print(i);
      Serial.print(". ");
      Serial.print("SSID:  ");
      Serial.print(novaXR_Rom.wifiCredentials[i].wifiSSID);
      Serial.print(" -- Password:  "); 
      Serial.println(novaXR_Rom.wifiCredentials[i].wifiPASS); 
    }
    Serial.println(" =============================================");
}


/**
 * @brief      Print saved Hardware Information from the NovaXR device EEPROM memory
 */
void displayHardwareInfo(void){

    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
    
    Serial.println("-------- Device Info --------");
    Serial.print("Vendor:         ");
    Serial.println(novaXR_Rom.boardInfo.VendorName);
    Serial.print("Device Name:    ");
    Serial.println(novaXR_Rom.boardInfo.Device_Name);
    Serial.print("Device SN:      ");
    Serial.println(novaXR_Rom.boardInfo.NXR_SerialNumber);  
    Serial.print("Board HW Rev :  ");
    Serial.println(novaXR_Rom.boardInfo.NXR_BoardRev);
    Serial.print("Last Update:    ");
    Serial.println(novaXR_Rom.boardInfo.FW_UpdateTime);  
    Serial.print("Firmware Ver:   ");
    float FW = ((float)novaXR_Rom.boardInfo.FW_version/100.00);
    Serial.println(FW,2);        
    Serial.println("-----------------------------\n");
    
}






/**
 * @brief      Creates a WiFi SSID network based on the device serial number
 *
 * @return     ssID returns a string pointer of the SSID name
 */
char * CreateSSID(void) {
    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
    char buf[13];
    sprintf(buf,"%s_%c%c%c%c%c" ,novaXR_Rom.boardInfo.Device_Name,
                       novaXR_Rom.boardInfo.NXR_SerialNumber[4],
                       novaXR_Rom.boardInfo.NXR_SerialNumber[4], 
                       novaXR_Rom.boardInfo.NXR_SerialNumber[5], 
                       novaXR_Rom.boardInfo.NXR_SerialNumber[6], 
                       novaXR_Rom.boardInfo.NXR_SerialNumber[7]);
                   
    char *ssID;
    ssID = (char *)malloc(sizeof (char) * 13); 
    sprintf(ssID,"%s",buf);
    return ssID;
}




/**
 * @brief      Creates a WiFi password based on the device serial number.
 *
 * @return     Passwd {returns a string pointer of the password}
 */
char * CreatePASS(void) {
    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
      
    char *Passwd;
    Passwd = (char *)malloc(sizeof (char) * 13); 
    sprintf(Passwd,"%s",novaXR_Rom.boardInfo.NXR_SerialNumber);
    return Passwd;
}



/**
 * @brief      Get Unique SMAD21 Chip ID and Create a Unique Serial Numner.
 *
 * @return     SerialNumber.
 */
uint32_t getSerialNumber_ID(void) {
  
  volatile uint32_t val1, val2, val3, val4;
  volatile uint32_t *ptr1 = (volatile uint32_t *)0x0080A00C; // SAMD21only
  val1 = *ptr1;
  volatile uint32_t *ptr = (volatile uint32_t *)0x0080A040;  // SAMD21only
  val2 = *ptr;
  ptr++;
  val3 = *ptr;
  ptr++;
  val4 = *ptr;

  uint32_t SerialNumber = val1+val2+val3+val4;
  return SerialNumber;
}
