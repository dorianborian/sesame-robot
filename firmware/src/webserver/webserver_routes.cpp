#include "webserver_routes.h"
#include "../../captive-portal.h"
#include "../../movement-sequences.h"
#include "../network/network.h"
#include "../gpio/gpio_servos.h"
#include "../face/face_anim.h"
#include "../globals.h"

WebServer server(80);

extern int frameDelay;
extern int walkCycles;

static void applyMotorCommand(const String& motorArg, int angle) {
  int motorNum = motorArg.toInt();
  int servoIdx = servoNameToIndex(motorArg);

  if (motorNum >= 1 && motorNum <= 8 && angle >= 0 && angle <= 180) {
    setServoAngle(motorNum - 1, angle);
  } else if (servoIdx != -1 && angle >= 0 && angle <= 180) {
    setServoAngle(servoIdx, angle);
  } else {
    server.send(400, "text/plain", "Invalid motor or angle");
    return;
  }
  recordInput();
  server.send(200, "text/plain", "OK");
}

static String parseJsonString(const String& body, const String& key) {
  int keyIdx = body.indexOf("\"" + key + "\":\"");
  if (keyIdx == -1) keyIdx = body.indexOf("\"" + key + "\": \"");
  if (keyIdx == -1) return "";

  int valStart = body.indexOf("\"", keyIdx + key.length() + 3) + 1;
  int valEnd   = body.indexOf("\"", valStart);
  if (valEnd <= valStart) return "";
  return body.substring(valStart, valEnd);
}

static bool hasJsonKey(const String& body, const String& key) {
  return body.indexOf("\"" + key + "\":") != -1 ||
         body.indexOf("\"" + key + "\": ") != -1;
}

static void handleRoot() {
  server.send(200, "text/html", index_html);
}

static void handleCommandWeb() {
  if (server.hasArg("pose") || server.hasArg("go")) {
    currentCommand = server.hasArg("pose") ? server.arg("pose") : server.arg("go");
    recordInput();
    exitIdle();
    server.send(200, "text/plain", "OK");
  } else if (server.hasArg("stop")) {
    currentCommand = "";
    recordInput();
    server.send(200, "text/plain", "OK");
  } else if (server.hasArg("motor") && server.hasArg("value")) {
    applyMotorCommand(server.arg("motor"), server.arg("value").toInt());
  } else {
    server.send(400, "text/plain", "Bad Args");
  }
}

static void handleGetSettings() {
  String json = "{";
  json += "\"frameDelay\":"       + String(frameDelay)       + ",";
  json += "\"walkCycles\":"       + String(walkCycles)       + ",";
  json += "\"motorCurrentDelay\":" + String(motorCurrentDelay) + ",";
  json += "\"faceFps\":"          + String(faceFps);
  json += "}";
  server.send(200, "application/json", json);
}

static void handleSetSettings() {
  if (server.hasArg("frameDelay"))       frameDelay        = server.arg("frameDelay").toInt();
  if (server.hasArg("walkCycles"))       walkCycles        = server.arg("walkCycles").toInt();
  if (server.hasArg("motorCurrentDelay")) motorCurrentDelay = server.arg("motorCurrentDelay").toInt();
  if (server.hasArg("faceFps"))          faceFps           = max(1, (int)server.arg("faceFps").toInt());
  server.send(200, "text/plain", "OK");
}

static void handleGetStatus() {
  extern String currentFaceName;
  String json = "{";
  json += "\"currentCommand\":\"" + currentCommand  + "\",";
  json += "\"currentFace\":\""    + currentFaceName + "\",";
  json += "\"networkConnected\":" + String(networkConnected ? "true" : "false") + ",";
  json += "\"apIP\":\""           + WiFi.softAPIP().toString() + "\"";
  if (networkConnected) {
    json += ",\"networkIP\":\"" + networkIP.toString() + "\"";
  }
  json += "}";
  server.send(200, "application/json", json);
}

static void handleApiCommand() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }

  String body    = server.arg("plain");
  String face    = parseJsonString(body, "face");
  bool faceOnly  = (face.length() > 0 && !hasJsonKey(body, "command"));
  String command = faceOnly ? "" : parseJsonString(body, "command");

  Serial.println("API: " + body);

  if (face.length() > 0) setFace(face);

  if (faceOnly) {
    recordInput();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Face updated\"}");
    return;
  }

  if (command.length() == 0) {
    server.send(400, "application/json", "{\"error\":\"Missing command field\"}");
    return;
  }

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

// ── Public API ───────────────────────────────────────────────────────────────

void initWebServer() {
  server.on("/",            handleRoot);
  server.on("/cmd",         handleCommandWeb);
  server.on("/getSettings", handleGetSettings);
  server.on("/setSettings", handleSetSettings);
  server.on("/api/status",  handleGetStatus);
  server.on("/api/command", handleApiCommand);
  server.onNotFound(handleRoot);   // catch-all for captive portal
  server.begin();
}

void processWebServer() {
  server.handleClient();
}
