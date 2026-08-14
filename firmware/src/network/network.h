#pragma once
#include <DNSServer.h>
#include <WiFi.h>

#define NETWORK_SSID        ""        // Your WiFi SSID
#define NETWORK_PASS        ""        // Your WiFi password
#define ENABLE_NETWORK_MODE false     // Set true to enable station mode

#define AP_SSID  "Sesame-Controller"
#define AP_PASS  "12345678"           // min 8 characters

#define DNS_PORT ((byte)53)

typedef enum { ACCESS_POINT_MODE, NETWORK_MODE } NetworkMode;

extern NetworkMode networkMode;
extern bool        networkConnected;
extern IPAddress   networkIP;
extern String      deviceHostname;
extern DNSServer   dnsServer;

void initWiFi();
void processDNS();
