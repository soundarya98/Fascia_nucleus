#include "HTML_Server.h"
#include "Arduino.h"

#define NOT_CONN 0
#define CONN_FAILED 1
#define CONN_SUCCESS 2


#define HTTPPORT 80

int status = WL_IDLE_STATUS;
uint8_t connectionFlag = 0;
bool APrelease_Flag = false;
bool clientConn_Flag = false;
WiFiServer HTTPserver(HTTPPORT);
int led =  LED_BUILTIN;

int i=0;

// ---- Start HTML based User Input Screen ----------
void start_HTMLserver(char *ssidAP, char *passAP){
     
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
             if(connectionFlag == CONN_FAILED) APrelease_Flag=false;   // emnable AP mode again   
             if(connectionFlag == CONN_SUCCESS) break;
            
          }
                        
      }

     Serial.println("WiFi Config is Done !! "); 

}


      







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
          } else {
              // a device has disconnected from the AP, and we are back in listening mode
              Conn_flag = false;
          }           
          delay(10);
        }
     }else{
        Conn_flag=true;    
       }
       
   return Conn_flag;         
}  





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










// ---- Register a New WiFi in your device and save it in the device flash memory --------
uint8_t register_New_WiFi (void) {
  
    char ssid[WIFI_SIZE];
    char pass[WIFI_SIZE];
    bool registeredFlag = false;
    uint8_t connectedFlag  = 0;
    String linebuf = "";

   
  WiFiClient HTTP_client = HTTPserver.available ();
   //if(HTTP_client){ // new client connected
 
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
          connectedFlag = CONN_SUCCESS; // successfull connection after WiFi registration
          displaySaved_WiFiCredentails();
         }else{
          connectedFlag = CONN_FAILED;  // failed to connect after WiFi registration
         } 
      }
    
    HTTP_client.stop(); 
    return connectedFlag ;
}







// ---- User Input HTML Page ----------
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


  // ---- User Input HTML Page ----------
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

// --- Parse the user entered wifi credentials HTMP page ------------
bool parse_HTML (String linebuf, char *ssid, char *pass){
  
    bool parsed = true;
    // expected string format http://192.168.4.1/?ssid=%ssid%&password=%password%
    String password_start_phrase = "&password=";
    String ssid_start_phrase = "?ssid=";
    String endString =" HTTP/1.1";

    if (linebuf.indexOf ("?ssid") != -1) {         
        // parse password
        int password_start_index = linebuf.indexOf (password_start_phrase);
        
        if (password_start_index == -1){
            parsed = false;

        }else {        
            int end_String_index = linebuf.indexOf (endString);
            String password = linebuf.substring (password_start_index + password_start_phrase.length (), end_String_index);
            password=urldecode(password);   // Decode password in UTF-8
            strcpy (pass, password.c_str());
            Serial.print ("WiFi Passord:");Serial.println (password);

            // parse ssid
            int ssid_start_index = linebuf.indexOf (ssid_start_phrase);
            if (ssid_start_index == -1) { //SSID is not found in string
                parsed = false;
                
            } else {
                String net_id = linebuf.substring (ssid_start_index + ssid_start_phrase.length (), password_start_index);
                strcpy (ssid, net_id.c_str ());
                Serial.print ("WiFi SSID:");Serial.println (net_id);
                bool parsed = true;
            }
            
        }    
    }else{
      parsed = false;
    }

    return parsed;
}





// --- Decode the URL user data ----- 
String urldecode(String str){
    
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}




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
