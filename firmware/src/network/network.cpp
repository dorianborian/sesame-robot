#include "network.h"
#include <ESPmDNS.h>
#include "../screen/screen.h"

NetworkMode networkMode     = ACCESS_POINT_MODE;
bool        networkConnected = false;
IPAddress   networkIP;
String      deviceHostname  = "sesame-robot";
DNSServer   dnsServer;

extern String wifiInfoText;

static void tryConnectStation() {
  if (!ENABLE_NETWORK_MODE || String(NETWORK_SSID).length() == 0) {
    WiFi.mode(WIFI_AP);
    Serial.println("Network mode disabled — AP only.");
    return;
  }

  Serial.println("Connecting to: " + String(NETWORK_SSID));
  WiFi.mode(WIFI_AP_STA);
  WiFi.setHostname(deviceHostname.c_str());
  WiFi.begin(NETWORK_SSID, NETWORK_PASS);

  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    networkConnected = true;
    networkMode      = NETWORK_MODE;
    networkIP        = WiFi.localIP();
    Serial.print("Connected! IP: ");
    Serial.println(networkIP);
  } else {
    Serial.println("Station connect failed — falling back to AP only.");
    WiFi.mode(WIFI_AP);
  }
}

static void buildWifiInfoText(const IPAddress& apIP) {
  if (networkConnected) {
    wifiInfoText = "AP: " + String(AP_SSID) +
                   " (" + apIP.toString() + ")  |  Network: " +
                   String(NETWORK_SSID) +
                   " (" + networkIP.toString() + ") or " +
                   deviceHostname + ".local  |  ";
  } else {
    wifiInfoText = "Connect to WiFi: " + String(AP_SSID) +
                   "  |  Pass: " + String(AP_PASS) +
                   "  |  IP: " + apIP.toString() +
                   "  |  Captive Portal will auto-open!  |  ";
  }
}

static void startAccessPoint() {
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP ready. IP: ");
  Serial.println(apIP);
  buildWifiInfoText(apIP);
}

static void startMDNS() {
  if (MDNS.begin(deviceHostname.c_str())) {
    MDNS.addService("http", "tcp", 80);
    Serial.print("mDNS: http://");
    Serial.print(deviceHostname);
    Serial.println(".local");
  } else {
    Serial.println("mDNS init failed.");
  }
}

static void startCaptiveDNS() {
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}

void initWiFi() {
  showSetupMessage("Setting up WiFi...");
  tryConnectStation();
  startAccessPoint();
  startMDNS();
  startCaptiveDNS();
}

void processDNS() {
  dnsServer.processNextRequest();
}
