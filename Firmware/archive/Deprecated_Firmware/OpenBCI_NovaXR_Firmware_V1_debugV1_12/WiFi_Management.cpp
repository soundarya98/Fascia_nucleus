
#include "WiFi_Management.h"
#include "HTML_Server.h"
#include <SPI.h>
#include <WiFiNINA.h>




// ----------------------------------------------------------------
void startHTMLserver(void){
  WiFiClient tcpClient;
  fill_form(tcpClient);
  
}


void WiFi_Management_Init(void){

   bool connStatus = false;
   while(!connStatus){
   int totalWiFi = count_SSIDsInList();
   Serial.print("Total WiFi:  ");Serial.println(totalWiFi); 

   if(totalWiFi>0){  // ------ Connect to a saved WiFi SSID --------------
        Serial.println("Connecting ....");
        // get the last connected WiFi credential
        uint8_t lastWiFi = lastWiFi_ID();
        for(uint8_t index=0; index<totalWiFi; index++){  
              //Serial.print("connect_lastWiFi: # ");Serial.println(lastWiFi);             
              connStatus = connect_lastWiFi(lastWiFi);
              if (connStatus){ // -- Connected Successfully 
                break;
              }else{           // -- Not Connected yet    
                lastWiFi = previous_WiFi_ID(); 
              }
         }
         
         if(!connStatus){ // if none of the stored SSIDs are near by then get in AP mode to register a new one
          Serial.println("Creating AP #0....");
          connStatus = start_HTMLserver(CreateSSID(), CreatePASS());
         }
             
    }else {  // ------ Create an WiFi network, get in AP mode --------------
      Serial.println("Creating AP #1....");
      connStatus = start_HTMLserver(CreateSSID(), CreatePASS());    
   }
      
   }

  
}




bool CreateAccessPoint(char APssid[], char APpass[]){
  int APstatus = WL_IDLE_STATUS;
  Serial.println("Access Point Device");
  
  //pinMode(led, OUTPUT);      // set the LED pin mode

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
  
  // wait 10 seconds for connection:
  delay(10000);
  
   // you're connected now, so print out the status
  printWiFiStatus();
  return true;
  
 }




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
        Serial.print ("Attempting to connect to SSID: ");
        Serial.println (ssid);
        Serial.print ("pass: ");
        Serial.println (pass);
        status = WiFi.begin(ssid, pass);
        delay (10000);
    }
    Serial.println("Connected to wifi");
}



bool Connect_to_AP(char* ssid, char* pass){
  
      int keyIndex = 0;   // your network key Index number (needed only for WEP)
      int status = WL_IDLE_STATUS;
      unsigned int UDPPort = NETPORT;
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
    
        // wait 10 seconds for connection:
        delay(10000);
        
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
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
      }else{
        Serial.println("Failed to Connect"); 
      }
      
  return connState;
}




void printWiFiStatus(void) {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}
