#include "HTML_Server.h"
#include "WiFi_Management.h"
#include "LED_Indication.h"


#define NOT_CONN      0   ///< Not connected state
#define CONN_FAILED   1   ///< Connection Failed state
#define CONN_SUCCESS  2   ///< Successfull Connection state
#define HTTPPORT      80  ///< Server Port Definition



// --- Global Variables - Status Global Flags
int status = WL_IDLE_STATUS;
uint8_t connectionFlag = 0;
bool APrelease_Flag = false;
bool clientConn_Flag = false;
int i=0;

WiFiServer HTTPserver(HTTPPORT); ///< WiFi Server Class Instance


/**
 * @brief      Start HTML based User Inτεreface Screen for Initial WiFi setup.
 *
 * @param      ssidAP  The SSID WiFi Acess Point to be created
 * @param      passAP  The WiFi Password for Access Point to be assigned
 *
 * @return     WiFiConnectionFlag returns True if the NovaXR device is successfully connected to the given AP WiFi network
 */
bool start_HTMLserver(char *ssidAP, char *passAP){
    bool WiFiConnectionFlag = false;
    Serial.println("Start HTTP Server !!"); 
    HTTPserver.begin ();       
    while(connectionFlag!=CONN_SUCCESS){                     
          // Get the Device in AP mode when it is not able to connect to another AP
          if(!APrelease_Flag){ 
             Serial.println(" == Start AP Mode ==");
             status = WiFi.status();
             CreateAccessPoint(ssidAP, passAP);
             APrelease_Flag=true;
             clientConn_Flag=wait_for_Client(); // if client is connected the AP mode is released                        
           }else{  
               connectionFlag = register_New_WiFi();   // register a new wifi in the device memory and try to connect to it                   
               if(connectionFlag == CONN_FAILED) {
                 WiFiConnectionFlag = false;
                 APrelease_Flag=false;   // emnable AP mode again   
                }
                
               if(connectionFlag == CONN_SUCCESS) {
                 WiFiConnectionFlag = true;
                 break;
               }            
          }                   
     }
  
     Serial.println("WiFi Config is Done !! "); 
     return  WiFiConnectionFlag;
}


      
/**
 * @brief      Wait for a Client to be connected
 *
 * @return     Conn_flag returns True if a WiFi device client is conneted to the NovaXR network
 */
bool wait_for_Client(void){
    bool Conn_flag=false;
    // Check if a device is still connected to the AP 
    status = WiFi.status();
     if(status != WL_AP_CONNECTED){    
        
        while (status != WL_AP_CONNECTED) {   
          status = WiFi.status();
     
          if (status == WL_AP_CONNECTED) {
            // a device has connected to the AP
            Serial.println("Device connected to AP");
            Conn_flag = true;
            break;
          }else {
              // a device has disconnected from the AP, and we are back in listening mode
              Conn_flag = false;
          }           
          delay(10);
          inProgress_Indication();   ///< AP mode - Waiting For clients State Indication  
        }
     }else{
        Conn_flag=true;    
       }
       
   return Conn_flag;         
}  




/**
 * @brief      Check If a WiFi Client is connected to the created AP
 *
 * @return     Conn_flag returns True if a WiFi device client is conneted to the NovaXR network
 */
bool check_Connected_Client(void){
      bool Conn_flag=false;
      status = WiFi.status();
      Serial.print("Status Check Point?;  ");Serial.println(status);
      if (status == WL_AP_CONNECTED) {
        Serial.println("Still Connected");
        // a device has connected to the AP
        Conn_flag = true;
      } else {
        // a device has disconnected from the AP, and we are back in listening mode              
        Serial.println("disconnected"); 
        Conn_flag = false;
      }           
    
   return Conn_flag;         
}




/**
 * @brief      Register a New WiFi in your device and save it in the device flash memory
 *
 * @return     connectedFlag returns True if the NovaXR device is conneted to the given WiFi network
 */
uint8_t register_New_WiFi (void) {
  
    char ssid[WIFI_SIZE];
    char pass[WIFI_SIZE];
    bool registeredFlag = false;
    uint8_t connectedFlag  = 0;
    String linebuf = "";

   
  WiFiClient HTTP_client = HTTPserver.available ();
  
    while (HTTP_client.connected()) {
      
       if (HTTP_client.available ()) {
            char c = HTTP_client.read ();            
          if (c == '\n'){         
              if (linebuf.length () == 0){                  
                  fill_form(HTTP_client);
                  break;
                  
              }else{   
                      
                  if (!parse_HTML(linebuf, ssid, pass)){                
                  } else { // TODO add timeout to new_wifi_setup and check return code to handle wrong credentials                                    
                     Serial.println("HTML Parsed ... "); 
                     registeredFlag = update_WifiCred(ssid, pass);
                     displaySaved_WiFiCredentails();           
                     if(registeredFlag)break; 
                     
                  }
                  linebuf = "";   
               }
              
            }else if (c != '\r'){
                linebuf += c;
            }
      
        }else{
            linebuf = "";
        }
       }  
    

       if(registeredFlag) {    
          
          Serial.println("try to connect .... ");
         if(Connect_to_AP(ssid, pass)){
          connectedFlag = CONN_SUCCESS; ///< successfull connection after WiFi registration
          displaySaved_WiFiCredentails();
         }else{
          connectedFlag = CONN_FAILED;  ///< failed to connect after WiFi registration
         } 
      }
    
    HTTP_client.stop(); 
    return connectedFlag ;
}




/**
 * @brief      Post HTML-Based User Interface - Setup Page
 *
 * @param      http_client  The http client network class instance
 */
void fill_form(WiFiClient &http_client) {

    http_client.println("HTTP/1.1 200 OK");
    http_client.println("Content-Type: text/html");
    http_client.println();
    http_client.println("<!DOCTYPE HTML>");
    http_client.println("<html>");
    http_client.println("     Nova XR - AP Mode  <br>");
    http_client.println ("<html><head></head><body>");
    http_client.println ("<br>");
    http_client.println("  Enter Your WiFi Credentials:  <br>");
    http_client.println ("<br>");
    http_client.print ("<form method=get>");
    http_client.print ("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SSID: &nbsp;&nbsp;&nbsp;&nbsp;<input type='text' name=ssid value='''><br>");
    http_client.print ("Password: &nbsp;&nbsp;&nbsp;&nbsp;<input type='text' name=password value=''><br>");
    http_client.println ();
    http_client.print ("&nbsp;&nbsp;&nbsp;<input type=submit value=submit></form>");
    http_client.print ("</body></html>");
}


/**
 * @brief      Post HTML-Based User Interface - Thank you Page
 *
 * @param      http_client  The http client network class instance
 */
void fill_END_form(WiFiClient &http_client) {
    Serial.println("  ========================  THANK YOU ===================== ");
    http_client.println("HTTP/1.1 200 OK");
    http_client.println("Content-Type: text/html");
    http_client.println();
    http_client.println("<!DOCTYPE HTML>");
    http_client.println("<html>");
    http_client.println("     Nova XR - AP Mode  <br>");
    http_client.println ("<html><head></head><body>");
    http_client.println ("<br>");
    http_client.print("<form method=get>");
    http_client.println("  Thank You  <br>");
    http_client.print ("</body></html>");
}



/**
 * @brief      Parse the user entered WiFi credentials from the HTML UI page
 *
 * @param[in]  linebuf  The string buffer 
 * @param      ssid     The given WiFi SSID name to be stored in this string
 * @param      pass     The given WiFi Password to be stored in this string
 *
 * @return     parsed -> returns True if the Credentials are parsed correctly
 */
bool parse_HTML (String linebuf, char *ssid, char *pass){
  
    bool parsed = true;
    // expected string format http://192.168.4.1/?ssid=%ssid%&password=%password%
    String password_start_phrase = "&password=";
    String ssid_start_phrase = "?ssid=";
    String endString =" HTTP/1.1";

    if (linebuf.indexOf ("?ssid") != -1) {         
        // --- parse password
        int password_start_index = linebuf.indexOf (password_start_phrase);
        
        if (password_start_index == -1){
            parsed = false;

        }else {        
            int end_String_index = linebuf.indexOf (endString);
            String password = linebuf.substring (password_start_index + password_start_phrase.length (), end_String_index);
            password=urldecode(password);   ///< Decode password in UTF-8
            strcpy (pass, password.c_str());

            // parse ssid
            int ssid_start_index = linebuf.indexOf (ssid_start_phrase);
            if (ssid_start_index == -1) { ///< SSID is not found in string
                parsed = false;
                
            } else {
                String net_id = linebuf.substring (ssid_start_index + ssid_start_phrase.length (), password_start_index);
                strcpy (ssid, net_id.c_str ());
                for(uint8_t p=0; p<WIFI_SIZE; p++){
                  if(ssid[p]=='+')ssid[p]=' ';
                }
                bool parsed = true;
            }       
        }    
    }else{
      parsed = false;
    }

    return parsed;
}



/**
 * @brief      Decode the URL user data
 *
 * @param[in]  str The given string to be decoded
 *
 * @return     decodedString -> returned Decoded string
 */
String urldecode(String str){
    
    String decodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        decodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        decodedString+=c;
      } else{
        
        decodedString+=c;  
      }
      
      yield();
    }
    
   return decodedString;
}



/**
 * @brief     trnaslate a HEX Character to a real ASCII character
 *
 * @param     c  Input HEX character
 *
 * @return    returns an ASCII character
 */
unsigned char h2int(char c){
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
