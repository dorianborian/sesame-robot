#pragma once
#include <Arduino.h>
#include "../../movement-sequences.h"

void initFaceSystem();
void updateFaceAnimation();
void updateIdleBlink();
void updateWifiInfoScroll();
void setFace(const String& name);
void setFaceMode(FaceAnimMode mode);
void setFaceWithMode(const String& name, FaceAnimMode mode);
void enterIdle();
void exitIdle();
void delayWithFace(unsigned long ms);
bool pressingCheck(String cmd, int ms);
void recordInput();

extern int faceFps;
extern String wifiInfoText;
