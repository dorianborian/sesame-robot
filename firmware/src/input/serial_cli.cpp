#include "serial_cli.h"
#include "../../movement-sequences.h"
#include "../gpio/gpio_servos.h"
#include "../face/face_anim.h"
#include "../globals.h"

static const size_t CLI_BUF_SIZE = 32;

static char cmdBuf[CLI_BUF_SIZE];
static byte cmdLen = 0;

static void handleMoveCommand(const char* buf) {
    if (strcmp(buf, "run walk") == 0 || strcmp(buf, "rn wf") == 0) {
        currentCommand = "forward";
        runWalkPose();
        currentCommand = "";
    } 
    else if (strcmp(buf, "rn wb") == 0) {
        currentCommand = "backward";
        runWalkBackward();
        currentCommand = "";
    } 
    else if (strcmp(buf, "rn tl") == 0) {
        currentCommand = "left";
        runTurnLeft();
        currentCommand = "";
    } 
    else if (strcmp(buf, "rn tr") == 0) {
        currentCommand = "right";
        runTurnRight();
        currentCommand = "";
    } 
    else if (strcmp(buf, "run rest") == 0 || strcmp(buf, "rn rs") == 0) {
        runRestPose();
    } 
    else if (strcmp(buf, "run stand") == 0 || strcmp(buf, "rn st") == 0) {
        runStandPose(1);
    } 
    else if (strcmp(buf, "rn wv") == 0) {
        currentCommand = "wave";
        runWavePose();
    } 
    else if (strcmp(buf, "rn dn") == 0) {
        currentCommand = "dance";
        runDancePose();
    } 
    else if (strcmp(buf, "rn sw") == 0) {
        currentCommand = "swim";
        runSwimPose();
    } 
    else if (strcmp(buf, "rn pt") == 0) {
        currentCommand = "point";
        runPointPose();
    } 
    else if (strcmp(buf, "rn pu") == 0) {
        currentCommand = "pushup";
        runPushupPose();
    } 
    else if (strcmp(buf, "rn bw") == 0) {
        currentCommand = "bow";
        runBowPose();
    } 
    else if (strcmp(buf, "rn ct") == 0) {
        currentCommand = "cute";
        runCutePose();
    } 
    else if (strcmp(buf, "rn fk") == 0) {
        currentCommand = "freaky";
        runFreakyPose();
    } 
    else if (strcmp(buf, "rn wm") == 0) {
        currentCommand = "worm";
        runWormPose();
    } 
    else if (strcmp(buf, "rn sk") == 0) {
        currentCommand = "shake";
        runShakePose();
    } 
    else if (strcmp(buf, "rn sg") == 0) {
        currentCommand = "shrug";
        runShrugPose();
    } 
    else if (strcmp(buf, "rn dd") == 0) {
        currentCommand = "dead";
        runDeadPose();
    } 
    else if (strcmp(buf, "rn cb") == 0) {
        currentCommand = "crab";
        runCrabPose();
    }
}

static bool handleSubtrimCommand(const char* buf) {
  if (strcmp(buf, "subtrim") == 0 || strcmp(buf, "st") == 0) {
    printSubtrims();
    return true;
  }

  if (strcmp(buf, "subtrim save") == 0 || strcmp(buf, "st save") == 0) {
    printSubtrimsSaveCommand();
    return true;
  }

  if (strncmp(buf, "subtrim reset", 13) == 0 || strncmp(buf, "st reset", 8) == 0) {
    resetAllSubtrims();
    Serial.println("All subtrim values reset to 0");
    return true;
  }

  // "subtrim <motor> <value>"  or  "st <motor> <value>"
  if (strncmp(buf, "subtrim ", 8) == 0 || strncmp(buf, "st ", 3) == 0) {
    const char* params = (buf[1] == 't') ? buf + 3 : buf + 8;
    int m, v;

    if (sscanf(params, "%d %d", &m, &v) == 2) {
      if (m < 0 || m >= 8)  {
        Serial.println("Invalid motor number (0-7)");
        return true; 
      }

      if (v < -90 || v > 90) {
        Serial.println("Subtrim must be -90..+90");   
        return true; 
      }

      setServoSubtrim(m, (int8_t)v);
      Serial.print("Motor "); Serial.print(m);
      Serial.print(" subtrim = "); if (v >= 0) Serial.print("+"); Serial.println(v);
    }

    return true;
  }

  return false;
}

static bool handleServoDirectCommand(const char* buf) {
  int angle;

  if (strncmp(buf, "all ", 4) == 0 && sscanf(buf + 4, "%d", &angle) == 1) {
    for (int i = 0; i < 8; i++) setServoAngle(i, angle);
    Serial.print("All servos → "); Serial.println(angle);
    return true;
  }

  int motor;

  if (sscanf(buf, "%d %d", &motor, &angle) == 2) {
    if (motor >= 0 && motor < 8) {
      setServoAngle(motor, angle);
      Serial.print("Servo "); Serial.print(motor); Serial.print(" → "); Serial.println(angle);
    } else {
      Serial.println("Invalid motor number (0-7)");
    }
    return true;
  }

  return false;
}

static void dispatchCommand(const char* buf) {
  recordInput();
  if (handleSubtrimCommand(buf)) return;
  if (handleServoDirectCommand(buf)) return;
  handleMoveCommand(buf);
}

void processSerialCLI() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (cmdLen > 0) {
        cmdBuf[cmdLen] = '\0';
        dispatchCommand(cmdBuf);
        cmdLen = 0;
      }
    } else if (cmdLen < CLI_BUF_SIZE - 1) {
      cmdBuf[cmdLen++] = c;
    }
  }
}
