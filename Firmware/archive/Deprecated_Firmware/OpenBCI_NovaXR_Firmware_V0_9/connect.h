#ifndef CONNECT
#define CONNECT

#define MAX_LINE_SIZE 1024

#include <WiFi.h>

void fill_form (WiFiClient &client);
bool parse_html (String linebuf, char *ssid, char *pass);
bool register_new_wifi (char *ssid, char *pass);
void ap_wifi_setup (); // wifi provided by board
void new_wifi_setup (char *ssid, char *pass); // connect to local net

#endif
