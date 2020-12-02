
/*
* Ioannis Smanis
* Flash Memory Management Module
*
*/

#include "Arduino.h"
#include "NXR_Flash.h"
#include "NXR_Utility_Lib.h"
#include <FlashStorage.h>


// --- Declarations ----
FlashStorage(Flash_handler, NXR_Rom); 




  
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



bool connect_lastWiFi( uint8_t last_wifi) {
   NXR_Rom novaXR_Rom;
   novaXR_Rom = Flash_handler.read();
   return Connect_to_AP(novaXR_Rom.wifiCredentials[last_wifi].wifiSSID, novaXR_Rom.wifiCredentials[last_wifi].wifiPASS); 
}


void eraseAllFlashContnent(void){
    eraseDev_Info();
    eraseWiFi_Info();
}


// ----- Initialize all data to Zero -----
void eraseDev_Info(void){
  NXR_Rom novaXR_Rom;
  novaXR_Rom = Flash_handler.read();
  novaXR_Rom.boardInfo.writtenFlag = false; // Mark memory are erased
  novaXR_Rom.boardInfo= {0};
  Flash_handler.write(novaXR_Rom);
  
}


// ----- Initialize WiFi data to Zero -----
void eraseWiFi_Info(void){
  NXR_Rom novaXR_Rom;
  novaXR_Rom = Flash_handler.read();
  novaXR_Rom.wifiCredentials[CRED_SIZE]={0};
  Flash_handler.write(novaXR_Rom);
  
}

// ---- Write the Flash Memory with standard information about the current device -------
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





// --- Get the memory position of the last Stored WiFi network -----------
int8_t lastWiFi_ID(void){   

    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
    
    int8_t lastWiFi_id = novaXR_Rom.boardInfo.last_Cred_Pos;
    
    if ((lastWiFi_id == 0)&&(!novaXR_Rom.wifiCredentials[lastWiFi_id].CredState)){
      lastWiFi_id=-1;
    }

    return lastWiFi_id;
}


bool WiFi_ID_exists(int8_t id){

    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
    if(novaXR_Rom.wifiCredentials[id].CredState){
      return true;
    }else{
      return false;
    }
  
}


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





// --- Find the array position of the last Updated WiFi Credentials in Flash Memory --- 
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



// ------ Update Device Information -----------
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




// ------ Add a new Wifi in the Credentials List -----------
bool update_WifiCred(char *wifi_ssid, char *Password){
  
      bool flagCred=false;
      NXR_Rom novaXR_Rom;
      novaXR_Rom = Flash_handler.read();

      uint8_t new_pos = find_WiFiSpace();
      Serial.print("new_pos:  ");Serial.println(new_pos);
      
      String WifiSSID = String(wifi_ssid);
      WifiSSID.toCharArray(novaXR_Rom.wifiCredentials[new_pos].wifiSSID, WIFI_SIZE);

      String WifiPASS = String(Password);
      WifiPASS.toCharArray(novaXR_Rom.wifiCredentials[new_pos].wifiPASS, WIFI_SIZE);
      
      flagCred = novaXR_Rom.wifiCredentials[new_pos].CredState = true;

       Serial.print("last_empty_pos<CRED_SIZE");Serial.print(new_pos);Serial.print(" < ");Serial.println(CRED_SIZE);
      if(new_pos<CRED_SIZE){
        novaXR_Rom.boardInfo.last_Cred_Pos = new_pos;
      }else{
        Serial.print("FAIL-SAFE: Last Position 10 -> 0");
        novaXR_Rom.boardInfo.last_Cred_Pos = 0;
      }
      
      Flash_handler.write(novaXR_Rom);  // Write data to the Memory     
      return flagCred;
}





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



// ---- Read a specific WiFi network 
/*
void getSaved_WiFiCredentials(NXR_Rom *flash_data, uint8_t id){

  NXR_Rom RomData;
  RomData = Flash_handler.read();
  
  memcpy(flash_data->wifiCredentials[id].wifiSSID, RomData.wifiCredentials[id].wifiSSID, WIFI_SIZE);
  memcpy(flash_data->wifiCredentials[id].wifiPASS, RomData.wifiCredentials[id].wifiPASS, WIFI_SIZE);
 
}
*/




// ---- encode SSID ------- 
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


// ----- make a wifi password using the serial number of the device ----
char * CreatePASS(void) {
    NXR_Rom novaXR_Rom;
    novaXR_Rom = Flash_handler.read();
      
    char *Passwd;
    Passwd = (char *)malloc(sizeof (char) * 13); 
    sprintf(Passwd,"%s",novaXR_Rom.boardInfo.NXR_SerialNumber);
    return Passwd;
}



// -------- Get Unique SMAD21 Chip ID and Create a Unique Serial Numner ---------
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
