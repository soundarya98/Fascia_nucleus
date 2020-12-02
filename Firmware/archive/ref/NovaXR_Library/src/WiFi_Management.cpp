#include "WiFi_Management.h"
#include "NXR_Peripherals.h"
#include "HTML_Server.h"
#include <SPI.h>
#include <WiFiNINA.h>


// ---------- Start a HTML-Based UI for Infrastructure Mode -----------------------

/**
 * @brief      Starts a Web-based WiFi configuration UI.
 */
void startHTMLserver(void){
    WiFiClient tcpClient;
    fill_form(tcpClient);
    printInstructions();  
}

/**
 * @brief      Initialize the WiFi of the NovaXR device
 */
void WiFi_Management_Init(void){
   bool connStatus = false;
  
   while(!connStatus){    
      int totalWiFi = count_SSIDsInList();
      Serial.print("Total WiFi:  ");Serial.println(totalWiFi); 
      inProgress_Indication();
      if(totalWiFi>0){  // ------ Connect to a saved WiFi SSID --------------
          Serial.println("Connecting ....");    
          uint8_t lastWiFi = lastWiFi_ID();
          
          for(uint8_t index=0; index<totalWiFi; index++){   //  get the last connected WiFi credential                      
                connStatus = connect_lastWiFi(lastWiFi);
                if (connStatus){   // -- Connected Successfully 
                  success_State(); // -- indicate connection successful
                  break;
                }else{             // -- Not Connected yet    
                  lastWiFi = previous_WiFi_ID(); 
                }
           }
           
           if(!connStatus){   //  if none of the stored SSIDs are near by then get in AP mode to register a new one
            Serial.println("Creating AP #0....");
            connStatus = start_HTMLserver(CreateSSID(), CreatePASS());
           }
               
      }else {  // ------ Create an WiFi network, get in AP mode --------------
        Serial.println("Creating AP #1....");
        connStatus = start_HTMLserver(CreateSSID(), CreatePASS());    
     }
        inProgress_Indication();
     }

}


/**
 * @brief    Prints instructions.
 */
void printInstructions(void){
      Serial.println(" ==== 1. Now Connect to the above SSID");
      Serial.println(" ==== 2. then hit htis link 'http://192.168.4.1' in your browser to register your NovaXR board to your home wifi....");  
}


/**
 * @brief   NovaXR WiFi Access Point Initialization 
 */
void WiFi_Management_AP_Init(void){
     
     Serial.println(" ==== Start AP Mode ====");
     while(!CreateAccessPoint(CreateSSID(), CreatePASS()));
     Serial.println(" ==== Client Connected ====");
}



/**
 * @brief      Creates a WiFi Access Point.
 *
 * @param      APssid  The WiFi AP Name 
 * @param      APpass  The WiFi AP Password
 *
 * @return     returns True if the device is connected succefully to that network 
 */
bool CreateAccessPoint(char APssid[], char APpass[]){
    int APstatus = WL_IDLE_STATUS;
    Serial.println("Access Point Device");
    
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      // don't continue
      while (true);
    }
  
    String fv = WiFi.firmwareVersion();
    if (fv < "1.0.0") {
      Serial.println("Please upgrade the firmware");
    }
  
    // by default the local IP address of will be 192.168.4.1
    // you can override it with the following:
    // WiFi.config(IPAddress(10, 0, 0, 1));
    Serial.print("Creating access point named: ");
    Serial.println(APssid);
  
    // Create open network. Change this line if you want to create an WEP network:
    APstatus = WiFi.beginAP(APssid, APpass);
    
    if (APstatus != WL_AP_LISTENING) {
      Serial.println("Creating Access Point failed");
      // don't continue
      while (1);
    }

     while(!check_For_Connected_Client()){
       // wait  for connection 
       delay(500);      
       if(check_For_Connected_Client()){
        break;
       }
     }
     delay(5000);
     
    // you're connected now, so print out the status
    printWiFiStatus();
    return true;
 }




/**
 * @brief    Check if any WiFi client is connected to NovaXR device
 *
 * @return   connFlag -> returns True if any wifi client is connected
 */
bool check_For_Connected_Client(void){
    int status = WL_IDLE_STATUS;
    bool connFlag = false;
    // compare the previous status to the current status
    if (status != WiFi.status()) {
        // it has changed update the variable
        status = WiFi.status();
    
        if (status == WL_AP_CONNECTED) {
          // a device has connected to the AP
          Serial.println("Device connected to AP");
          connFlag = true;
        } 
        
    }   
    return connFlag;
}



/**
 * @brief      Connect to a New WiFi Network
 *
 * @param      ssid  The WiFi SSID Network Name string
 * @param      pass  The WiFi Network password string
 */
void new_wifi_setup (char *ssid, char *pass){
    if (WiFi.status () == WL_NO_SHIELD)
    {
        Serial.println ("WiFi shield not present");
        // don't continue:
        while (true);
    }
    int status = WL_IDLE_STATUS;
    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED)
    {   
        WiFi_conn_inProgress_Indication();
        Serial.print ("Attempting to connect to SSID: ");
        Serial.println (ssid);
        Serial.print ("pass: ");
        Serial.println (pass);
        status = WiFi.begin(ssid, pass);
        delay (7000);
    }
    Serial.println("Connected to wifi");
}


/**
 * @brief     User can choose in which mode the device will operate
 *
 * @param     bypassCMD  The bypass command - Force the WiFi connectivity to Fixed operation
 */
void WiFi_connectivityOption(char bypassCMD){
   char modeSet=0;   
   while(1){ 
      delay(100);    
      if (Serial.available() > 0){
          // ---- Instructions ------------------------------------------------
          Serial.println("*********************************************************");
          Serial.println("\n------------------ WIFI INSTRUCTIONS --------------------"); 
          Serial.println("   Type 'A' for AP mode or 'I' for Infrastructure mode   ");
          modeSet = Serial.read();
       } else{
            modeSet = bypassCMD;
           if(modeSet=='A'){          
              WiFi_Management_AP_Init();
              break;
           }else if (modeSet=='I'){
              WiFi_Management_Init();
              break;
          } 
      }
   }
}



/**
 * @brief      Connects to a given WiFi network.
 *
 * @param      ssid  The WiFi SSID Network Name string
 * @param      pass  The WiFi Network password string
 *
 * @return     connState returns True if the device is connected succefully to that network 
 */
bool Connect_to_AP(char* ssid, char* pass){
  
      int keyIndex = 0;   // your network key Index number (needed only for WEP)
      int status = WL_IDLE_STATUS;
      bool connState = false;
      uint8_t attemptCount=0;
        
      Serial.print("Connecting to ");
      Serial.println(ssid);
       
      // check for the WiFi module:
      if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
      }
    
      String fv = WiFi.firmwareVersion();
      if (fv < "1.0.0") {
        Serial.println("Please upgrade the firmware");
      }
    
      // attempt to connect to Wifi network:
      while (status != WL_CONNECTED) {
          attemptCount++;
          Serial.print("Attempting to connect to SSID: ");
          Serial.println(ssid);
          // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
          status = WiFi.begin(ssid, pass);
      
          // wait 7 seconds for connection:
          WiFi_conn_inProgress_Indication();
          delay(7000);
          
          if (attemptCount>3) {
            connState = false;
            break;
          }else{
            connState = true;
          }
      }
    
      if(connState) {
        Serial.println("");
        Serial.println("WiFi connected"); 
          Serial.print("NovaXR IP address: ");
        Serial.println(WiFi.localIP());
      }else{
        Serial.println("Failed to Connect"); 
      }
      
  return connState;
}



/**
 * @brief   Prints a WiFi status.
 */
void printWiFiStatus(void) {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

   // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print your WiFi shield's IP address:
  IPAddress serverip = WiFi.localIP();
  Serial.print("Server IP Address: ");
  Serial.println(serverip);

}
