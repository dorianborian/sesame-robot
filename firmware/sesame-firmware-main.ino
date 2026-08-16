#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ElegantOTA.h>
#include <Preferences.h>
#include "buzzer.h"
#include "face-bitmaps.h"
#include "movement-sequences.h"
#include "captive-portal.h"

#define FIRMWARE_VERSION "1.1.0"

Preferences preferences;
int lastWifiStatus = -1;
String savedSSID = "";
String savedPassword = "";

void onOTAStart() {
  Serial.println("OTA Update Started");
  setFaceWithMode("surprised", FACE_ANIM_ONCE);
}

void onOTAEnd(bool success) {
  Serial.println(success ? "OTA Update Successful" : "OTA Update Failed");
  if (success) {
    setFaceWithMode("happy", FACE_ANIM_ONCE);
    delay(2000);
  }
  setFace("default");
}

// --- Access Point Configuration ---
// This is the network the Robot will create
const String DEFAULT_AP_SSID = "Sesame-Controller-BETA";
const String DEFAULT_AP_PASS = "12345678"; // Must be at least 8 characters
String currentAPSSID = DEFAULT_AP_SSID;
String currentAPPass = DEFAULT_AP_PASS;

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

// I2C Pins for Distro Board V2
//#define I2C_SDA 8
//#define I2C_SCL 9

// I2C Pins for Distro Board
//#define I2C_SDA 21
//#define I2C_SCL 22

// I2C Pins for S2 Mini Board
#define I2C_SDA 33
#define I2C_SCL 35

// Touch Sensor Pin
#define TOP_TOUCH_PIN   12
// Sensors were physically installed on opposite sides; keep logical LEFT/RIGHT
// behavior correct by mapping each label to its actual GPIO.
#define LEFT_TOUCH_PIN  11
#define RIGHT_TOUCH_PIN 18

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
const String DEFAULT_HOSTNAME = "sesame-robot";
String deviceHostname = DEFAULT_HOSTNAME;

// Touch Sensor
bool lastTouchState = false;
bool touchWiggleActive = false;
int8_t wiggleRunoutCount = 0;

bool lastLeftTouchState = false;
bool lastRightTouchState = false;

enum SideTouchReaction : uint8_t {
  SIDE_TOUCH_IDLE,
  SIDE_TOUCH_PENDING_TURN,
  SIDE_TOUCH_SUPERMAN
};

SideTouchReaction sideTouchReaction = SIDE_TOUCH_IDLE;
bool pendingTurnIsLeft = false;
unsigned long pendingTurnUntilMs = 0;
unsigned long lastSupermanShakeMs = 0;
bool supermanShakeAlternate = false;


// Servo Pins for Distro Board
// ======================================================================
// Pin numbers are coorisponding to the ESP32 GPIO pins and may differ based on which board you use.
// If you are using a different board, please adjust the servoPins array accordingly.
// ======================================================================
Servo servos[8];
// Sesame Distro Board V2 Pinout
//const int servoPins[8] = {4, 5, 6, 7, 15, 16, 17, 18};

// Sesame Distro Board Pinout
//const int servoPins[8] = {15, 2, 23, 19, 4, 16, 17, 18};

// Lolin S2 Mini Pinout
const int servoPins[8] = {1, 2, 4, 6, 8, 10, 13, 14};

// Subtrim values for each servo (offset in degrees)
int8_t servoSubtrim[8] = {0, +15, 0, 0, -15, +15, -15, 15};


// Animation constants
int frameDelay = 100;
int walkCycles = 10;
int motorCurrentDelay = 20; // ms delay between motor movements to prevent over-current

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
void handleGetSettings();
void handleSetSettings();
void handleGetStatus();
void handleGetServoPositions();
void handleApiCommand();
void updateWifiInfoScroll();
void updateWifiInfoDisplay();
void recordInput();

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
  json += "}";
  server.send(200, "application/json", json);
}

void handleSetSettings() {
  if (server.hasArg("frameDelay")) frameDelay = server.arg("frameDelay").toInt();
  if (server.hasArg("walkCycles")) walkCycles = server.arg("walkCycles").toInt();
  if (server.hasArg("motorCurrentDelay")) motorCurrentDelay = server.arg("motorCurrentDelay").toInt();
  if (server.hasArg("faceFps")) faceFps = (int)max(1L, server.arg("faceFps").toInt());
  server.send(200, "text/plain", "OK");
}

// API endpoint for network clients to get robot status
void handleGetStatus() {
  String json = "{";
  json += "\"firmware\":\"" FIRMWARE_VERSION "\",";
  json += "\"currentCommand\":\"" + currentCommand + "\",";
  json += "\"currentFace\":\"" + currentFaceName + "\",";
  json += "\"networkConnected\":" + String(networkConnected ? "true" : "false") + ",";
  json += "\"apIP\":\"" + WiFi.softAPIP().toString() + "\",";
  json += "\"apSSID\":\"" + currentAPSSID + "\",";
  json += "\"hostname\":\"" + deviceHostname + "\"";
  
  if (networkConnected) {
    json += ",\"networkIP\":\"" + networkIP.toString() + "\",";
    json += "\"ssid\":\"" + WiFi.SSID() + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI());
  }
  
  json += "}";
  server.send(200, "application/json", json);
}

void handleGetServoPositions() {
  String json = "{";
  for (int i = 0; i < 8; i++) {
    int angle = servos[i].attached() ? servos[i].read() : 90;
    json += "\"s" + String(i) + "\":" + String(angle);
    if (i < 7) json += ",";
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
    recordInput();
    exitIdle();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Command executed\"}");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Sesame Robot Firmware v" FIRMWARE_VERSION " ===");
  randomSeed(micros());
  
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println(F("SSD1306 allocation failed."));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(F("Initializing..."));
  display.display();

  WiFi.mode(WIFI_AP_STA);
  
  // Load settings from Preferences BEFORE creating AP
  preferences.begin("sesame-wifi", true);
  savedSSID = preferences.getString("ssid", "");
  savedPassword = preferences.getString("pass", "");
  deviceHostname = preferences.getString("hostname", DEFAULT_HOSTNAME);
  String savedAPSSID = preferences.getString("apssid", "");
  String savedAPPass = preferences.getString("appass", "");
  if (savedAPSSID.length() > 0) {
    currentAPSSID = savedAPSSID;
  }
  if (savedAPPass.length() >= 8) {
    currentAPPass = savedAPPass;
  }
  preferences.end();

  Serial.println("[HOSTNAME] Device name: " + deviceHostname);
  Serial.println("[HOSTNAME] AP SSID: " + currentAPSSID);
  Serial.println("[HOSTNAME] AP Password: " + currentAPPass);

  // Create AP with the correct SSID
  WiFi.softAP(currentAPSSID.c_str(), currentAPPass.c_str());

  if (savedSSID.length() > 0) {
    Serial.println("[WIFI] Attempting saved connection: " + savedSSID);
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
      delay(500);
      Serial.print(".");
      timeout++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      networkConnected = true;
      networkIP = WiFi.localIP();
      Serial.println("\n[WIFI] Connected! IP: " + networkIP.toString());
      
      if (MDNS.begin(deviceHostname.c_str())) {
        Serial.println("[mDNS] Started: http://" + deviceHostname + ".local");
        MDNS.addService("http", "tcp", 80);
      }
    } else {
      Serial.println("\n[WIFI] Failed to connect - running AP only");
    }
  } else {
    Serial.println("[WIFI] No saved credentials - running AP only");
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("[AP] Created. IP: ");
  Serial.println(myIP);

  updateWifiInfoText();

  lastInputTime = millis();
  firstInputReceived = false;
  showingWifiInfo = true;  // Show WiFi info immediately on startup
  wifiScrollPos = 0;
  lastWifiScrollMs = millis();

  if (!networkConnected) {
    if (MDNS.begin(deviceHostname.c_str())) {
      Serial.println("[mDNS] Started: http://" + deviceHostname + ".local");
      MDNS.addService("http", "tcp", 80);
    }
  }

  dnsServer.start(DNS_PORT, "*", myIP);

  server.on("/", handleRoot);
  server.on("/cmd", handleCommandWeb);
  server.on("/getSettings", handleGetSettings);
  server.on("/setSettings", handleSetSettings);
  server.on("/api/status", handleGetStatus);
  server.on("/api/servoPositions", handleGetServoPositions);
  server.on("/api/command", handleApiCommand);
  
  server.on("/scan", handleWiFiScan);
  server.on("/wificonnect", handleWiFiConnect);
  server.on("/resetwifi", handleWiFiReset);
  server.on("/setHostname", handleSetHostname);
  server.on("/setApPassword", handleSetApPassword);
  
  server.onNotFound(handleRoot);
  
  ElegantOTA.begin(&server);
  server.begin();

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  for (int i = 0; i < 8; i++) {
    servos[i].setPeriodHertz(50);
    servos[i].attach(servoPins[i], 732, 2929);
  }
  delay(10);
  setupBuzzer();
  
  pinMode(TOP_TOUCH_PIN, INPUT);
  pinMode(LEFT_TOUCH_PIN, INPUT);
  pinMode(RIGHT_TOUCH_PIN, INPUT);

  
  setFace("rest");
  
  Serial.println(F("=== System Ready ==="));
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  ElegantOTA.loop();
  checkWiFiStatus();
  updateAnimatedFace();
  updateIdleBlink();
  updateWifiInfoScroll();
  updateBuzzer();
  
  // Touch sensor handling - wiggle animation
  bool touchState = digitalRead(TOP_TOUCH_PIN);
  
  if (touchState && !lastTouchState) {
    if (currentCommand == "wiggle" && wiggleRunoutCount > 0) {
      wiggleRunoutCount = 0;
      touchWiggleActive = true;
      exitIdle();
    }
    else if (!touchWiggleActive && currentCommand == "") {
      wiggleRunoutCount = 0;
      touchWiggleActive = true;
      currentCommand = "wiggle";
      recordInput();
      exitIdle();
      setFaceWithMode("cute", FACE_ANIM_LOOP);
      playBuzzerCue(BUZZER_CUE_PET);
      runStandPose(0);
      delayWithFace(200);
    }
  }
  
  if (!touchState && lastTouchState) {
    wiggleRunoutCount = 4;
    touchWiggleActive = false;
  }
  
  lastTouchState = touchState;

  bool leftTouch = digitalRead(LEFT_TOUCH_PIN);
  bool rightTouch = digitalRead(RIGHT_TOUCH_PIN);
  bool bothTouch = leftTouch && rightTouch;
  unsigned long now = millis();

  // BOTH SIDES is level-priority: it also promotes an already-started single
  // nudge when the second sensor is pressed a moment later.
  if (bothTouch && currentCommand == "" &&
      sideTouchReaction != SIDE_TOUCH_SUPERMAN) {
    Serial.println(F("Both sides touched - Superman!"));
    exitIdle();
    setFaceWithMode("happy", FACE_ANIM_ONCE);
    playBuzzerCue(BUZZER_CUE_SUPERMAN);
    runSupermanPose();
    sideTouchReaction = SIDE_TOUCH_SUPERMAN;
    lastSupermanShakeMs = now;
    supermanShakeAlternate = false;
  } else if (sideTouchReaction == SIDE_TOUCH_SUPERMAN) {
    if (!bothTouch) {
      runStandPose(1);
      sideTouchReaction = SIDE_TOUCH_IDLE;
    } else if (now - lastSupermanShakeMs >= 280) {
      supermanShakeAlternate = !supermanShakeAlternate;
      runSupermanShake(supermanShakeAlternate);
      lastSupermanShakeMs = now;
    }
  } else if (sideTouchReaction == SIDE_TOUCH_PENDING_TURN) {
    // Give the second side sensor time to arrive before committing to a turn.
    if (currentCommand != "" || (!leftTouch && !rightTouch)) {
      sideTouchReaction = SIDE_TOUCH_IDLE;
    } else if (now >= pendingTurnUntilMs) {
      if (pendingTurnIsLeft && leftTouch && !rightTouch) {
        Serial.println(F("Left touch - four-cycle turn"));
        exitIdle();
        playBuzzerCue(BUZZER_CUE_TURN_LEFT);
        runTouchTurnLeft();
      } else if (!pendingTurnIsLeft && rightTouch && !leftTouch) {
        Serial.println(F("Right touch - four-cycle turn"));
        exitIdle();
        playBuzzerCue(BUZZER_CUE_TURN_RIGHT);
        runTouchTurnRight();
      }
      sideTouchReaction = SIDE_TOUCH_IDLE;
    }
  } else if (currentCommand == "") {
    if (leftTouch && !rightTouch && !lastLeftTouchState) {
      pendingTurnIsLeft = true;
      pendingTurnUntilMs = now + 200;
      sideTouchReaction = SIDE_TOUCH_PENDING_TURN;
    } else if (rightTouch && !leftTouch && !lastRightTouchState) {
      pendingTurnIsLeft = false;
      pendingTurnUntilMs = now + 200;
      sideTouchReaction = SIDE_TOUCH_PENDING_TURN;
    }
  }

  // Edge tracking prevents re-triggering while a sensor remains touched.
  lastLeftTouchState = leftTouch;
  lastRightTouchState = rightTouch;


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
    else if (cmd == "pissleft") runPissLeftPose();
    else if (cmd == "pissright") runPissRightPose();
    else if (cmd == "wiggle") {
      runWigglePose();
      if (touchWiggleActive || wiggleRunoutCount > 0) {
        delayWithFace(60 + random(-40, 41));
      }
    }
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
        else if(strcmp(command_buffer, "rn pl") == 0) { currentCommand = "pissleft"; runPissLeftPose(); }
        else if(strcmp(command_buffer, "rn pr") == 0) { currentCommand = "pissright"; runPissRightPose(); }
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
    updateBuzzer();
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
    if (currentCommand != cmd) {
      runStandPose(1);
      return false;
    }
    yield();
  }
  return true;
}

void recordInput() {
  lastInputTime = millis();
  if (!firstInputReceived) {
    firstInputReceived = true;
    showingWifiInfo = false;
    // Restore the face when stopping WiFi info display
    if (currentFaceFrames != nullptr && currentFaceFrameCount > 0) {
      updateFaceBitmap(currentFaceFrames[currentFaceFrameIndex]);
    }
  }
}

void updateWifiInfoScroll() {
  unsigned long now = millis();
  
  // If not showing WiFi info, check if we should start (after 30s idle)
  if (!showingWifiInfo && (now - lastInputTime >= 30000)) {
    showingWifiInfo = true;
    wifiScrollPos = 0;
    lastWifiScrollMs = now;
  }
  
  // If WiFi info is active, check if we should stop (after 30s of activity)
  if (showingWifiInfo && firstInputReceived && (now - lastInputTime < 30000)) {
    showingWifiInfo = false;
    // Restore the face
    if (currentFaceFrames != nullptr && currentFaceFrameCount > 0) {
      updateFaceBitmap(currentFaceFrames[currentFaceFrameIndex]);
    }
    return;
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

void checkWiFiStatus() {
  int currentStatus = WiFi.status();
  
  // Check if we just connected to WiFi
  if (currentStatus == WL_CONNECTED && !networkConnected) {
    networkConnected = true;
    networkIP = WiFi.localIP();
    Serial.println("[WIFI] Status: CONNECTED | IP: " + networkIP.toString());
    updateWifiInfoText();
    updateWifiInfoDisplay(); // Force refresh display
    
    if (!MDNS.begin(deviceHostname.c_str())) {
      Serial.println("[mDNS] Error starting");
    } else {
      MDNS.addService("http", "tcp", 80);
    }
  }
  // Check if we got disconnected
  else if (currentStatus != WL_CONNECTED && networkConnected) {
    Serial.println("[WIFI] Status: DISCONNECTED");
    networkConnected = false;
    updateWifiInfoText();
    updateWifiInfoDisplay(); // Force refresh display
  }
  
  lastWifiStatus = currentStatus;
}

// Force immediate display update
void updateWifiInfoDisplay() {
  if (showingWifiInfo) {
    wifiScrollPos = 0;
    lastWifiScrollMs = millis();
  }
}

void updateWifiInfoText() {
  IPAddress apIP = WiFi.softAPIP();
  if (networkConnected) {
    wifiInfoText = "AP: " + currentAPSSID + " (" + apIP.toString() + ")  |  Network: " + 
                   WiFi.SSID() + " (" + networkIP.toString() + ") or " + deviceHostname + ".local  |  ";
  } else {
    wifiInfoText = "WiFi: " + currentAPSSID + " | Pass: " + currentAPPass + " | IP: " + 
                   apIP.toString() + " | http://" + deviceHostname + ".local  |  ";
  }
}

void handleWiFiScan() {
  Serial.println("[WIFI] Scanning networks...");
  int n = WiFi.scanNetworks(false, true);
  String json = "[";
  for (int i = 0; i < n; i++) {
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
    if (i < n - 1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
  WiFi.scanDelete();
  Serial.print("[WIFI] Scan complete. Found: ");
  Serial.println(n);
}

void handleWiFiConnect() {
  if (!server.hasArg("ssid") || !server.hasArg("pass")) {
    server.send(400, "text/plain", "Missing ssid or pass");
    return;
  }
  
  String newSSID = server.arg("ssid");
  String newPass = server.arg("pass");
  
  Serial.println("[WIFI] Saving new credentials: " + newSSID);
  
  preferences.begin("sesame-wifi", false);
  preferences.putString("ssid", newSSID);
  preferences.putString("pass", newPass);
  preferences.end();
  
  server.send(200, "text/plain", "Rebooting to connect...");
  delay(500);
  ESP.restart();
}

void handleWiFiReset() {
  Serial.println("[WIFI] Resetting saved credentials");
  preferences.begin("sesame-wifi", false);
  preferences.clear();
  preferences.end();
  
  server.send(200, "text/plain", "WiFi credentials cleared. Rebooting...");
  delay(500);
  ESP.restart();
}

void handleSetHostname() {
  if (!server.hasArg("hostname")) {
    server.send(400, "text/plain", "Missing hostname parameter");
    return;
  }
  
  String newHostname = server.arg("hostname");
  
  // Validate hostname (lowercase, no spaces)
  newHostname.trim();
  newHostname.toLowerCase();
  
  // Remove invalid characters
  for (int i = newHostname.length() - 1; i >= 0; i--) {
    char c = newHostname.charAt(i);
    if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-')) {
      newHostname.remove(i, 1);
    }
  }
  
  // Remove hyphens at start/end
  while (newHostname.startsWith("-")) newHostname.remove(0, 1);
  while (newHostname.endsWith("-")) newHostname.remove(newHostname.length() - 1, 1);
  
  if (newHostname.length() == 0) {
    newHostname = DEFAULT_HOSTNAME;
  }
  
  // Create friendly display name from hostname (sesame-green -> Sesame Green)
  String friendlyName = newHostname;
  friendlyName.replace("-", " ");
  // Capitalize first letter of each word
  if (friendlyName.length() > 0) {
    friendlyName[0] = toupper(friendlyName[0]);
    for (int i = 1; i < friendlyName.length(); i++) {
      if (friendlyName[i-1] == ' ') {
        friendlyName[i] = toupper(friendlyName[i]);
      }
    }
  }
  
  // Create AP SSID from friendly name
  String newAPSSID = friendlyName + " AP";
  
  Serial.println("[HOSTNAME] Saving: " + newHostname);
  Serial.println("[HOSTNAME] AP SSID: " + newAPSSID);
  
  preferences.begin("sesame-wifi", false);
  preferences.putString("hostname", newHostname);
  preferences.putString("apssid", newAPSSID);
  preferences.end();
  
  server.send(200, "text/plain", "Saved: " + friendlyName + ". Rebooting...");
  delay(500);
  ESP.restart();
}

void handleSetApPassword() {
  if (!server.hasArg("password")) {
    server.send(400, "text/plain", "Missing password parameter");
    return;
  }
  
  String newPassword = server.arg("password");
  
  if (newPassword.length() < 8) {
    server.send(400, "text/plain", "Password must be at least 8 characters");
    return;
  }
  
  Serial.println("[AP] Saving new password");
  
  preferences.begin("sesame-wifi", false);
  preferences.putString("appass", newPassword);
  preferences.end();
  
  server.send(200, "text/plain", "AP password saved. Rebooting...");
  delay(500);
  ESP.restart();
}
