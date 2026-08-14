#include <Arduino.h>
#include <ESP32Servo.h>

// ======================================================================
// --- CONFIGURATION ---
// ======================================================================

Servo servos[8];

// Motor Pin Mapping
// Index: 0  1  2  3  4   5   6   7
const int servoPins[8] = {1, 2, 4, 6, 8, 10, 13, 14};
// Distro board V3 pins: (uncomment if needed)
// const int servoPins[8] = {4, 5, 6, 7, 10, 11, 12, 13};
// Distro board V2 pins: (uncomment if needed)
// const int servoPins[8] = {4, 5, 6, 7, 15, 16, 17, 18};
// Distro board V1 pins: (uncomment if needed)
// const int servoPins[8] = {15, 2, 23, 19, 4, 16, 17, 18};



// Pulse width settings
const int MIN_PULSE = 732;
const int MAX_PULSE = 2929;

// ======================================================================
// --- SETUP ---
// ======================================================================

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("-----------------------------------");
  Serial.println("   Sesame Motor Tester Interface   ");
  Serial.println("-----------------------------------");
  Serial.println("Commands:");
  Serial.println("1. id,angle   -> e.g. '0,90'");
  Serial.println("2. all,angle  -> e.g. 'all,90'");
  Serial.println("3. stop       -> Detaches/Powers down motors");
  Serial.println("-----------------------------------");
  Serial.println("Status: Motors are currently OFF (Limp).");

  // Allocate timers for ESP32Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // We do NOT attach motors here. They stay limp until a command is sent.
}

// ======================================================================
// --- MAIN LOOP ---
// ======================================================================

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines

    if (input.length() == 0) return;

    // --- Check for STOP command ---
    if (input.equalsIgnoreCase("stop")) {
      stopMotors();
      return;
    }

    // --- Parse "key,value" format ---
    int commaIndex = input.indexOf(',');
    
    if (commaIndex != -1) {
      String cmd = input.substring(0, commaIndex);
      String valStr = input.substring(commaIndex + 1);
      int angle = valStr.toInt();

      // Check Angle Limits
      if (angle < 0) angle = 0;
      if (angle > 180) angle = 180;

      // Check if command is "all" or a motor ID
      if (cmd.equalsIgnoreCase("all")) {
        moveAll(angle);
      } else {
        // Assume it's a number
        int motorId = cmd.toInt();
        // Basic check to ensure "0" wasn't actually text garbage
        if (motorId == 0 && cmd.charAt(0) != '0') {
           Serial.println("Error: Invalid Motor ID");
        } else {
           moveMotor(motorId, angle);
        }
      }
      
    } else {
      Serial.println("Error: Invalid format. Use 'id,angle', 'all,angle', or 'stop'.");
    }
  }
}

// ======================================================================
// --- HELPER FUNCTIONS ---
// ======================================================================

void moveMotor(int id, int angle) {
  if (id < 0 || id > 7) {
    Serial.println("Error: Motor ID must be 0-7");
    return;
  }

  // If servo isn't attached yet (powered off), attach it now
  if (!servos[id].attached()) {
    servos[id].setPeriodHertz(50);
    servos[id].attach(servoPins[id], MIN_PULSE, MAX_PULSE);
  }

  servos[id].write(angle);
  
  Serial.print("OK: Motor ");
  Serial.print(id);
  Serial.print(" -> ");
  Serial.println(angle);
}

void moveAll(int angle) {
  Serial.print("Moving ALL to ");
  Serial.println(angle);
  for (int i = 0; i < 8; i++) {
    moveMotor(i, angle);
  }
}

void stopMotors() {
  Serial.println("Stopping (Detaching) all motors...");
  for (int i = 0; i < 8; i++) {
    if (servos[i].attached()) {
      servos[i].detach();
    }
  }
  Serial.println("Motors are now OFF.");
}