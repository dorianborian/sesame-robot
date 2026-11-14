#pragma once

// ======================================================================
// Motion watchdog: a continuous move (forward/backward/left/right) started
// over /api/command stops by itself if the controller goes quiet.
//
// Added to the Sesame robot firmware by BotInfo.ai, 2026-10-11 (revised
// 2026-11-14). Licensed under the Apache License, Version 2.0, like the rest
// of the firmware: http://www.apache.org/licenses/LICENSE-2.0
//
// Pure logic, no Arduino calls, so it can be unit-tested on a desktop
// compiler. The sketch passes millis() in.
// ======================================================================

#include <stdint.h>
#include <string.h>

// Default timeout in ms. 0 starts the watchdog turned off. Runtime value:
// /setSettings?motionTimeout=<ms> (clamped to the range below).
#ifndef MOTION_TIMEOUT_MS
#define MOTION_TIMEOUT_MS 3000
#endif
#define MOTION_TIMEOUT_MIN_MS 500
#define MOTION_TIMEOUT_MAX_MS 60000

// Moves that loop() repeats until something else replaces them.
inline bool isContinuousMove(const char* cmd) {
  return strcmp(cmd, "forward") == 0 || strcmp(cmd, "backward") == 0 ||
         strcmp(cmd, "left") == 0 || strcmp(cmd, "right") == 0;
}

inline uint32_t clampMotionTimeoutMs(long ms) {
  if (ms < MOTION_TIMEOUT_MIN_MS) return MOTION_TIMEOUT_MIN_MS;
  if (ms > MOTION_TIMEOUT_MAX_MS) return MOTION_TIMEOUT_MAX_MS;
  return (uint32_t)ms;
}

class MotionWatchdog {
 public:
  explicit MotionWatchdog(uint32_t timeoutMs)
      : timeoutMs_(timeoutMs), lastMs_(0), armed_(false) {}

  void setTimeoutMs(uint32_t ms) { timeoutMs_ = ms; }
  uint32_t timeoutMs() const { return timeoutMs_; }
  bool armed() const { return armed_; }

  // Any accepted /api/command: the controller is alive, restart the countdown.
  void feed(uint32_t nowMs) { lastMs_ = nowMs; }

  // A continuous move was sent (or re-sent) over /api/command: watch it.
  void arm(uint32_t nowMs) {
    armed_ = true;
    lastMs_ = nowMs;
  }

  // Another controller took over (e.g. the web portal's hold-to-move).
  void disarm() { armed_ = false; }

  // Call between animation frames and once per loop(). Returns true once,
  // when the watched move must stop now. Unsigned subtraction keeps it
  // correct across the millis() rollover (every ~49.7 days).
  bool shouldStop(uint32_t nowMs, bool continuousMoveActive) {
    if (!armed_) return false;
    if (!continuousMoveActive) {  // the move already ended (stop, pose, ...)
      armed_ = false;
      return false;
    }
    if (timeoutMs_ == 0) return false;  // watchdog turned off
    if ((uint32_t)(nowMs - lastMs_) < timeoutMs_) return false;
    armed_ = false;
    return true;
  }

 private:
  uint32_t timeoutMs_;
  uint32_t lastMs_;
  bool armed_;
};
