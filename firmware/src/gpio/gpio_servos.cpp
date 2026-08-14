#include "gpio_servos.h"
#include <ESP32Servo.h>

static const int SERVO_PINS[8] = {1, 2, 4, 6, 8, 10, 13, 14};

static Servo servos[8];
static int8_t servoSubtrim[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int motorCurrentDelay = 20;

extern void delayWithFace(unsigned long ms);

void initServos() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  for (int i = 0; i < 8; i++) {
    servos[i].setPeriodHertz(50);
    servos[i].attach(SERVO_PINS[i], 732, 2929);
  }
}

void setServoAngle(uint8_t channel, int angle) {
  if (channel >= 8) return;
  int adjusted = constrain(angle + (int)servoSubtrim[channel], 0, 180);
  servos[channel].write(adjusted);
  delayWithFace(motorCurrentDelay);
}

void setServoSubtrim(uint8_t channel, int8_t value) {
  if (channel < 8) servoSubtrim[channel] = value;
}

int8_t getServoSubtrim(uint8_t channel) {
  return (channel < 8) ? servoSubtrim[channel] : 0;
}

void resetAllSubtrims() {
  for (int i = 0; i < 8; i++) servoSubtrim[i] = 0;
}

void printSubtrims() {
  Serial.println("Subtrim values:");
  for (int i = 0; i < 8; i++) {
    Serial.print("  Motor "); Serial.print(i); Serial.print(": ");
    if (servoSubtrim[i] >= 0) Serial.print("+");
    Serial.println(servoSubtrim[i]);
  }
}

void printSubtrimsSaveCommand() {
  Serial.println("Paste this into your code:");
  Serial.print("int8_t servoSubtrim[8] = {");
  for (int i = 0; i < 8; i++) {
    Serial.print(servoSubtrim[i]);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("};");
}
