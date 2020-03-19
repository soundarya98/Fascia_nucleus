#include <string.h>

#include <SPI.h>
#include <WiFiNINA.h> // EXTERNAL DEPENDENCY NEED TO INSTALL FROM LIBRARY MANAGER
#include <WiFiUdp.h>

#include "connect.h"
#include "ArduinoUniqueID.h"

#define AP_SSID "OpenBCI_NOVAXR"


WiFiServer server (80);

void fill_form (WiFiClient &client)
{
    client.println ("HTTP/1.1 200 OK");
    client.println ("Content-Type: text/html");
    client.println ("<html><head></head><body>");
    client.println ();
    client.print ("<form method=get>");
    client.print ("<input type='text' name=ssid value='''> ssid<br>");
    client.print ("<input type='text' name=password value=''> password<br>");
    client.print ("<input type=submit value=submit></form>");
    client.print ("</body></html>");
}

// TODO handle url encode
bool parse_html (String linebuf, char *ssid, char *pass)
{
    bool parsed = true;
    // expected string format http://192.168.4.1/?ssid=%ssid%&password=%password%
    String password_start_phrase = "&password=";
    String ssid_start_phrase = "?ssid=";
    if (linebuf.indexOf ("192.168.4.1") != -1)
    {
        // parse password
        int password_start_index = linebuf.indexOf (password_start_phrase);
        if (password_start_index == -1)
        {
            Serial.print ("password is not found in string:");
            Serial.println (linebuf);
            parsed = false;
        }
        else
        {
            String password = linebuf.substring (password_start_index + password_start_phrase.length ());
            strcpy (pass, password.c_str ());
            Serial.print ("pass:");
            Serial.println (password);

            // parse ssid
            int ssid_start_index = linebuf.indexOf (ssid_start_phrase);
            if (ssid_start_index == -1)
            {
                Serial.print ("ssid is not found in string:");
                Serial.println (linebuf);
                parsed = false;
            }
            else
            {
                String net_id = linebuf.substring (ssid_start_index + ssid_start_phrase.length (), password_start_index);
                strcpy (ssid, net_id.c_str ());
                Serial.print ("ssid:");
                Serial.println (net_id);
            }
        }
    }
    else
    {
        Serial.print ("ip addr is not found in string:");
        Serial.println (linebuf);
        parsed = false;
    }
    return parsed;
}

// TODO keep provided credentials to dont ask user type it every time
bool register_new_wifi (char *ssid, char *pass) 
{
    WiFiClient client = server.available ();
    bool connected = false;
    String linebuf = "";
    while (client.connected ())
    {
        if (client.available ())
        {
            char c = client.read ();
            if (c == '\n')
            {
                if (linebuf.length () == 0)
                {
                    fill_form (client);
                    Serial.println ("form was sent");
                    break;
                }
                else
                {
                    if (!parse_html (linebuf, ssid, pass))
                    {
                        Serial.println ("failed to parse");
                    }
                    else
                    {
                        // TODO add timeout to new_wifi_setup and check return code to handle wrong credentials
                        new_wifi_setup (ssid, pass);
                        connected = true;
                        break;
                    }
                    linebuf = "";
                }
            }
            else if (c != '\r')
            {
                linebuf += c;
            }
        }
        else
        {
            linebuf = "";
        }
    }
    client.stop ();
    return connected;
}

void ap_wifi_setup ()
{
    String ap_ssid = "OpenBCINovaXR-";
    UniqueIDdump (Serial);
    // first 8 should be enough I suppose. if not we can ise bit ops to create shorter id
    for (size_t i = 0; i < 8; i++)
    {
        String hex = String (UniqueID[i], HEX);
        ap_ssid += hex;
    }
    Serial.print ("Created AP net named:");
    Serial.println (ap_ssid);

    if (WiFi.status () == WL_NO_MODULE)
    {
        Serial.println ("Communication with WiFi module failed!");
        while (true);
    }
    String fv = WiFi.firmwareVersion ();
    if (fv < "1.0.0")
    {
        Serial.println ("Please upgrade the firmware");
    }
    int status = WiFi.beginAP (ap_ssid.c_str ());
    if (status != WL_AP_LISTENING)
    {
        Serial.println ("Creating access point failed");
        while (true);
    }
    delay (10000);
    server.begin ();

}

void new_wifi_setup (char *ssid, char *pass)
{
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
        status = WiFi.begin (ssid, pass);
        delay (1000);
    }
    Serial.println("Connected to wifi");
}
