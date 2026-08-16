#pragma once

#include <Arduino.h>
#include "buzzer.h"

extern bool touchWiggleActive;
extern int8_t wiggleRunoutCount;

enum ServoName : uint8_t {
  R1 = 0,
  R2 = 1,
  L1 = 2,
  L2 = 3,
  R4 = 4,
  R3 = 5,
  L3 = 6,
  L4 = 7
};

const String ServoNames[] = { "R1", "R2", "L1", "L2", "R4", "R3", "L3", "L4" };

inline int servoNameToIndex(const String& servo) {
  if (servo == "L1") return L1;
  if (servo == "L2") return L2;
  if (servo == "L3") return L3;
  if (servo == "L4") return L4;
  if (servo == "R1") return R1;
  if (servo == "R2") return R2;
  if (servo == "R3") return R3;
  if (servo == "R4") return R4;
  return -1;
}

enum FaceAnimMode : uint8_t {
  FACE_ANIM_LOOP = 0,
  FACE_ANIM_ONCE = 1,
  FACE_ANIM_BOOMERANG = 2
};

// External globals and helpers used by movement/pose sequences
extern int frameDelay;
extern int walkCycles;
extern String currentCommand;

extern void setServoAngle(uint8_t channel, int angle);
extern void setFace(const String& faceName);
extern void setFaceMode(FaceAnimMode mode);
extern void setFaceWithMode(const String& faceName, FaceAnimMode mode);
extern void delayWithFace(unsigned long ms);
extern void enterIdle();
extern bool pressingCheck(String cmd, int ms);

// Pose/animation prototypes
void runRestPose();
void runStandPose(int face = 1);
void runWavePose();
void runDancePose();
void runSwimPose();
void runPointPose();
void runPushupPose();
void runBowPose();
void runCutePose();
void runFreakyPose();
void runWormPose();
void runShakePose();
void runShrugPose();
void runDeadPose();
void runCrabPose();
void runWigglePose();
void runWalkPose();
void runWalkBackward();
void runTurnLeft();
void runTurnRight();
void runTouchTurnLeft();
void runTouchTurnRight();
void runSupermanPose();
void runSupermanShake(bool alternate);
void runPissLeftPose();
void runPissRightPose();

// ====== POSES ======
inline void runRestPose() {
  Serial.println(F("REST"));
  setFaceWithMode("rest", FACE_ANIM_BOOMERANG);
  for (int i = 0; i < 8; i++) setServoAngle(i, 90);
}

inline void runStandPose(int face) {
  Serial.println(F("STAND"));
  if (face == 1) setFaceWithMode("stand", FACE_ANIM_ONCE);
  setServoAngle(R1, 135);
  setServoAngle(R2, 45);
  setServoAngle(L1, 45);
  setServoAngle(L2, 135);
  setServoAngle(R4, 0);
  setServoAngle(R3, 180);
  setServoAngle(L3, 0);
  setServoAngle(L4, 180);
  if (face == 1) enterIdle();
}

inline void runWavePose() {
  Serial.println(F("WAVE"));
  playBuzzerCue(BUZZER_CUE_WAVE_START);
  setFaceWithMode("wave", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(R4, 80);
  setServoAngle(L3, 180);
  setServoAngle(L2, 60);
  setServoAngle(R1, 100);
  delayWithFace(200);
  setServoAngle(L3, 180);
  delayWithFace(300);
  for (int i = 0; i < 4; i++) {
    playBuzzerCue(BUZZER_CUE_WAVE_BEAT);
    setServoAngle(L3, 180);
    delayWithFace(300);
    setServoAngle(L3, 100);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "wave") currentCommand = "";
}

inline void runDancePose() {
  Serial.println(F("DANCE"));
  playBuzzerCue(BUZZER_CUE_CUTE);
  setFaceWithMode("dance", FACE_ANIM_LOOP);
  setServoAngle(R1, 90);
  setServoAngle(R2, 90);
  setServoAngle(L1, 90);
  setServoAngle(L2, 90);
  setServoAngle(R4, 160);
  setServoAngle(R3, 160);
  setServoAngle(L3, 10);
  setServoAngle(L4, 10);
  delayWithFace(300);
  for (int i = 0; i < 5; i++) {
    playBuzzerCue(BUZZER_CUE_DANCE_BEAT);
    setServoAngle(R4, 115);
    setServoAngle(R3, 115);
    setServoAngle(L3, 10);
    setServoAngle(L4, 10);
    delayWithFace(300);
    setServoAngle(R4, 160);
    setServoAngle(R3, 160);
    setServoAngle(L3, 65);
    setServoAngle(L4, 65);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "dance") currentCommand = "";
}

inline void runSwimPose() {
  Serial.println(F("SWIM"));
  playBuzzerCue(BUZZER_CUE_SWIM);
  setFaceWithMode("swim", FACE_ANIM_ONCE);
  for (int i = 0; i < 8; i++) setServoAngle(i, 90);
  for (int i = 0; i < 4; i++) {
    playBuzzerCue(BUZZER_CUE_SWIM);
    setServoAngle(R1, 135);
    setServoAngle(R2, 45);
    setServoAngle(L1, 45);
    setServoAngle(L2, 135);
    delayWithFace(400);
    setServoAngle(R1, 90);
    setServoAngle(R2, 90);
    setServoAngle(L1, 90);
    setServoAngle(L2, 90);
    delayWithFace(400);
  }
  runStandPose(1);
  if (currentCommand == "swim") currentCommand = "";
}

inline void runPointPose() {
  Serial.println(F("POINT"));
  playBuzzerCue(BUZZER_CUE_POINT);
  setFaceWithMode("point", FACE_ANIM_BOOMERANG);
  setServoAngle(L2, 60);
  setServoAngle(R1, 135);
  setServoAngle(R2, 100);
  setServoAngle(L4, 180);
  setServoAngle(L1, 25);
  setServoAngle(L3, 145);
  setServoAngle(R4, 80);
  setServoAngle(R3, 170);
  delayWithFace(2000);
  runStandPose(1);
  if (currentCommand == "point") currentCommand = "";
}

inline void runPushupPose() {
  Serial.println(F("PUSHUP"));
  playBuzzerCue(BUZZER_CUE_PUSHUP_DOWN);
  setFaceWithMode("pushup", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(L1, 0);
  setServoAngle(R1, 180);
  setServoAngle(L3, 90);
  setServoAngle(R3, 90);
  delayWithFace(500);
  for (int i = 0; i < 4; i++) {
    playBuzzerCue(BUZZER_CUE_PUSHUP_DOWN);
    setServoAngle(L3, 0);
    setServoAngle(R3, 180);
    delayWithFace(600);
    playBuzzerCue(BUZZER_CUE_PUSHUP_UP);
    setServoAngle(L3, 90);
    setServoAngle(R3, 90);
    delayWithFace(500);
  }
  runStandPose(1);
  if (currentCommand == "pushup") currentCommand = "";
}

inline void runBowPose() {
  Serial.println(F("BOW"));
  playBuzzerCue(BUZZER_CUE_BOW);
  setFaceWithMode("bow", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(L1, 0);
  setServoAngle(R1, 180);
  setServoAngle(L3, 0);
  setServoAngle(R3, 180);
  setServoAngle(L2, 180);
  setServoAngle(R2, 0);
  setServoAngle(R4, 0);
  setServoAngle(L4, 180);
  delayWithFace(600);
  setServoAngle(L3, 90);
  setServoAngle(R3, 90);
  delayWithFace(3000);
  runStandPose(1);
  if (currentCommand == "bow") currentCommand = "";
}

inline void runCutePose() {
  Serial.println(F("CUTE"));
  playBuzzerCue(BUZZER_CUE_CUTE);
  setFaceWithMode("cute", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(L2, 160);
  setServoAngle(R2, 20);
  setServoAngle(R4, 180);
  setServoAngle(L4, 0);

  setServoAngle(L1, 0);
  setServoAngle(R1, 180);
  setServoAngle(L3, 180);
  setServoAngle(R3, 0);
  delayWithFace(200);
  for (int i = 0; i < 5; i++) {
    setServoAngle(R4, 180);
    setServoAngle(L4, 45);
    delayWithFace(300);
    setServoAngle(R4, 135);
    setServoAngle(L4, 0);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "cute") currentCommand = "";
}

inline void runFreakyPose() {
  Serial.println(F("FREAKY"));
  playBuzzerCue(BUZZER_CUE_FREAKY);
  setFaceWithMode("freaky", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(L1, 0);
  setServoAngle(R1, 180);
  setServoAngle(L2, 180);
  setServoAngle(R2, 0);
  setServoAngle(R4, 90);
  setServoAngle(R3, 0);
  delayWithFace(200);
  for (int i = 0; i < 3; i++) {
    setServoAngle(R3, 25);
    delayWithFace(400);
    setServoAngle(R3, 0);
    delayWithFace(400);
  }
  runStandPose(1);
  if (currentCommand == "freaky") currentCommand = "";
}

inline void runWormPose() {
  Serial.println(F("WORM"));
  playBuzzerCue(BUZZER_CUE_WORM);
  setFaceWithMode("worm", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(R1, 180);
  setServoAngle(R2, 0);
  setServoAngle(L1, 0);
  setServoAngle(L2, 180);
  setServoAngle(R4, 90);
  setServoAngle(R3, 90);
  setServoAngle(L3, 90);
  setServoAngle(L4, 90);
  delayWithFace(200);
  for (int i = 0; i < 5; i++) {
    playBuzzerCue(BUZZER_CUE_WORM);
    setServoAngle(R3, 45);
    setServoAngle(L3, 135);
    setServoAngle(R4, 45);
    setServoAngle(L4, 135);
    delayWithFace(300);
    setServoAngle(R3, 135);
    setServoAngle(L3, 45);
    setServoAngle(R4, 135);
    setServoAngle(L4, 45);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "worm") currentCommand = "";
}

inline void runShakePose() {
  Serial.println(F("SHAKE"));
  playBuzzerCue(BUZZER_CUE_SHAKE);
  setFaceWithMode("shake", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(R1, 135);
  setServoAngle(L1, 45);
  setServoAngle(L3, 90);
  setServoAngle(R3, 90);
  setServoAngle(L2, 90);
  setServoAngle(R2, 90);
  delayWithFace(200);
  for (int i = 0; i < 5; i++) {
    playBuzzerCue(BUZZER_CUE_SHAKE);
    setServoAngle(R4, 45);
    setServoAngle(L4, 135);
    delayWithFace(300);
    setServoAngle(R4, 0);
    setServoAngle(L4, 180);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "shake") currentCommand = "";
}

inline void runShrugPose() {
  Serial.println(F("SHRUG"));
  playBuzzerCue(BUZZER_CUE_SHRUG);
  runStandPose(0);
  setFaceWithMode("dead", FACE_ANIM_ONCE);
  delayWithFace(200);
  setServoAngle(R3, 90);
  setServoAngle(R4, 90);
  setServoAngle(L3, 90);
  setServoAngle(L4, 90);
  delayWithFace(1000);
  setFaceWithMode("shrug", FACE_ANIM_ONCE);
  setServoAngle(R3, 0);
  setServoAngle(R4, 180);
  setServoAngle(L3, 180);
  setServoAngle(L4, 0);
  delayWithFace(1500);
  runStandPose(1);
  if (currentCommand == "shrug") currentCommand = "";
}

inline void runDeadPose() {
  Serial.println(F("DEAD"));
  playBuzzerCue(BUZZER_CUE_DEAD);
  runStandPose(0);
  setFaceWithMode("dead", FACE_ANIM_BOOMERANG);
  delayWithFace(200);
  setServoAngle(R3, 90);
  setServoAngle(R4, 90);
  setServoAngle(L3, 90);
  setServoAngle(L4, 90);
  if (currentCommand == "dead") currentCommand = "";
}

inline void runCrabPose() {
  Serial.println(F("CRAB"));
  playBuzzerCue(BUZZER_CUE_CRAB);
  setFaceWithMode("crab", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(R1, 90);
  setServoAngle(R2, 90);
  setServoAngle(L1, 90);
  setServoAngle(L2, 90);
  setServoAngle(R4, 0);
  setServoAngle(R3, 180);
  setServoAngle(L3, 45);
  setServoAngle(L4, 135);
  for (int i = 0; i < 5; i++) {
    playBuzzerCue(BUZZER_CUE_CRAB);
    setServoAngle(R4, 45);
    setServoAngle(R3, 135);
    setServoAngle(L3, 0);
    setServoAngle(L4, 180);
    delayWithFace(300);
    setServoAngle(R4, 0);
    setServoAngle(R3, 180);
    setServoAngle(L3, 45);
    setServoAngle(L4, 135);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "crab") currentCommand = "";
}

inline void runWigglePose() {
  static bool dir = false;

  if (!touchWiggleActive && wiggleRunoutCount <= 0) {
    dir = false;
    runStandPose(1);
    currentCommand = "";
    return;
  }

  if (touchWiggleActive && wiggleRunoutCount == 0) {
    // Normalny krok mruczenia
  } else if (wiggleRunoutCount > 0) {
    // Runout po puszczeniu
    wiggleRunoutCount--;
  }

  if (dir) {
    playBuzzerCue(BUZZER_CUE_WIGGLE);
    setServoAngle(R3, 163);
    setServoAngle(L3, 20);
    setServoAngle(R4, 23);
    setServoAngle(L4, 163);
  } else {
    setServoAngle(R3, 180);
    setServoAngle(L3, 0);
    setServoAngle(R4, 0);
    setServoAngle(L4, 180);
  }
  dir = !dir;
}

// Touch turns reuse the calibrated normal turn sequence with a fixed length.
inline void runTouchTurnLeft() {
  // Reuse the calibrated normal turn, without permanently changing walkCycles.
  const int savedWalkCycles = walkCycles;
  const String savedCommand = currentCommand;
  walkCycles = 4;
  currentCommand = "left";
  runTurnLeft();
  walkCycles = savedWalkCycles;
  // Preserve a new web/serial command received while the turn was running.
  if (currentCommand == "left") currentCommand = savedCommand;
}

inline void runTouchTurnRight() {
  // Reuse the calibrated normal turn, without permanently changing walkCycles.
  const int savedWalkCycles = walkCycles;
  const String savedCommand = currentCommand;
  walkCycles = 4;
  currentCommand = "right";
  runTurnRight();
  walkCycles = savedWalkCycles;
  // Preserve a new web/serial command received while the turn was running.
  if (currentCommand == "right") currentCommand = savedCommand;
}

inline void runSupermanPose() {
  Serial.println(F("SUPERMAN"));
  // Straight, extended limbs: confirmed against Sesame's angle guide.
  setServoAngle(R1, 180);
  setServoAngle(R2, 0);
  setServoAngle(L1, 0);
  setServoAngle(L2, 180);
  // Straight lower joints for the fully extended Superman pose.
  setServoAngle(R4, 90);
  setServoAngle(R3, 90);
  setServoAngle(L3, 90);
  setServoAngle(L4, 90);
}

inline void runSupermanShake(bool alternate) {
  // Gentle 20-degree peak-to-peak wobble around straight, out of phase.
  const int rightOffset = alternate ? 10 : -10;
  const int leftOffset = -rightOffset;
  setServoAngle(R4, 90 + rightOffset);
  setServoAngle(R3, 90 - rightOffset);
  setServoAngle(L3, 90 + leftOffset);
  setServoAngle(L4, 90 - leftOffset);
}

// --- MOVEMENT ANIMATIONS ---
inline void runWalkPose() {
  Serial.println(F("WALK FWD"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  // Initial Step
  setServoAngle(R3, 135); setServoAngle(L3, 45);
  setServoAngle(R2, 100); setServoAngle(L1, 25);
  if (!pressingCheck("forward", frameDelay)) return;
  
  for (int i = 0; i < walkCycles; i++) {
    setServoAngle(R3, 135); setServoAngle(L3, 0);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(L4, 135); setServoAngle(L2, 90);
    setServoAngle(R4, 0); setServoAngle(R1, 180);
    if (!pressingCheck("forward", frameDelay)) return;    
    setServoAngle(R2, 45); setServoAngle(L1, 90);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(R4, 45); setServoAngle(L4, 180);
    if (!pressingCheck("forward", frameDelay)) return;
    setServoAngle(R3, 180); setServoAngle(L3, 45);
    setServoAngle(R2, 90); setServoAngle(L1, 0);
    if (!pressingCheck("forward", frameDelay)) return;  
    setServoAngle(L2, 135); setServoAngle(R1, 90);
    if (!pressingCheck("forward", frameDelay)) return;
  }
  runStandPose(1);
}

// Logic reversed from Walk
inline void runWalkBackward() {
  Serial.println(F("WALK BACK"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  if (!pressingCheck("backward", frameDelay)) return;

  for (int i = 0; i < walkCycles; i++) {
    setServoAngle(R3, 135);
    setServoAngle(L3, 0);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(L4, 135);
    setServoAngle(L2, 135);
    setServoAngle(R4, 0);
    setServoAngle(R1, 90);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(R2, 90);
    setServoAngle(L1, 0);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(R4, 45);
    setServoAngle(L4, 180);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(R3, 180);
    setServoAngle(L3, 45);
    setServoAngle(R2, 45);
    setServoAngle(L1, 90);
    if (!pressingCheck("backward", frameDelay)) return;
    setServoAngle(L2, 90);
    setServoAngle(R1, 180);
    if (!pressingCheck("backward", frameDelay)) return;
  }
  runStandPose(1);
}

// Simple turn logic
inline void runTurnLeft() {
  Serial.println(F("TURN LEFT"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  for (int i = 0; i < walkCycles; i++) {
    //legset 1 (R1 L2)
    setServoAngle(R3, 135);
    setServoAngle(L4, 135);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R1, 180);
    setServoAngle(L2, 180);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R3, 180);
    setServoAngle(L4, 180);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R1, 135);
    setServoAngle(L2, 135);
    if (!pressingCheck("left", frameDelay)) return;
    //legset 2 (R2 L1)
    setServoAngle(R4, 45);
    setServoAngle(L3, 45);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R2, 90);
    setServoAngle(L1, 90);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R4, 0);
    setServoAngle(L3, 0);
    if (!pressingCheck("left", frameDelay)) return;
    setServoAngle(R2, 45);
    setServoAngle(L1, 45);
    if (!pressingCheck("left", frameDelay)) return;
  }
  runStandPose(1);
}

inline void runTurnRight() {
  Serial.println(F("TURN RIGHT"));
  setFaceWithMode("walk", FACE_ANIM_ONCE);
  for (int i = 0; i < walkCycles; i++) {
    //legset 2 (R2 L1)
    setServoAngle(R4, 45);
    setServoAngle(L3, 45);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R2, 0);
    setServoAngle(L1, 0);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R4, 0);
    setServoAngle(L3, 0);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R2, 45);
    setServoAngle(L1, 45);
    if (!pressingCheck("right", frameDelay)) return;
    //legset 1 (R1 L2)
    setServoAngle(R3, 135);
    setServoAngle(L4, 135);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R1, 90);
    setServoAngle(L2, 90);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R3, 180);
    setServoAngle(L4, 180);
    if (!pressingCheck("right", frameDelay)) return;
    setServoAngle(R1, 135);
    setServoAngle(L2, 135);
    if (!pressingCheck("right", frameDelay)) return;
  }
  runStandPose(1);
}

inline void runPissLeftPose() {
  Serial.println(F("PISS LEFT"));
  setFaceWithMode("pushup", FACE_ANIM_ONCE);
  setServoAngle(R3, 125);
  setServoAngle(R4, 55);
  setServoAngle(L3, 0);
  setServoAngle(L4, 180);
  setServoAngle(R1, 135);
  setServoAngle(R2, 45);
  setServoAngle(L1, 45);
  setServoAngle(L2, 135);
  setServoAngle(L4, 40);
  delayWithFace(1500);
  for (int i = 0; i < 3; i++) {
    setServoAngle(L4, 40+20);
    delayWithFace(100);
    setServoAngle(L4, 40-20);
    delayWithFace(100);
  }
  runStandPose(1);
  if (currentCommand == "pissleft") currentCommand = "";
}

inline void runPissRightPose() {
  Serial.println(F("PISS RIGHT"));
  setFaceWithMode("pushup", FACE_ANIM_ONCE);
  setServoAngle(R3, 180);
  setServoAngle(R4, 0);
  setServoAngle(L3, 55);
  setServoAngle(L4, 125);
  setServoAngle(R1, 135);
  setServoAngle(R2, 45);
  setServoAngle(L1, 45);
  setServoAngle(L2, 135);
  setServoAngle(R4, 130);
  delayWithFace(1500);
  for (int i = 0; i < 3; i++) {
    setServoAngle(R4, 130+20);
    delayWithFace(100);
    setServoAngle(R4, 130-20);
    delayWithFace(100);
  }
  runStandPose(1);
  if (currentCommand == "pissright") currentCommand = "";
}
