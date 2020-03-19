#ifndef HTML_SERVER_H
#define HTML_SERVER_H

#define MAX_LINE_SIZE 1024


#include "NXR_Utility_Lib.h"
#include "NXR_Flash.h"
#include <string.h>




void fill_END_form(WiFiClient &client);
void fill_form (WiFiClient &client);
bool parse_HTML (String linebuf, char *ssid, char *pass);
void start_HTMLserver(char *ssidAP, char *passAP);
uint8_t register_New_WiFi (void);
void new_wifi_setup (char *ssid, char *pass);
String urldecode(String str);
unsigned char h2int(char c);
bool wait_for_Client(void);
bool check_Connected_Client(void);





uint8_t new_registerWiFi(void);


#endif
