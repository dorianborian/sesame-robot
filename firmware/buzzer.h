#pragma once

#include <Arduino.h>
#include "esp32-hal-rmt.h"

// GPIO 5 drives the 2N2222 base. RMT is separate from the LEDC timers used
// by the eight servos, avoiding the timer conflict seen with tone()/LEDC.
constexpr uint8_t BUZZER_PIN = 5;
constexpr uint16_t BUZZER_QUEUE_SIZE = 256;

struct BuzzerNote { uint16_t frequency; uint16_t durationMs; uint16_t gapMs; };
enum BuzzerCue : uint8_t {
  BUZZER_CUE_PET, BUZZER_CUE_TURN_LEFT, BUZZER_CUE_TURN_RIGHT,
  BUZZER_CUE_SUPERMAN, BUZZER_CUE_WAVE_START, BUZZER_CUE_WAVE_BEAT,
  BUZZER_CUE_DANCE_BEAT, BUZZER_CUE_SWIM, BUZZER_CUE_POINT,
  BUZZER_CUE_PUSHUP_DOWN, BUZZER_CUE_PUSHUP_UP, BUZZER_CUE_BOW,
  BUZZER_CUE_CUTE, BUZZER_CUE_FREAKY, BUZZER_CUE_WORM,
  BUZZER_CUE_SHAKE, BUZZER_CUE_SHRUG, BUZZER_CUE_DEAD,
  BUZZER_CUE_CRAB, BUZZER_CUE_WIGGLE
};

static BuzzerNote buzzerQueue[BUZZER_QUEUE_SIZE];
static uint16_t buzzerQueueHead = 0, buzzerQueueTail = 0;
static bool buzzerReady = false, buzzerToneActive = false, buzzerGapActive = false;
static unsigned long buzzerDeadlineMs = 0;
static uint16_t buzzerCurrentGapMs = 0;
static rmt_data_t buzzerWave[1];

inline void clearBuzzerQueue() {
  buzzerQueueHead = 0;
  buzzerQueueTail = 0;
  buzzerToneActive = false;
  buzzerGapActive = false;
  rmtWriteLooping(BUZZER_PIN, nullptr, 0);
  digitalWrite(BUZZER_PIN, LOW);
}

inline bool enqueueBuzzerNote(uint16_t frequency, uint16_t durationMs, uint16_t gapMs = 0) {
  const uint16_t nextTail = (buzzerQueueTail + 1) % BUZZER_QUEUE_SIZE;
  if (nextTail == buzzerQueueHead) return false;
  buzzerQueue[buzzerQueueTail] = { frequency, durationMs, gapMs };
  buzzerQueueTail = nextTail;
  return true;
}

inline void startBuzzerTone(uint16_t frequency) {
  if (!buzzerReady || frequency == 0) return;
  const uint16_t halfPeriodUs = (500000UL / frequency) ? (500000UL / frequency) : 1;
  buzzerWave[0].level0 = 1; buzzerWave[0].duration0 = halfPeriodUs;
  buzzerWave[0].level1 = 0; buzzerWave[0].duration1 = halfPeriodUs;
  rmtWriteLooping(BUZZER_PIN, buzzerWave, 1);
  buzzerToneActive = true;
}

inline void stopBuzzerTone() {
  if (!buzzerReady) return;
  rmtWriteLooping(BUZZER_PIN, nullptr, 0);
  digitalWrite(BUZZER_PIN, LOW);
  buzzerToneActive = false;
}

inline void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  buzzerReady = rmtInit(BUZZER_PIN, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, 1000000);
  if (buzzerReady) rmtSetEOT(BUZZER_PIN, LOW);
  Serial.println(buzzerReady ? F("[BUZZER] RMT ready on GPIO 5") : F("[BUZZER] RMT setup failed"));
}

inline void updateBuzzer() {
  if (!buzzerReady) return;
  const unsigned long now = millis();
  if (buzzerToneActive && now >= buzzerDeadlineMs) {
    stopBuzzerTone();
    buzzerGapActive = buzzerCurrentGapMs > 0;
    buzzerDeadlineMs = now + buzzerCurrentGapMs;
  }
  if (buzzerToneActive || (buzzerGapActive && now < buzzerDeadlineMs) || buzzerQueueHead == buzzerQueueTail) return;
  buzzerGapActive = false;
  const BuzzerNote note = buzzerQueue[buzzerQueueHead];
  buzzerQueueHead = (buzzerQueueHead + 1) % BUZZER_QUEUE_SIZE;
  buzzerCurrentGapMs = note.gapMs;
  buzzerDeadlineMs = now + note.durationMs;
  if (note.frequency > 0) {
    startBuzzerTone(note.frequency);
  } else {
    buzzerGapActive = true;
  }
}

inline void playBuzzerCue(BuzzerCue cue) {
  switch (cue) {
    case BUZZER_CUE_PET:          enqueueBuzzerNote(1760, 35, 15); enqueueBuzzerNote(2093, 55); break;
    case BUZZER_CUE_TURN_LEFT:    enqueueBuzzerNote(1480, 45); break;
    case BUZZER_CUE_TURN_RIGHT:   enqueueBuzzerNote(1976, 45); break;
    case BUZZER_CUE_SUPERMAN:     enqueueBuzzerNote(1318, 45, 15); enqueueBuzzerNote(1760, 45, 15); enqueueBuzzerNote(2637, 100); break;
    case BUZZER_CUE_WAVE_START:   enqueueBuzzerNote(1567, 50, 15); enqueueBuzzerNote(2093, 70); break;
    case BUZZER_CUE_WAVE_BEAT:    enqueueBuzzerNote(2349, 28); break;
    case BUZZER_CUE_DANCE_BEAT:   enqueueBuzzerNote(1318, 35); break;
    case BUZZER_CUE_SWIM:         enqueueBuzzerNote(1567, 30); break;
    case BUZZER_CUE_POINT:        enqueueBuzzerNote(1760, 40, 10); enqueueBuzzerNote(2637, 65); break;
    case BUZZER_CUE_PUSHUP_DOWN:  enqueueBuzzerNote(1047, 35); break;
    case BUZZER_CUE_PUSHUP_UP:    enqueueBuzzerNote(1567, 35); break;
    case BUZZER_CUE_BOW:          enqueueBuzzerNote(1318, 55, 12); enqueueBuzzerNote(1567, 55, 12); enqueueBuzzerNote(2093, 100); break;
    case BUZZER_CUE_CUTE:         enqueueBuzzerNote(2093, 32, 12); enqueueBuzzerNote(2637, 48); break;
    case BUZZER_CUE_FREAKY:       enqueueBuzzerNote(784, 45); break;
    case BUZZER_CUE_WORM:         enqueueBuzzerNote(1175, 25); break;
    case BUZZER_CUE_SHAKE:        enqueueBuzzerNote(1967, 25); break;
    case BUZZER_CUE_SHRUG:        enqueueBuzzerNote(988, 45, 18); enqueueBuzzerNote(1318, 55); break;
    case BUZZER_CUE_DEAD:         enqueueBuzzerNote(784, 70); break;
    case BUZZER_CUE_CRAB:         enqueueBuzzerNote(2093, 24); break;
    case BUZZER_CUE_WIGGLE:       enqueueBuzzerNote(1760, 22); break;
  }
}
