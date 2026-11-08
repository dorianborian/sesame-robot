#pragma once

#include <Arduino.h>

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

const String ServoNames[]={"R1","R2","L1","L2","R4","R3","L3","L4"};

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
void runWalkPose();
void runWalkBackward();
void runTurnLeft();
void runTurnRight();

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
  setFaceWithMode("wave", FACE_ANIM_ONCE); 
  runStandPose(0); 
  delayWithFace(200);
  setServoAngle(R4, 80); setServoAngle(L3, 180); 
  setServoAngle(L2, 90); setServoAngle(R1, 100); 
  delayWithFace(200);
  setServoAngle(L3, 180); 
  delayWithFace(300); 
  for (int i = 0; i < 4; i++) { 
    setServoAngle(L3, 180); delayWithFace(300); 
    setServoAngle(L3, 100); delayWithFace(300); 
  } 
  runStandPose(1); 
  if (currentCommand == "wave") currentCommand = "";
}

inline void runDancePose() { 
  Serial.println(F("DANCE")); 
  setFaceWithMode("dance", FACE_ANIM_LOOP); 
  setServoAngle(R1, 90); setServoAngle(R2, 90); 
  setServoAngle(L1, 90); setServoAngle(L2, 90); 
  setServoAngle(R4, 160); setServoAngle(R3, 160); 
  setServoAngle(L3, 10); setServoAngle(L4, 10); 
  delayWithFace(300); 
  for (int i = 0; i < 5; i++) { 
    setServoAngle(R4, 115); setServoAngle(R3, 115); 
    setServoAngle(L3, 10); setServoAngle(L4, 10); 
    delayWithFace(300); 
    setServoAngle(R4, 160); setServoAngle(R3, 160); 
    setServoAngle(L3, 65); setServoAngle(L4, 65); 
    delayWithFace(300); 
  } 
  runStandPose(1); 
  if (currentCommand == "dance") currentCommand = "";
}

inline void runSwimPose() { 
  Serial.println(F("SWIM")); 
  setFaceWithMode("swim", FACE_ANIM_ONCE); 
  for (int i = 0; i < 8; i++) setServoAngle(i, 90); 
  for (int i = 0; i < 4; i++) { 
    setServoAngle(R1, 135); setServoAngle(R2, 45); 
    setServoAngle(L1, 45); setServoAngle(L2, 135); 
    delayWithFace(400); 
    setServoAngle(R1, 90); setServoAngle(R2, 90); 
    setServoAngle(L1, 90); setServoAngle(L2, 90); 
    delayWithFace(400); 
  } 
  runStandPose(1); 
  if (currentCommand == "swim") currentCommand = "";
}

inline void runPointPose() { 
  Serial.println(F("POINT")); 
  setFaceWithMode("point", FACE_ANIM_BOOMERANG); 
  setServoAngle(L2, 90); setServoAngle(R1, 135); 
  setServoAngle(R2, 100); setServoAngle(L4, 180); 
  setServoAngle(L1, 25); setServoAngle(L3, 145);
  setServoAngle(R4, 80); setServoAngle(R3, 170); 
  delayWithFace(2000); 
  runStandPose(1); 
  if (currentCommand == "point") currentCommand = "";
}

inline void runPushupPose() {
  Serial.println(F("PUSHUP"));
  setFaceWithMode("pushup", FACE_ANIM_ONCE);
  runStandPose(0); 
  delayWithFace(200);
  setServoAngle(L1, 0);
  setServoAngle(R1, 180);
  setServoAngle(L3, 90);
  setServoAngle(R3, 90);
  delayWithFace(500);
  for (int i = 0; i < 4; i++) {
    setServoAngle(L3, 0);
    setServoAngle(R3, 180);
    delayWithFace(600);
    setServoAngle(L3, 90);
    setServoAngle(R3, 90);
    delayWithFace(500);
  }
  runStandPose(1);
  if (currentCommand == "pushup") currentCommand = "";
}

inline void runBowPose() {
  Serial.println(F("BOW"));
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
  setFaceWithMode("worm", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(R1, 180); setServoAngle(R2, 0); setServoAngle(L1, 0); setServoAngle(L2, 180);
  setServoAngle(R4, 90); setServoAngle(R3, 90); setServoAngle(L3, 90); setServoAngle(L4, 90);
  delayWithFace(200);
  for(int i=0; i<5; i++) {
    setServoAngle(R3, 45); setServoAngle(L3, 135); setServoAngle(R4, 45); setServoAngle(L4, 135);
    delayWithFace(300);
    setServoAngle(R3, 135); setServoAngle(L3, 45); setServoAngle(R4, 135); setServoAngle(L4, 45);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "worm") currentCommand = "";
}

inline void runShakePose() {
  Serial.println(F("SHAKE"));
  setFaceWithMode("shake", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(R1, 135); setServoAngle(L1, 45); setServoAngle(L3, 90); setServoAngle(R3, 90);
  setServoAngle(L2, 90); setServoAngle(R2, 90);
  delayWithFace(200);
  for(int i=0; i<5; i++) {
    setServoAngle(R4, 45); setServoAngle(L4, 135);
    delayWithFace(300);
    setServoAngle(R4, 0); setServoAngle(L4, 180);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "shake") currentCommand = "";
}

inline void runShrugPose() {
  Serial.println(F("SHRUG"));
  runStandPose(0);
  setFaceWithMode("dead", FACE_ANIM_ONCE);
  delayWithFace(200);
  setServoAngle(R3, 90); setServoAngle(R4, 90); setServoAngle(L3, 90); setServoAngle(L4, 90);
  delayWithFace(1000);
  setFaceWithMode("shrug", FACE_ANIM_ONCE);
  setServoAngle(R3, 0); setServoAngle(R4, 180); setServoAngle(L3, 180); setServoAngle(L4, 0);
  delayWithFace(1500);
  runStandPose(1);
  if (currentCommand == "shrug") currentCommand = "";
}

inline void runDeadPose() {
  Serial.println(F("DEAD"));
  runStandPose(0);
  setFaceWithMode("dead", FACE_ANIM_BOOMERANG);
  delayWithFace(200);
  setServoAngle(R3, 90); setServoAngle(R4, 90); setServoAngle(L3, 90); setServoAngle(L4, 90);
  if (currentCommand == "dead") currentCommand = "";
}

inline void runCrabPose() {
  Serial.println(F("CRAB"));
  setFaceWithMode("crab", FACE_ANIM_ONCE);
  runStandPose(0);
  delayWithFace(200);
  setServoAngle(R1, 90); setServoAngle(R2, 90); setServoAngle(L1, 90); setServoAngle(L2, 90);
  setServoAngle(R4, 0); setServoAngle(R3, 180); setServoAngle(L3, 45); setServoAngle(L4, 135);
  for(int i=0; i<5; i++) {
    setServoAngle(R4, 45); setServoAngle(R3, 135); setServoAngle(L3, 0); setServoAngle(L4, 180);
    delayWithFace(300);
    setServoAngle(R4, 0); setServoAngle(R3, 180); setServoAngle(L3, 45); setServoAngle(L4, 135);
    delayWithFace(300);
  }
  runStandPose(1);
  if (currentCommand == "crab") currentCommand = "";
}

// --- MOVEMENT ANIMATIONS ---
// --- REMASTERED MOVEMENT BY @SAIREOUS ---

inline void runWalkPose() {
  Serial.println(F("WALK FWD ALIVE FAST V5"));
  setFaceWithMode("walk", FACE_ANIM_LOOP);

  if (!pressingCheck("forward", frameDelay)) return;

  setServoAngle(R3, 143);
  setServoAngle(L4, 148);
  if (!pressingCheck("forward", frameDelay)) return;

  while (currentCommand == "forward") {
    // A swings forward while grounded B pushes the body forward.
    // Front legs use a slightly longer stride than rear legs.
    setServoAngle(R1, 170);
    setServoAngle(L2, 106);
    setServoAngle(L1, 78);
    setServoAngle(R2, 18);
    if (!pressingCheck("forward", frameDelay)) return;

    // Soft, simultaneous support exchange: A lands while B lifts.
    setServoAngle(R3, 171);
    setServoAngle(L4, 176);
    setServoAngle(L3, 38);
    setServoAngle(R4, 42);
    if (!pressingCheck("forward", frameDelay)) return;

    // B swings forward while grounded A pushes the body forward.
    setServoAngle(L1, 10);
    setServoAngle(R2, 76);
    setServoAngle(R1, 102);
    setServoAngle(L2, 164);
    if (!pressingCheck("forward", frameDelay)) return;

    // B lands while A lifts. The next cycle continues immediately without
    // returning to the rigid stand pose.
    setServoAngle(L3, 10);
    setServoAngle(R4, 14);
    setServoAngle(R3, 143);
    setServoAngle(L4, 148);
    if (!pressingCheck("forward", frameDelay)) return;
  }

  runStandPose(1);
}

inline void runWalkBackward() {
  Serial.println(F("WALK BACK ALIVE FAST V5"));
  setFaceWithMode("walk", FACE_ANIM_LOOP);

  if (!pressingCheck("backward", frameDelay)) return;

  setServoAngle(R3, 143);
  setServoAngle(L4, 148);
  if (!pressingCheck("backward", frameDelay)) return;

  while (currentCommand == "backward") {
    // A swings backward while grounded B pushes the body backward.
    setServoAngle(R1, 102);
    setServoAngle(L2, 164);
    setServoAngle(L1, 10);
    setServoAngle(R2, 76);
    if (!pressingCheck("backward", frameDelay)) return;

    // A lands while B lifts.
    setServoAngle(R3, 171);
    setServoAngle(L4, 176);
    setServoAngle(L3, 38);
    setServoAngle(R4, 42);
    if (!pressingCheck("backward", frameDelay)) return;

    // B swings backward while grounded A pushes the body backward.
    setServoAngle(L1, 78);
    setServoAngle(R2, 18);
    setServoAngle(R1, 170);
    setServoAngle(L2, 106);
    if (!pressingCheck("backward", frameDelay)) return;

    // B lands while A lifts for the next uninterrupted cycle.
    setServoAngle(L3, 10);
    setServoAngle(R4, 14);
    setServoAngle(R3, 143);
    setServoAngle(L4, 148);
    if (!pressingCheck("backward", frameDelay)) return;
  }

  runStandPose(1);
}

inline void runTurnLeft() {
  Serial.println(F("TURN LEFT IN PLACE FAST V5"));
  setFaceWithMode("walk", FACE_ANIM_LOOP);

  if (!pressingCheck("left", frameDelay)) return;

  setServoAngle(R3, 143);
  setServoAngle(L4, 148);
  if (!pressingCheck("left", frameDelay)) return;

  while (currentCommand == "left") {
    // Lifted A resets for a left turn. Grounded B twists the body left:
    // the right side pushes forward and the left side pushes backward.
    setServoAngle(R1, 166);
    setServoAngle(L2, 160);
    setServoAngle(L1, 12);
    setServoAngle(R2, 18);
    if (!pressingCheck("left", frameDelay)) return;

    // A lands and B lifts without pausing the rhythm.
    setServoAngle(R3, 171);
    setServoAngle(L4, 176);
    setServoAngle(L3, 38);
    setServoAngle(R4, 42);
    if (!pressingCheck("left", frameDelay)) return;

    // Lifted B resets; grounded A completes the same leftward twist.
    setServoAngle(L1, 78);
    setServoAngle(R2, 76);
    setServoAngle(R1, 102);
    setServoAngle(L2, 106);
    if (!pressingCheck("left", frameDelay)) return;

    setServoAngle(L3, 10);
    setServoAngle(R4, 14);
    setServoAngle(R3, 143);
    setServoAngle(L4, 148);
    if (!pressingCheck("left", frameDelay)) return;
  }

  runStandPose(1);
}

inline void runTurnRight() {
  Serial.println(F("TURN RIGHT IN PLACE FAST V5"));
  setFaceWithMode("walk", FACE_ANIM_LOOP);
  if (!pressingCheck("right", frameDelay)) return;

  setServoAngle(R3, 143);
  setServoAngle(L4, 148);
  if (!pressingCheck("right", frameDelay)) return;

  while (currentCommand == "right") {
    // Exact opposite of the left turn: grounded B twists the body right.
    setServoAngle(R1, 102);
    setServoAngle(L2, 106);
    setServoAngle(L1, 78);
    setServoAngle(R2, 76);
    if (!pressingCheck("right", frameDelay)) return;

    setServoAngle(R3, 171);
    setServoAngle(L4, 176);
    setServoAngle(L3, 38);
    setServoAngle(R4, 42);
    if (!pressingCheck("right", frameDelay)) return;

    // Grounded A completes the rightward twist while B resets.
    setServoAngle(L1, 10);
    setServoAngle(R2, 18);
    setServoAngle(R1, 170);
    setServoAngle(L2, 164);
    if (!pressingCheck("right", frameDelay)) return;

    setServoAngle(L3, 10);
    setServoAngle(R4, 14);
    setServoAngle(R3, 143);
    setServoAngle(L4, 148);
    if (!pressingCheck("right", frameDelay)) return;
  }

  runStandPose(1);
}
