// Modified by BotInfo.ai, 2026-11-14: motion watchdog (a continuous move sent
// to /api/command stops and stands up if no /api/command is accepted for
// motionTimeout ms; re-send the move to keep going). See motion-watchdog.h.
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "face-bitmaps.h"
#include "movement-sequences.h"
#include "captive-portal.h"
#include "motion-watchdog.h"

// --- Access Point Configuration ---
// This is the network the Robot will create
#define AP_SSID  "Sesame-Controller"
#define AP_PASS  "12345678" // Must be at least 8 characters

// --- Station Mode Configuration (Optional) ---
// Set these to connect to your home/office WiFi network
// Leave NETWORK_SSID empty to disable station mode
#define NETWORK_SSID ""  // Your WiFi network name
#define NETWORK_PASS ""  // Your WiFi password
#define ENABLE_NETWORK_MODE false  // Set to true to enable network connection attempts

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDR 0x3C

// I2C Pins for Distro Board V2 / V3
//#define I2C_SDA 8
//#define I2C_SCL 9

// I2C Pins for Distro Board V1
//#define I2C_SDA 21
//#define I2C_SCL 22

// I2C Pins for S2 Mini Board
#define I2C_SDA 33
#define I2C_SCL 35


// DNS Server for Captive Portal
DNSServer dnsServer;
const byte DNS_PORT = 53;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

// Global state for animations
String currentCommand = "";
String currentFaceName = "default";
const unsigned char* const* currentFaceFrames = nullptr;
uint8_t currentFaceFrameCount = 0;
uint8_t currentFaceFrameIndex = 0;
unsigned long lastFaceFrameMs = 0;
int faceFps = 8;
FaceAnimMode currentFaceMode = FACE_ANIM_LOOP;
int8_t faceFrameDirection = 1;
bool faceAnimFinished = false;
int currentFaceFps = 0;
bool idleActive = false;
bool idleBlinkActive = false;
unsigned long nextIdleBlinkMs = 0;
uint8_t idleBlinkRepeatsLeft = 0;

// WiFi Info Scrolling
unsigned long lastInputTime = 0;
bool firstInputReceived = false;
bool showingWifiInfo = false;
int wifiScrollPos = 0;
unsigned long lastWifiScrollMs = 0;
String wifiInfoText = "";

// Network Mode
bool networkConnected = false;
IPAddress networkIP;
String deviceHostname = "sesame-robot";
bool mdnsOk = false;

// Runtime WiFi provisioning (web UI) — the connect attempt runs as a state
// machine driven from loop() so HTTP handlers never block the captive portal.
enum WifiSetupState { WIFI_SETUP_IDLE, WIFI_SETUP_QUEUED, WIFI_SETUP_CONNECTING };
WifiSetupState wifiSetupState = WIFI_SETUP_IDLE;
String wifiSetupSsid = "";
String wifiSetupPass = "";
String wifiSetupError = "";          // result of the last finished attempt ("" = none/success)
unsigned long wifiSetupQueuedMs = 0;
unsigned long wifiSetupStartMs = 0;
bool wifiRestoreApOnly = false;      // drop the station iface again after an AP-only scan
const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
const uint32_t WIFI_SETUP_START_DELAY_MS = 300;  // let the HTTP response flush before the AP channel may hop

// Servo Pins for Distro Board
// ======================================================================
// Pin numbers are coorisponding to the ESP32 GPIO pins and may differ based on which board you use.
// If you are using a different board, please adjust the servoPins array accordingly.
// ======================================================================
Servo servos[8];
// Sesame Distro Board V3 Pinout [NEW]
//const int servoPins[8] = {4, 5, 6, 7, 10, 11, 12, 13};

// Sesame Distro Board V2 Pinout (Legacy)
//const int servoPins[8] = {4, 5, 6, 7, 15, 16, 17, 18};

// Sesame Distro Board V1 Pinout (Legacy)
//const int servoPins[8] = {15, 2, 23, 19, 4, 16, 17, 18};

// Lolin S2 Mini Pinout
const int servoPins[8] = {1, 2, 4, 6, 8, 10, 13, 14};

// Subtrim values for each servo (offset in degrees)
int8_t servoSubtrim[8] = {0, 0, 0, 0, 0, 0, 0, 0};


// Animation constants
int frameDelay = 100;
int walkCycles = 10;
int motorCurrentDelay = 20; // ms delay between motor movements to prevent over-current
MotionWatchdog motionWatchdog(MOTION_TIMEOUT_MS); // stops API moves when commands stop arriving

struct FaceEntry {
  const char* name;
  const unsigned char* const* frames;
  uint8_t maxFrames;
};

static const uint8_t MAX_FACE_FRAMES = 6;

#define MAKE_FACE_FRAMES(name) \
  const unsigned char* const face_##name##_frames[] = { \
    epd_bitmap_##name, epd_bitmap_##name##_1, epd_bitmap_##name##_2, \
    epd_bitmap_##name##_3, epd_bitmap_##name##_4, epd_bitmap_##name##_5 \
  };

#define X(name) MAKE_FACE_FRAMES(name)
FACE_LIST
#undef X
#undef MAKE_FACE_FRAMES

const FaceEntry faceEntries[] = {
#define X(name) { #name, face_##name##_frames, MAX_FACE_FRAMES },
  FACE_LIST
#undef X
  { "default", face_defualt_frames, MAX_FACE_FRAMES }
};

struct FaceFpsEntry {
  const char* name;
  uint8_t fps;
};

const FaceFpsEntry faceFpsEntries[] = {
  { "walk", 1 },
  { "rest", 1 },
  { "swim", 1 },
  { "dance", 1 },
  { "wave", 1 },
  { "point", 5 },
  { "stand", 1 },
  { "cute", 1 },
  { "pushup", 1 },
  { "freaky", 1 },
  { "bow", 1 },
  { "worm", 1 },
  { "shake", 1 },
  { "shrug", 1 },
  { "dead", 2 },
  { "crab", 1 },
  { "idle", 1 },
  { "idle_blink", 7 },
  { "default", 1 },
  // Conversational faces (manually controlled by Python - no auto-animation)
  { "happy", 1 },
  { "talk_happy", 1 },
  { "sad", 1 },
  { "talk_sad", 1 },
  { "angry", 1 },
  { "talk_angry", 1 },
  { "surprised", 1 },
  { "talk_surprised", 1 },
  { "sleepy", 1 },
  { "talk_sleepy", 1 },
  { "love", 1 },
  { "talk_love", 1 },
  { "excited", 1 },
  { "talk_excited", 1 },
  { "confused", 1 },
  { "talk_confused", 1 },
  { "thinking", 1 },
  { "talk_thinking", 1 },
};


// Prototypes
void setServoAngle(uint8_t channel, int angle);
void updateFaceBitmap(const unsigned char* bitmap);
void setFace(const String& faceName);
void setFaceMode(FaceAnimMode mode);
void setFaceWithMode(const String& faceName, FaceAnimMode mode);
void updateAnimatedFace();
void delayWithFace(unsigned long ms);
void enterIdle();
void exitIdle();
void updateIdleBlink();
int getFaceFpsForName(const String& faceName);
bool pressingCheck(String cmd, int ms);
void checkMotionWatchdog();
void handleGetSettings();
void handleSetSettings();
void handleGetStatus();
void handleApiCommand();
void updateWifiInfoScroll();
void recordInput();
bool connectToWifi(const String& ssid, const String& pass, uint32_t timeoutMs = 10000);
void handleWifiScan();
void handleWifiConnect();
void handleWifiStatus();
void handleNotFound();
String jsonEscape(const String& s);
bool startMdns();
void announceNetwork(const String& ssid);
void setApOnlyInfoText();
void showWifiInfoNow();
void updateWifiSetup();
void finishWifiSetup(const String& err);

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleCommandWeb() {
  // We send 200 OK immediately so the web browser doesn't hang waiting for animation to finish
  if (server.hasArg("pose")) {
    currentCommand = server.arg("pose");
    recordInput();
    exitIdle();
    server.send(200, "text/plain", "OK"); 
  } 
  else if (server.hasArg("go")) {
    currentCommand = server.arg("go");
    motionWatchdog.disarm(); // hold-to-move: the portal sends its own stop on release
    recordInput();
    exitIdle();
    server.send(200, "text/plain", "OK");
  } 
  else if (server.hasArg("stop")) {
    currentCommand = "";
    recordInput();
    server.send(200, "text/plain", "OK");
  }
  else if (server.hasArg("motor") && server.hasArg("value")) {
    int motorNum = server.arg("motor").toInt();
    int servoIdx = servoNameToIndex(server.arg("motor"));
    int angle = server.arg("value").toInt();
    if (motorNum >= 1 && motorNum <= 8 && angle >= 0 && angle <= 180) {
      setServoAngle(motorNum - 1, angle); // Convert 1-based to 0-based index
      recordInput();
      server.send(200, "text/plain", "OK");
    } else if (servoIdx != -1 && angle >= 0 && angle <= 180) {
      setServoAngle(servoIdx, angle);
      recordInput();
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Invalid motor or angle");
    }
  }
  else {
    server.send(400, "text/plain", "Bad Args");
  }
}

void handleGetSettings() {
  String json = "{";
  json += "\"frameDelay\":" + String(frameDelay) + ",";
  json += "\"walkCycles\":" + String(walkCycles) + ",";
  json += "\"motorCurrentDelay\":" + String(motorCurrentDelay) + ",";
  json += "\"faceFps\":" + String(faceFps);
  json += ",\"motionTimeout\":" + String(motionWatchdog.timeoutMs());
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetSettings() {
  if (server.hasArg("frameDelay")) frameDelay = server.arg("frameDelay").toInt();
  if (server.hasArg("walkCycles")) walkCycles = server.arg("walkCycles").toInt();
  if (server.hasArg("motorCurrentDelay")) motorCurrentDelay = server.arg("motorCurrentDelay").toInt();
  if (server.hasArg("faceFps")) faceFps = (int)max(1L, server.arg("faceFps").toInt());
  if (server.hasArg("motionTimeout")) motionWatchdog.setTimeoutMs(clampMotionTimeoutMs(server.arg("motionTimeout").toInt()));
  server.send(200, "text/plain", "OK");
}

// API endpoint for network clients to get robot status
void handleGetStatus() {
  String json = "{";
  json += "\"currentCommand\":\"" + currentCommand + "\",";
  json += "\"currentFace\":\"" + currentFaceName + "\",";
  json += "\"networkConnected\":" + String(networkConnected ? "true" : "false") + ",";
  json += "\"apIP\":\"" + WiFi.softAPIP().toString() + "\"";
  if (networkConnected) {
    json += ",\"networkIP\":\"" + networkIP.toString() + "\"";
  }
  json += "}";
  server.send(200, "application/json", json);
}

// API endpoint for network clients to send commands (JSON-based)
void handleApiCommand() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  
  Serial.println("API Command received:");
  Serial.println(body);
  
  // Check for face-only command (no movement)
  int faceOnlyStart = body.indexOf("\"face\":\"");
  if (faceOnlyStart == -1) {
    faceOnlyStart = body.indexOf("\"face\": \"");
  }
  
  // If we have a face but no command field, it's face-only
  bool faceOnly = (faceOnlyStart > 0 && body.indexOf("\"command\":") == -1 && body.indexOf("\"command\": ") == -1);
  
  String command = "";
  String face = "";
  
  // Parse face
  if (faceOnlyStart > 0) {
    faceOnlyStart = body.indexOf("\"", faceOnlyStart + 6) + 1;
    int faceEnd = body.indexOf("\"", faceOnlyStart);
    if (faceEnd > faceOnlyStart) {
      face = body.substring(faceOnlyStart, faceEnd);
      Serial.print("Parsed face: ");
      Serial.println(face);
    }
  }
  
  // Parse command (if not face-only)
  if (!faceOnly) {
    int cmdStart = body.indexOf("\"command\":\"");
    if (cmdStart == -1) {
      cmdStart = body.indexOf("\"command\": \"");
    }
    
    if (cmdStart == -1) {
      Serial.println("Error: command field not found");
      server.send(400, "application/json", "{\"error\":\"Missing command field\"}");
      return;
    }
    
    cmdStart = body.indexOf("\"", cmdStart + 10) + 1;
    int cmdEnd = body.indexOf("\"", cmdStart);
    
    if (cmdEnd <= cmdStart) {
      Serial.println("Error: invalid command format");
      server.send(400, "application/json", "{\"error\":\"Invalid command format\"}");
      return;
    }
    
    command = body.substring(cmdStart, cmdEnd);
    Serial.print("Parsed command: ");
    Serial.println(command);
  }
  
  // Set face if provided
  if (face.length() > 0) {
    setFace(face);
  }
  
  motionWatchdog.feed(millis()); // any accepted command shows the controller is alive

  // If face-only, just acknowledge
  if (faceOnly) {
    recordInput();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Face updated\"}");
    return;
  }
  
  // Execute command
  if (command == "stop") {
    currentCommand = "";
    recordInput();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command stopped\"}");
  } else {
    currentCommand = command;
    if (isContinuousMove(command.c_str())) motionWatchdog.arm(millis());
    recordInput();
    exitIdle();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command executed\"}");
  }
}

// Escape a string for embedding in a JSON string literal. Handles backslash,
// double-quote, and control characters (SSIDs are arbitrary octets — a nearby
// network with a newline in its name must not break the whole response).
String jsonEscape(const String& s) {
  String out;
  out.reserve(s.length() + 8);
  for (unsigned int i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '\\') out += "\\\\";
    else if (c == '"') out += "\\\"";
    else if ((uint8_t)c < 0x20) {
      char buf[8];
      snprintf(buf, sizeof(buf), "\\u%04x", (uint8_t)c);
      out += buf;
    } else out += c;
  }
  return out;
}

// Start (or restart, after MDNS.end()) the mDNS responder. Tracks the result
// in mdnsOk so the API can avoid advertising a .local name that won't resolve.
bool startMdns() {
  mdnsOk = MDNS.begin(deviceHostname.c_str());
  if (mdnsOk) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS responder started: http://" + deviceHostname + ".local");
  } else {
    Serial.println("Error setting up mDNS responder!");
  }
  return mdnsOk;
}

// Post-join side effects shared by the boot path and the web provisioning
// path: re-announce mDNS on the new station interface and rebuild the OLED
// scroll text for the joined network.
void announceNetwork(const String& ssid) {
  MDNS.end();
  startMdns();
  wifiInfoText = "AP: " + String(AP_SSID) + " (" + WiFi.softAPIP().toString() +
                 ")  |  Network: " + ssid + " (" + networkIP.toString() + ") or " +
                 deviceHostname + ".local  |  ";
}

// OLED scroll text for AP-only operation (no station connection).
void setApOnlyInfoText() {
  wifiInfoText = "Connect to WiFi: " + String(AP_SSID) + "  |  Pass: " + String(AP_PASS) +
                 "  |  IP: " + WiFi.softAPIP().toString() + "  |  Captive Portal will auto-open!  |  ";
}

// Force the WiFi info scroll back on, even if the user has already driven the
// robot (recordInput() normally suppresses it permanently). Used after web
// provisioning so the new address is actually visible on the OLED.
void showWifiInfoNow() {
  firstInputReceived = false;
  lastInputTime = millis() - 30000;  // make the idle check pass immediately
  showingWifiInfo = false;           // let updateWifiInfoScroll re-init scroll state
}

// Blocking connect for the BOOT path only (the web server isn't running yet,
// so waiting here is harmless). Keeps the SoftAP alive via WIFI_AP_STA.
// Fast-fails on terminal states (wrong password / SSID not found) and stops
// background retries on failure. The web path uses updateWifiSetup() instead.
bool connectToWifi(const String& ssid, const String& pass, uint32_t timeoutMs) {
  if (ssid.length() == 0) return false;

  Serial.println("Connecting to WiFi network: " + ssid);
  WiFi.mode(WIFI_AP_STA);                 // keep AP up alongside station
  WiFi.setHostname(deviceHostname.c_str());
  WiFi.begin(ssid.c_str(), pass.c_str());

  unsigned long start = millis();
  while ((millis() - start) < timeoutMs) {
    wl_status_t st = WiFi.status();
    if (st == WL_CONNECTED) break;
    // Terminal states — waiting longer won't help.
    if ((millis() - start) > 1000 && (st == WL_CONNECT_FAILED || st == WL_NO_SSID_AVAIL)) break;
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connect failed.");
    WiFi.disconnect();  // stop the driver retrying bad credentials in the background
    return false;
  }

  networkConnected = true;
  networkIP = WiFi.localIP();
  Serial.print("Connected! IP: ");
  Serial.println(networkIP);
  return true;
}

// Finish a web-initiated connect attempt (success or failure).
void finishWifiSetup(const String& err) {
  wifiSetupError = err;
  wifiSetupPass = "";  // don't keep the password in RAM longer than needed
  wifiSetupState = WIFI_SETUP_IDLE;
}

// Drives the non-blocking web connect from loop(). Also acts as a watchdog
// that keeps the cached networkConnected/networkIP honest if the router
// drops (or restores) the station link later.
void updateWifiSetup() {
  if (wifiSetupState == WIFI_SETUP_IDLE) {
    static unsigned long lastCheckMs = 0;
    if (millis() - lastCheckMs >= 5000) {
      lastCheckMs = millis();
      bool live = (WiFi.status() == WL_CONNECTED);
      if (live != networkConnected) {
        networkConnected = live;
        if (live) {
          networkIP = WiFi.localIP();
          announceNetwork(WiFi.SSID());
        } else {
          Serial.println("Station link lost.");
        }
      }
    }
    return;
  }

  if (wifiSetupState == WIFI_SETUP_QUEUED) {
    // Wait for the HTTP response to reach the client: joining a router on
    // another channel drags the SoftAP with it and deauths AP clients.
    if (millis() - wifiSetupQueuedMs < WIFI_SETUP_START_DELAY_MS) return;
    Serial.println("Connecting to WiFi network: " + wifiSetupSsid);
    WiFi.mode(WIFI_AP_STA);
    WiFi.setHostname(deviceHostname.c_str());
    WiFi.begin(wifiSetupSsid.c_str(), wifiSetupPass.c_str());
    wifiSetupStartMs = millis();
    wifiSetupState = WIFI_SETUP_CONNECTING;
    return;
  }

  // WIFI_SETUP_CONNECTING
  wl_status_t st = WiFi.status();
  if (st == WL_CONNECTED) {
    networkConnected = true;
    networkIP = WiFi.localIP();
    Serial.println("Connected! IP: " + networkIP.toString());
    announceNetwork(wifiSetupSsid);
    showWifiInfoNow();
    finishWifiSetup("");
    return;
  }
  // Grace period before trusting terminal states: right after WiFi.begin()
  // the status can still reflect the previous attempt.
  bool terminal = (millis() - wifiSetupStartMs > 1000) &&
                  (st == WL_CONNECT_FAILED || st == WL_NO_SSID_AVAIL);
  if (terminal || (millis() - wifiSetupStartMs) >= WIFI_CONNECT_TIMEOUT_MS) {
    String err = (st == WL_NO_SSID_AVAIL)  ? "Network not found"
               : (st == WL_CONNECT_FAILED) ? "Wrong password or connection rejected"
                                           : "Connection timed out";
    Serial.println("WiFi connect failed: " + err);
    WiFi.disconnect();        // cancel the attempt; stop background retries
    networkConnected = false; // WiFi.begin() already tore down any previous link
    setApOnlyInfoText();      // OLED must not keep advertising a dead network IP
    finishWifiSetup(err);
  }
}

// GET /api/wifi/scan -> {"scanning":true} while the async scan runs, then a
// JSON array of nearby networks (raw; the UI dedups/sorts). Async keeps the
// captive portal responsive — the blocking scan stalls loop() for 2-4s.
void handleWifiScan() {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_RUNNING) {
    server.send(200, "application/json", "{\"scanning\":true}");
    return;
  }
  if (n < 0) {  // no scan results yet -> start one (unless a connect is mid-flight)
    if (wifiSetupState != WIFI_SETUP_IDLE) {
      server.send(200, "application/json", "{\"scanning\":true}");
      return;
    }
    if (WiFi.getMode() == WIFI_AP) {
      WiFi.mode(WIFI_AP_STA);   // station iface must be up to scan
      wifiRestoreApOnly = true; // drop it again once the scan is done
    }
    WiFi.scanNetworks(true /*async*/);
    server.send(200, "application/json", "{\"scanning\":true}");
    return;
  }

  String json = "[";
  json.reserve(n * 64 + 2);
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{\"ssid\":\"" + jsonEscape(WiFi.SSID(i)) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"secure\":" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true") + "}";
  }
  json += "]";
  WiFi.scanDelete();
  // Restore the deliberate AP-only fallback if the scan was the only reason
  // the station interface came up.
  if (wifiRestoreApOnly && wifiSetupState == WIFI_SETUP_IDLE && WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP);
  }
  wifiRestoreApOnly = false;
  server.send(200, "application/json", json);
}

// POST /api/wifi/connect (form: ssid, password) -> {"success":true,"pending":true}.
// The attempt itself runs from loop() (updateWifiSetup) so this handler never
// blocks; the UI polls /api/wifi/status for the outcome.
void handleWifiConnect() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"success\":false,\"error\":\"Method not allowed\"}");
    return;
  }
  String ssid = server.arg("ssid");
  if (ssid.length() == 0) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"SSID required\"}");
    return;
  }
  if (wifiSetupState != WIFI_SETUP_IDLE) {
    server.send(409, "application/json", "{\"success\":false,\"error\":\"Connection attempt already in progress\"}");
    return;
  }

  wifiSetupSsid = ssid;
  wifiSetupPass = server.arg("password");
  wifiSetupError = "";
  wifiSetupQueuedMs = millis();
  wifiSetupState = WIFI_SETUP_QUEUED;
  wifiRestoreApOnly = false;  // an explicit connect supersedes scan cleanup
  server.send(200, "application/json", "{\"success\":true,\"pending\":true}");
}

// GET /api/wifi/status -> station state for the Settings panel, including
// in-progress attempts and the last error so the UI can poll for the result.
void handleWifiStatus() {
  bool connected = (WiFi.status() == WL_CONNECTED);
  String json = "{\"connected\":" + String(connected ? "true" : "false");
  json += ",\"connecting\":" + String(wifiSetupState != WIFI_SETUP_IDLE ? "true" : "false");
  if (wifiSetupError.length() > 0) {
    json += ",\"lastError\":\"" + jsonEscape(wifiSetupError) + "\"";
  }
  if (connected) {
    json += ",\"ssid\":\"" + jsonEscape(WiFi.SSID()) + "\"";
    json += ",\"ip\":\"" + WiFi.localIP().toString() + "\"";
    json += ",\"host\":\"" + deviceHostname + ".local\"";
    json += ",\"mdns\":" + String(mdnsOk ? "true" : "false");
    json += ",\"rssi\":" + String(WiFi.RSSI());
  }
  json += "}";
  server.send(200, "application/json", json);
}

// Unmatched routes: JSON 404 for API paths (a typo'd /api/* URL must not get
// 200 + portal HTML), captive-portal redirect for everything else.
void handleNotFound() {
  if (server.uri().startsWith("/api/")) {
    server.send(404, "application/json", "{\"error\":\"Not found\"}");
    return;
  }
  handleRoot();
}

void setup() {
  Serial.begin(115200);
  randomSeed(micros());
  
  // I2C Init for ESP32
  Wire.begin(I2C_SDA, I2C_SCL);

  // OLED Init
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println(F("SSD1306 allocation failed."));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(F("Setting up WiFi..."));
  display.display();

  // --- WIFI CONFIGURATION ---
  // Don't write credentials to NVS flash: runtime WiFi setup is session-only
  // (see firmware/README.md), and passwords shouldn't persist silently.
  WiFi.persistent(false);

  // Try to connect to network first if configured
  if (ENABLE_NETWORK_MODE && String(NETWORK_SSID).length() > 0) {
    if (!connectToWifi(NETWORK_SSID, NETWORK_PASS)) {
      Serial.println("Failed to connect to network. Running in AP-only mode.");
      WiFi.mode(WIFI_AP); // Fall back to AP-only
    }
  } else {
    WiFi.mode(WIFI_AP);
    Serial.println("Network mode disabled. Running in AP-only mode.");
  }
  
  // --- ACCESS POINT CONFIGURATION ---
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress myIP = WiFi.softAPIP();
  
  Serial.print("AP Created. IP: ");
  Serial.println(myIP);

  // Build WiFi info text for scrolling + start mDNS responder
  if (networkConnected) {
    announceNetwork(NETWORK_SSID);
  } else {
    setApOnlyInfoText();
    startMdns();
  }

  // Initialize input tracking
  lastInputTime = millis();
  firstInputReceived = false;
  showingWifiInfo = false;

  // Start DNS Server for Captive Portal
  // This redirects ALL domain requests to the ESP32's IP
  dnsServer.start(DNS_PORT, "*", myIP);

  // Web Server Routes
  server.on("/", handleRoot);
  server.on("/cmd", handleCommandWeb);
  server.on("/getSettings", handleGetSettings);
  server.on("/setSettings", handleSetSettings);
  
  // API endpoints for network communication
  server.on("/api/status", handleGetStatus);
  server.on("/api/command", handleApiCommand);

  // WiFi provisioning endpoints (runtime network setup from the web UI)
  server.on("/api/wifi/scan", handleWifiScan);
  server.on("/api/wifi/connect", handleWifiConnect);
  server.on("/api/wifi/status", handleWifiStatus);
  
  // Catch-all route for captive portal
  // This ensures any URL redirects to the controller page
  // (except /api/* paths, which get a JSON 404 — see handleNotFound)
  server.onNotFound(handleNotFound);
  
  server.begin();

  // PWM Init
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  for (int i = 0; i < 8; i++) {
    servos[i].setPeriodHertz(50);
    // Map 0-180 to approx 732-2929us
    servos[i].attach(servoPins[i], 732, 2929);
  }
  delay(10);
  
  // Show rest face on startup without moving motors
  setFace("rest");
  
  Serial.println(F("HTTP server & Captive Portal started."));
}

void loop() {
  // Process DNS requests for captive portal
  dnsServer.processNextRequest();
  
  server.handleClient();
  updateWifiSetup();
  updateAnimatedFace();
  updateIdleBlink();
  updateWifiInfoScroll();
  checkMotionWatchdog();

  if (currentCommand != "") {
    String cmd = currentCommand;
    if (cmd == "forward") runWalkPose();
    else if (cmd == "backward") runWalkBackward();
    else if (cmd == "left") runTurnLeft();
    else if (cmd == "right") runTurnRight();
    else if (cmd == "rest") { runRestPose(); if (currentCommand == "rest") currentCommand = ""; }
    else if (cmd == "stand") { runStandPose(1); if (currentCommand == "stand") currentCommand = ""; }
    else if (cmd == "wave") runWavePose();
    else if (cmd == "dance") runDancePose();
    else if (cmd == "swim") runSwimPose();
    else if (cmd == "point") runPointPose();
    else if (cmd == "pushup") runPushupPose();
    else if (cmd == "bow") runBowPose();
    else if (cmd == "cute") runCutePose();
    else if (cmd == "freaky") runFreakyPose();
    else if (cmd == "worm") runWormPose();
    else if (cmd == "shake") runShakePose();
    else if (cmd == "shrug") runShrugPose();
    else if (cmd == "dead") runDeadPose();
    else if (cmd == "crab") runCrabPose();
  }
  
  // Serial CLI for debugging (can be used to diagnose servo position issues and wiring)
  if (Serial.available()) {
    static char command_buffer[32];
    static byte buffer_pos = 0;
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (buffer_pos > 0) {
        command_buffer[buffer_pos] = '\0';
        int motorNum, angle;
        recordInput();
        if(strcmp(command_buffer, "run walk") == 0 || strcmp(command_buffer, "rn wf") == 0) { currentCommand = "forward"; runWalkPose(); currentCommand = ""; }
        else if(strcmp(command_buffer, "rn wb") == 0) { currentCommand = "backward"; runWalkBackward(); currentCommand = ""; }
        else if(strcmp(command_buffer, "rn tl") == 0) { currentCommand = "left"; runTurnLeft(); currentCommand = ""; }
        else if(strcmp(command_buffer, "rn tr") == 0) { currentCommand = "right"; runTurnRight(); currentCommand = ""; }
        else if(strcmp(command_buffer, "run rest") == 0 || strcmp(command_buffer, "rn rs") == 0) runRestPose();
        else if(strcmp(command_buffer, "run stand") == 0 || strcmp(command_buffer, "rn st") == 0) runStandPose(1);
        else if(strcmp(command_buffer, "rn wv") == 0) { currentCommand = "wave"; runWavePose(); }
        else if(strcmp(command_buffer, "rn dn") == 0) { currentCommand = "dance"; runDancePose(); }
        else if(strcmp(command_buffer, "rn sw") == 0) { currentCommand = "swim"; runSwimPose(); }
        else if(strcmp(command_buffer, "rn pt") == 0) { currentCommand = "point"; runPointPose(); }
        else if(strcmp(command_buffer, "rn pu") == 0) { currentCommand = "pushup"; runPushupPose(); }
        else if(strcmp(command_buffer, "rn bw") == 0) { currentCommand = "bow"; runBowPose(); }
        else if(strcmp(command_buffer, "rn ct") == 0) { currentCommand = "cute"; runCutePose(); }
        else if(strcmp(command_buffer, "rn fk") == 0) { currentCommand = "freaky"; runFreakyPose(); }
        else if(strcmp(command_buffer, "rn wm") == 0) { currentCommand = "worm"; runWormPose(); }
        else if(strcmp(command_buffer, "rn sk") == 0) { currentCommand = "shake"; runShakePose(); }
        else if(strcmp(command_buffer, "rn sg") == 0) { currentCommand = "shrug"; runShrugPose(); }
        else if(strcmp(command_buffer, "rn dd") == 0) { currentCommand = "dead"; runDeadPose(); }
        else if(strcmp(command_buffer, "rn cb") == 0) { currentCommand = "crab"; runCrabPose(); }
        else if (strncmp(command_buffer, "face ", 5) == 0 || strncmp(command_buffer, "fc ", 3) == 0) {
          const char* faceName = (command_buffer[1] == 'c') ? command_buffer + 3 : command_buffer + 5;
          if (strlen(faceName) > 0) {
            currentCommand = "";
            setFace(String(faceName));
            Serial.print("Face set to ");
            Serial.println(faceName);
          } else {
            Serial.println("Usage: face <name>");
          }
        }
        else if (strcmp(command_buffer, "subtrim") == 0 || strcmp(command_buffer, "st") == 0) {
          Serial.println("Subtrim values:");
          for (int i = 0; i < 8; i++) {
            Serial.print("Motor "); Serial.print(i); Serial.print(": ");
            if (servoSubtrim[i] >= 0) Serial.print("+");
            Serial.println(servoSubtrim[i]);
          }
        }
        else if (strcmp(command_buffer, "subtrim save") == 0 || strcmp(command_buffer, "st save") == 0) {
          Serial.println("Copy and paste this into your code:");
          Serial.print("int8_t servoSubtrim[8] = {");
          for (int i = 0; i < 8; i++) {
            Serial.print(servoSubtrim[i]);
            if (i < 7) Serial.print(", ");
          }
          Serial.println("};");
        }
        else if (strncmp(command_buffer, "subtrim reset", 13) == 0 || strncmp(command_buffer, "st reset", 8) == 0) {
          for (int i = 0; i < 8; i++) servoSubtrim[i] = 0;
          Serial.println("All subtrim values reset to 0");
        }
        else if (strncmp(command_buffer, "subtrim ", 8) == 0 || strncmp(command_buffer, "st ", 3) == 0) {
          const char* params = (command_buffer[1] == 't') ? command_buffer + 3 : command_buffer + 8;
          int trimMotor, trimValue;
          if (sscanf(params, "%d %d", &trimMotor, &trimValue) == 2) {
            if (trimMotor >= 0 && trimMotor < 8) {
              if (trimValue >= -90 && trimValue <= 90) {
                servoSubtrim[trimMotor] = trimValue;
                Serial.print("Motor "); Serial.print(trimMotor); Serial.print(" subtrim set to ");
                if (trimValue >= 0) Serial.print("+");
                Serial.println(trimValue);
              } else {
                Serial.println("Subtrim value must be between -90 and +90");
              }
            } else {
              Serial.println("Invalid motor number (0-7)");
            }
          }
        }
        else if (strncmp(command_buffer, "all ", 4) == 0) {
             if (sscanf(command_buffer + 4, "%d", &angle) == 1) {
                 for (int i = 0; i < 8; i++) setServoAngle(i, angle);
                 Serial.print("All servos set to "); Serial.println(angle);
             }
        }
        else if (sscanf(command_buffer, "%d %d", &motorNum, &angle) == 2) {
             if (motorNum >= 0 && motorNum < 8) {
                 setServoAngle(motorNum, angle);
                 Serial.print("Servo "); Serial.print(motorNum); Serial.print(" set to "); Serial.println(angle);
             } else {
                 Serial.println("Invalid motor number (0-7)");
             }
        }
        buffer_pos = 0;
      }
    } else if (buffer_pos < sizeof(command_buffer) - 1) {
      command_buffer[buffer_pos++] = c;
    }
  }
}

// Function to update the robot's face
void updateFaceBitmap(const unsigned char* bitmap) {
  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, 128, 64, SSD1306_WHITE);
  display.display();
}

uint8_t countFrames(const unsigned char* const* frames, uint8_t maxFrames) {
  if (frames == nullptr || frames[0] == nullptr) return 0;
  uint8_t count = 0;
  for (uint8_t i = 0; i < maxFrames; i++) {
    if (frames[i] == nullptr) break;
    count++;
  }
  return count;
}

void setFace(const String& faceName) {
  if (faceName == currentFaceName && currentFaceFrames != nullptr) return;

  currentFaceName = faceName;
  currentFaceFrameIndex = 0;
  lastFaceFrameMs = 0;
  faceFrameDirection = 1;
  faceAnimFinished = false;
  currentFaceFps = getFaceFpsForName(faceName);

  currentFaceFrames = face_defualt_frames;
  currentFaceFrameCount = countFrames(face_defualt_frames, MAX_FACE_FRAMES);

  for (size_t i = 0; i < (sizeof(faceEntries) / sizeof(faceEntries[0])); i++) {
    if (faceName.equalsIgnoreCase(faceEntries[i].name)) {
      currentFaceFrames = faceEntries[i].frames;
      currentFaceFrameCount = countFrames(faceEntries[i].frames, faceEntries[i].maxFrames);
      break;
    }
  }

  if (currentFaceFrameCount == 0) {
    currentFaceFrames = face_defualt_frames;
    currentFaceFrameCount = countFrames(face_defualt_frames, MAX_FACE_FRAMES);
    currentFaceName = "default";
    currentFaceFps = getFaceFpsForName(currentFaceName);
  }

  if (currentFaceFrameCount > 0 && currentFaceFrames[0] != nullptr) {
    updateFaceBitmap(currentFaceFrames[0]);
  }
}

void setFaceMode(FaceAnimMode mode) {
  currentFaceMode = mode;
  faceFrameDirection = 1;
  faceAnimFinished = false;
}

void setFaceWithMode(const String& faceName, FaceAnimMode mode) {
  setFaceMode(mode);
  setFace(faceName);
}

int getFaceFpsForName(const String& faceName) {
  for (size_t i = 0; i < (sizeof(faceFpsEntries) / sizeof(faceFpsEntries[0])); i++) {
    if (faceName.equalsIgnoreCase(faceFpsEntries[i].name)) {
      return faceFpsEntries[i].fps;
    }
  }
  return faceFps;
}

void updateAnimatedFace() {
  if (currentFaceFrames == nullptr || currentFaceFrameCount <= 1) return;
  if (currentFaceMode == FACE_ANIM_ONCE && faceAnimFinished) return;

  unsigned long now = millis();
  int fps = max(1, (currentFaceFps > 0 ? currentFaceFps : faceFps));
  unsigned long interval = 1000UL / fps;
  if (now - lastFaceFrameMs >= interval) {
    lastFaceFrameMs = now;
    if (currentFaceMode == FACE_ANIM_LOOP) {
      currentFaceFrameIndex = (currentFaceFrameIndex + 1) % currentFaceFrameCount;
    } else if (currentFaceMode == FACE_ANIM_ONCE) {
      if (currentFaceFrameIndex + 1 >= currentFaceFrameCount) {
        currentFaceFrameIndex = currentFaceFrameCount - 1;
        faceAnimFinished = true;
      } else {
        currentFaceFrameIndex++;
      }
    } else {
      if (faceFrameDirection > 0) {
        if (currentFaceFrameIndex + 1 >= currentFaceFrameCount) {
          faceFrameDirection = -1;
          if (currentFaceFrameIndex > 0) currentFaceFrameIndex--;
        } else {
          currentFaceFrameIndex++;
        }
      } else {
        if (currentFaceFrameIndex == 0) {
          faceFrameDirection = 1;
          if (currentFaceFrameCount > 1) currentFaceFrameIndex++;
        } else {
          currentFaceFrameIndex--;
        }
      }
    }
    updateFaceBitmap(currentFaceFrames[currentFaceFrameIndex]);
  }
}

void delayWithFace(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    updateAnimatedFace();
    server.handleClient();
    dnsServer.processNextRequest();
    delay(5);
  }
}

void scheduleNextIdleBlink(unsigned long minMs, unsigned long maxMs) {
  unsigned long now = millis();
  unsigned long interval = (unsigned long)random(minMs, maxMs);
  nextIdleBlinkMs = now + interval;
}

void enterIdle() {
  idleActive = true;
  idleBlinkActive = false;
  idleBlinkRepeatsLeft = 0;
  setFaceWithMode("idle", FACE_ANIM_BOOMERANG);
  scheduleNextIdleBlink(3000, 7000);
}

void exitIdle() {
  idleActive = false;
  idleBlinkActive = false;
}

void updateIdleBlink() {
  if (!idleActive) return;

  if (!idleBlinkActive) {
    if (millis() >= nextIdleBlinkMs) {
      idleBlinkActive = true;
      if (idleBlinkRepeatsLeft == 0 && random(0, 100) < 30) {
        idleBlinkRepeatsLeft = 1; // double blink
      }
      setFaceWithMode("idle_blink", FACE_ANIM_ONCE);
    }
    return;
  }

  if (currentFaceMode == FACE_ANIM_ONCE && faceAnimFinished) {
    idleBlinkActive = false;
    setFaceWithMode("idle", FACE_ANIM_BOOMERANG);
    if (idleBlinkRepeatsLeft > 0) {
      idleBlinkRepeatsLeft--;
      scheduleNextIdleBlink(120, 220);
    } else {
      scheduleNextIdleBlink(3000, 7000);
    }
  }
}

// ====== HELPERS ======
void setServoAngle(uint8_t channel, int angle) { 
  if (channel < 8) {
    int adjustedAngle = constrain(angle + servoSubtrim[channel], 0, 180);
    servos[channel].write(adjustedAngle);
    delayWithFace(motorCurrentDelay);
  }
}

bool pressingCheck(String cmd, int ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    server.handleClient();
    dnsServer.processNextRequest();
    updateAnimatedFace();
    checkMotionWatchdog();
    if (currentCommand != cmd) {
      runStandPose(1);
      return false;
    }
    yield();
  }
  return true;
}

// Stop a continuous move whose controller has gone quiet (motion-watchdog.h).
// Clearing currentCommand is what "stop" does: pressingCheck() stands up.
void checkMotionWatchdog() {
  if (motionWatchdog.shouldStop(millis(), isContinuousMove(currentCommand.c_str()))) {
    Serial.println(F("Motion timeout: no command received, stopping"));
    currentCommand = "";
  }
}

void recordInput() {
  lastInputTime = millis();
  if (!firstInputReceived) {
    firstInputReceived = true;
    showingWifiInfo = false;
  }
}

void updateWifiInfoScroll() {
  // Don't show WiFi info if first input has been received
  if (firstInputReceived) {
    if (showingWifiInfo) {
      showingWifiInfo = false;
      // Restore the current face
      if (currentFaceFrames != nullptr && currentFaceFrameCount > 0) {
        updateFaceBitmap(currentFaceFrames[currentFaceFrameIndex]);
      }
    }
    return;
  }
  
  unsigned long now = millis();
  
  // Check if 30 seconds have passed without input
  if (!showingWifiInfo && (now - lastInputTime >= 30000)) {
    showingWifiInfo = true;
    wifiScrollPos = 0;
    lastWifiScrollMs = now;
  }
  
  if (!showingWifiInfo) return;
  
  // Update scroll every 150ms
  if (now - lastWifiScrollMs >= 150) {
    lastWifiScrollMs = now;
    
    // Clear and redraw with current face in background
    display.clearDisplay();
    
    // Draw the face bitmap in the background
    if (currentFaceFrames != nullptr && currentFaceFrameCount > 0) {
      display.drawBitmap(0, 0, currentFaceFrames[currentFaceFrameIndex], 128, 64, SSD1306_WHITE);
    }
    
    // Draw black bar for text background on top row
    display.fillRect(0, 0, 128, 10, SSD1306_BLACK);
    
    // Draw scrolling text
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    display.setCursor(-wifiScrollPos, 1);
    display.print(wifiInfoText);
    display.setTextWrap(true);
    
    display.display();
    
    // Advance scroll position
    wifiScrollPos += 2;
    if (wifiScrollPos >= (int)(wifiInfoText.length() * 6)) {
      wifiScrollPos = 0;
    }
  }
}
