#ifndef DATAL_HANDLER_H
#define DATA_HANDLER_H



// ------- Network Stuff ------------------------------- //
void printWiFiStatus(void);
bool Connect_to_AP(char *ssid, char *pass);
void new_wifi_setup (char *ssid, char *pass);
bool CreateAccessPoint(char APssid[], char APpass[]);
void startHTMLserver(void);
void WiFi_Management_Init(void);
void WiFi_Management_AP_Init(void);
void WiFi_connectivityOption(void);



#endif
