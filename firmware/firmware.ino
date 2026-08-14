#include "src/i2c/i2c_bus.h"
#include "src/screen/screen.h"
#include "src/gpio/gpio_servos.h"
#include "src/network/network.h"
#include "src/webserver/webserver_routes.h"
#include "src/face/face_anim.h"
#include "src/input/command_router.h"
#include "src/input/serial_cli.h"
#include "src/globals.h"

String currentCommand = "";

int frameDelay  = 100;
int walkCycles  = 10;

void setup() {
  Serial.begin(115200);
  randomSeed(micros());

  initI2C();
  initScreen();
  initWiFi();
  initWebServer();
  initServos();
  initFaceSystem();

  setFace("rest");

  Serial.println(F("Kitto-bot ready."));
}

void loop() {
  processDNS();
  processWebServer();

  updateFaceAnimation();
  updateIdleBlink();
  updateWifiInfoScroll();

  routeCurrentCommand();
  processSerialCLI();
}
