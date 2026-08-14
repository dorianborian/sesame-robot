#include "face_anim.h"
#include "../screen/screen.h"
#include "../../face-bitmaps.h"
#include "../../movement-sequences.h"

struct FaceEntry {
  const char* name;
  const unsigned char* const* frames;
  uint8_t maxFrames;
};

struct FaceFpsEntry {
  const char* name;
  uint8_t     fps;
};

static const uint8_t MAX_FACE_FRAMES = 6;

#define MAKE_FACE_FRAMES(name) \
  static const unsigned char* const face_##name##_frames[] = { \
    epd_bitmap_##name,   epd_bitmap_##name##_1, epd_bitmap_##name##_2, \
    epd_bitmap_##name##_3, epd_bitmap_##name##_4, epd_bitmap_##name##_5 \
  };

#define X(name) MAKE_FACE_FRAMES(name)
FACE_LIST
#undef X
#undef MAKE_FACE_FRAMES

static const FaceEntry FACE_REGISTRY[] = {
#define X(name) { #name, face_##name##_frames, MAX_FACE_FRAMES },
  FACE_LIST
#undef X
  { "default", face_defualt_frames, MAX_FACE_FRAMES }
};

static const size_t FACE_REGISTRY_SIZE = sizeof(FACE_REGISTRY) / sizeof(FACE_REGISTRY[0]);

static const FaceFpsEntry FPS_REGISTRY[] = {
  { "walk",           1 }, { "rest",           1 }, { "swim",           1 },
  { "dance",          1 }, { "wave",           1 }, { "point",          5 },
  { "stand",          1 }, { "cute",           1 }, { "pushup",         1 },
  { "freaky",         1 }, { "bow",            1 }, { "worm",           1 },
  { "shake",          1 }, { "shrug",          1 }, { "dead",           2 },
  { "crab",           1 }, { "idle",           1 }, { "idle_blink",     7 },
  { "default",        1 },
  { "happy",          1 }, { "talk_happy",     1 },
  { "sad",            1 }, { "talk_sad",       1 },
  { "angry",          1 }, { "talk_angry",     1 },
  { "surprised",      1 }, { "talk_surprised", 1 },
  { "sleepy",         1 }, { "talk_sleepy",    1 },
  { "love",           1 }, { "talk_love",      1 },
  { "excited",        1 }, { "talk_excited",   1 },
  { "confused",       1 }, { "talk_confused",  1 },
  { "thinking",       1 }, { "talk_thinking",  1 },
};

static const size_t FPS_REGISTRY_SIZE = sizeof(FPS_REGISTRY) / sizeof(FPS_REGISTRY[0]);

static const unsigned char* const* currentFaceFrames = nullptr;
static uint8_t currentFaceFrameCount = 0;
static uint8_t  currentFaceFrameIndex = 0;
static int8_t faceFrameDirection = 1;
static unsigned long lastFaceFrameMs = 0;
static bool faceAnimFinished = false;
static int currentFaceFps = 0;

static FaceAnimMode currentFaceMode = FACE_ANIM_LOOP;

// Exposed for webserver settings
String currentFaceName = "default";
int faceFps = 8;

static bool idleActive = false;
static bool idleBlinkActive = false;
static unsigned long nextIdleBlinkMs = 0;
static uint8_t idleBlinkRepeatsLeft = 0;

String wifiInfoText = "";
static int wifiScrollPos = 0;
static unsigned long lastWifiScrollMs = 0;
static unsigned long lastInputTime = 0;
static bool firstInputReceived = false;
static bool showingWifiInfo = false;

extern void processWebServer();
extern void processDNS();
extern void runStandPose(int face);

static uint8_t countFrames(const unsigned char* const* frames, uint8_t max) {
  if (!frames || !frames[0]) return 0;

  uint8_t n = 0;

  for (uint8_t i = 0; i < max; i++) {
    if (!frames[i]) break;
    n++;
  }

  return n;
}

static int lookupFaceFps(const String& name) {
  for (size_t i = 0; i < FPS_REGISTRY_SIZE; i++) {
    if (name.equalsIgnoreCase(FPS_REGISTRY[i].name)) return FPS_REGISTRY[i].fps;
  }

  return faceFps;
}

static void scheduleNextIdleBlink(unsigned long minMs, unsigned long maxMs) {
  nextIdleBlinkMs = millis() + (unsigned long)random(minMs, maxMs);
}

void initFaceSystem() {
  lastInputTime = millis();
  firstInputReceived = false;
  showingWifiInfo = false;
  setFace("rest");
}

void setFace(const String& name) {
  if (name == currentFaceName && currentFaceFrames != nullptr) return;

  currentFaceName        = name;
  currentFaceFrameIndex  = 0;
  lastFaceFrameMs        = 0;
  faceFrameDirection     = 1;
  faceAnimFinished       = false;
  currentFaceFps         = lookupFaceFps(name);

  // Default fallback
  currentFaceFrames     = face_defualt_frames;
  currentFaceFrameCount = countFrames(face_defualt_frames, MAX_FACE_FRAMES);

  for (size_t i = 0; i < FACE_REGISTRY_SIZE; i++) {
    if (name.equalsIgnoreCase(FACE_REGISTRY[i].name)) {
      currentFaceFrames     = FACE_REGISTRY[i].frames;
      currentFaceFrameCount = countFrames(FACE_REGISTRY[i].frames, FACE_REGISTRY[i].maxFrames);
      break;
    }
  }

  // Fallback if still empty
  if (currentFaceFrameCount == 0) {
    currentFaceFrames     = face_defualt_frames;
    currentFaceFrameCount = countFrames(face_defualt_frames, MAX_FACE_FRAMES);
    currentFaceName       = "default";
    currentFaceFps        = lookupFaceFps("default");
  }

  if (currentFaceFrameCount > 0 && currentFaceFrames[0]) {
    updateFaceBitmap(currentFaceFrames[0]);
  }
}

void setFaceMode(FaceAnimMode mode) {
  currentFaceMode    = mode;
  faceFrameDirection = 1;
  faceAnimFinished   = false;
}

void setFaceWithMode(const String& name, FaceAnimMode mode) {
  setFaceMode(mode);
  setFace(name);
}

void updateFaceAnimation() {
  if (!currentFaceFrames || currentFaceFrameCount <= 1) return;
  if (currentFaceMode == FACE_ANIM_ONCE && faceAnimFinished) return;

  unsigned long now      = millis();
  int           fps      = max(1, currentFaceFps > 0 ? currentFaceFps : faceFps);
  unsigned long interval = 1000UL / fps;

  if (now - lastFaceFrameMs < interval) return;
  lastFaceFrameMs = now;

  switch (currentFaceMode) {
    case FACE_ANIM_LOOP:
      currentFaceFrameIndex = (currentFaceFrameIndex + 1) % currentFaceFrameCount;
      break;

    case FACE_ANIM_ONCE:
      if (currentFaceFrameIndex + 1 >= currentFaceFrameCount) {
        currentFaceFrameIndex = currentFaceFrameCount - 1;
        faceAnimFinished = true;
      } else {
        currentFaceFrameIndex++;
      }
      break;

    case FACE_ANIM_BOOMERANG:
      if (faceFrameDirection > 0) {
        if (currentFaceFrameIndex + 1 >= currentFaceFrameCount) {
          faceFrameDirection = -1;
          if (currentFaceFrameIndex > 0) currentFaceFrameIndex--;
        } else {
          currentFaceFrameIndex++;
        }
      } else {
        if (currentFaceFrameIndex == 0) {
          faceFrameDirection = 1;
          if (currentFaceFrameCount > 1) currentFaceFrameIndex++;
        } else {
          currentFaceFrameIndex--;
        }
      }
      break;
  }

  updateFaceBitmap(currentFaceFrames[currentFaceFrameIndex]);
}

void enterIdle() {
  idleActive          = true;
  idleBlinkActive     = false;
  idleBlinkRepeatsLeft = 0;
  setFaceWithMode("idle", FACE_ANIM_BOOMERANG);
  scheduleNextIdleBlink(3000, 7000);
}

void exitIdle() {
  idleActive      = false;
  idleBlinkActive = false;
}

void updateIdleBlink() {
  if (!idleActive) return;

  if (!idleBlinkActive) {
    if (millis() >= nextIdleBlinkMs) {
      idleBlinkActive = true;
      if (idleBlinkRepeatsLeft == 0 && random(0, 100) < 30) {
        idleBlinkRepeatsLeft = 1;  // occasional double-blink
      }
      setFaceWithMode("idle_blink", FACE_ANIM_ONCE);
    }
    return;
  }

  if (currentFaceMode == FACE_ANIM_ONCE && faceAnimFinished) { idleBlinkActive = false;
    setFaceWithMode("idle", FACE_ANIM_BOOMERANG);
    if (idleBlinkRepeatsLeft > 0) {
      idleBlinkRepeatsLeft--;
      scheduleNextIdleBlink(120, 220);
    } else {
      scheduleNextIdleBlink(3000, 7000);
    }
  }
}

void delayWithFace(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    updateFaceAnimation();
    processWebServer();
    processDNS();
    delay(5);
  }
}

bool pressingCheck(String cmd, int ms) {
  extern String currentCommand;
  unsigned long start = millis();
  while (millis() - start < (unsigned long)ms) {
    processWebServer();
    processDNS();
    updateFaceAnimation();
    if (currentCommand != cmd) {
      runStandPose(1);
      return false;
    }
    yield();
  }
  return true;
}

void recordInput() {
  lastInputTime = millis();
  if (!firstInputReceived) {
    firstInputReceived = true;
    showingWifiInfo    = false;
  }
}

void updateWifiInfoScroll() {
  // Stop scrolling once the user has interacted
  if (firstInputReceived) {
    if (showingWifiInfo) {
      showingWifiInfo = false;
      // Restore current face frame
      if (currentFaceFrames && currentFaceFrameCount > 0) {
        updateFaceBitmap(currentFaceFrames[currentFaceFrameIndex]);
      }
    }
    return;
  }

  unsigned long now = millis();

  // Start scrolling after 30 s idle
  if (!showingWifiInfo && (now - lastInputTime >= 30000)) {
    showingWifiInfo = true;
    wifiScrollPos   = 0;
    lastWifiScrollMs = now;
  }

  if (!showingWifiInfo) return;

  // Advance scroll every 150 ms
  if (now - lastWifiScrollMs < 150) return;
  lastWifiScrollMs = now;

  const unsigned char* faceBitmap = (currentFaceFrames && currentFaceFrameCount > 0)
                                      ? currentFaceFrames[currentFaceFrameIndex]
                                      : nullptr;

  drawWifiInfoScroll(wifiInfoText, wifiScrollPos, faceBitmap);

  wifiScrollPos += 2;
  if (wifiScrollPos >= (int)(wifiInfoText.length() * 6)) {
    wifiScrollPos = 0;
  }
}
