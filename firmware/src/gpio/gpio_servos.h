#pragma once
#include <Arduino.h>

extern int motorCurrentDelay;

void initServos();
void setServoAngle(uint8_t channel, int angle);
void setServoSubtrim(uint8_t channel, int8_t value);
int8_t getServoSubtrim(uint8_t channel);
void resetAllSubtrims();
void printSubtrims();
void printSubtrimsSaveCommand();
