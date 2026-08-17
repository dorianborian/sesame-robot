#pragma once

#include <Arduino.h>

// ======================================================================
// --- WEB INTERFACE HTML ---
// ======================================================================

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Sesame Access Point Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    :root {
      --content-color: #ff8c42;
      --content-color-dark: #e67a30;
      --content-color-darker: #cc6b29;
      --content-color-glow: rgba(255, 140, 66, 0.3);
    }

    * {
      user-select: none;
      -webkit-user-select: none;
      -webkit-touch-callout: none;
    }

    body {
      font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
      text-align: center;
      background: linear-gradient(135deg, #0a0a0a, #1a1a2e);
      color: #e0e0e0;
      touch-action: manipulation;
      margin: 0;
      padding: 10px;
      overflow-x: hidden;
      box-sizing: border-box;
    }

    h2 {
      margin: 10px 0 20px 0;
      color: #fff;
      font-size: 32px;
      font-weight: 600;
      text-shadow: 0 2px 4px rgba(0,0,0,0.5);
    }

    /* Command Queue Status */
    .command-queue {
      font-size: 12px;
      color: #888;
      margin-bottom: 20px;
    }
    .command-queue.full {
      color: #ff6b6b;
      font-weight: bold;
    }

    /* Section Containers */
    .sections-container {
      display: flex;
      flex-direction: column;
      gap: 15px;
      max-width: 1400px;
      margin: 0 auto;
    }

    .section {
      background: rgba(30, 30, 30, 0.8);
      border: 1px solid #333;
      border-radius: 16px;
      padding: 15px;
      margin: 0 auto;
      width: calc(100% - 20px);
      max-width: 450px;
      box-shadow: 0 4px 20px rgba(0,0,0,0.3);
      box-sizing: border-box;
    }

    .section-title {
      font-size: 16px;
      font-weight: 600;
      color: var(--content-color);
      margin: 0 0 15px 0;
      text-transform: uppercase;
      letter-spacing: 1px;
    }

    /* Button Base Styles */
    button {
      background: linear-gradient(145deg, #3a3a3a, #2a2a2a);
      border: none;
      color: #e0e0e0;
      padding: 15px;
      font-size: 18px;
      border-radius: 12px;
      cursor: pointer;
      box-shadow: 0 4px 8px rgba(0,0,0,0.3);
      transition: all 0.1s;
      font-weight: 500;
    }
    button:active {
      box-shadow: 0 2px 4px rgba(0,0,0,0.3);
      transform: translateY(2px);
    }

    /* D-Pad Controls */
    .dpad-container {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 15px;
      width: 100%;
    }
    .dpad {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(2, 1fr);
      gap: 12px;
      width: 100%;
      max-width: 294px;
      aspect-ratio: 3 / 2;
    }
    .dpad button {
      font-size: 35px;
      border: 2px solid #555;
      color: #fff;
      width: 100%;
      height: 100%;
      min-height: 70px;
    }
    .spacer {
      visibility: hidden;
    }

    /* Pose Grid */
    .grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 10px;
    }
    .btn-pose {
      background: linear-gradient(145deg, var(--content-color), var(--content-color-dark));
      padding: 12px 8px;
      font-size: 15px;
    }
    .btn-pose:active {
      background: linear-gradient(145deg, var(--content-color-dark), var(--content-color-darker));
    }

    /* Special Buttons */
    .btn-stop-all {
      background: linear-gradient(145deg, #e63946, #c92a35);
      width: 100%;
      font-size: 20px;
      padding: 18px;
      box-shadow: 0 6px 12px rgba(230, 57, 70, 0.4);
      border: 2px solid #ff6b6b;
      color: #fff;
      text-transform: uppercase;
      letter-spacing: 2px;
    }
    .btn-stop-all:active {
      background: linear-gradient(145deg, #c92a35, #a8222c);
      transform: translateY(3px);
    }

    .btn-settings {
      background: linear-gradient(145deg, #555, #444);
      padding: 12px 25px;
      font-size: 16px;
    }

    /* Motor Controls */
    .lock-indicator {
      font-size: 11px;
      color: #ff6b6b;
      text-align: center;
      margin-top: 5px;
      display: none;
    }
    .lock-indicator.active {
      display: block;
    }

    .motor-controls {
      margin-top: 10px;
    }
    .motor-slider {
      margin: 15px 0;
    }
    .motor-slider label {
      display: flex;
      justify-content: space-between;
      font-size: 12px;
      color: #aaa;
      margin-bottom: 5px;
    }
    .motor-slider input[type="range"] {
      width: 100%;
      height: 6px;
      background: #333;
      border-radius: 5px;
      outline: none;
      -webkit-appearance: none;
    }
    .motor-slider input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 18px;
      height: 18px;
      background: var(--content-color);
      border-radius: 50%;
      cursor: pointer;
      box-shadow: 0 2px 6px var(--content-color-glow);
    }
    .motor-slider input[type="range"]::-moz-range-thumb {
      width: 18px;
      height: 18px;
      background: var(--content-color);
      border-radius: 50%;
      cursor: pointer;
      border: none;
      box-shadow: 0 2px 6px var(--content-color-glow);
    }
    .motor-slider input[type="range"]:disabled {
      opacity: 0.5;
      cursor: not-allowed;
    }
    .motor-slider input[type="range"]:disabled::-webkit-slider-thumb,
    .motor-slider input[type="range"]:disabled::-moz-range-thumb {
      background: #666;
      cursor: not-allowed;
    }

    /* Gamepad Status */
    .gamepad-status {
      font-size: 13px;
      padding: 8px 14px;
      border-radius: 10px;
      border: 2px solid #666;
      color: #ccc;
      background: rgba(26, 26, 26, 0.8);
      display: inline-block;
    }
    .gamepad-status.connected {
      border-color: #2ecc71;
      color: #2ecc71;
      background: rgba(46, 204, 113, 0.1);
    }

    /* Settings Panel */
    .settings-panel {
      display: none;
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(0,0,0,0.9);
      z-index: 100;
      backdrop-filter: blur(8px);
      overflow-y: auto;
    }
    .settings-content {
      background: linear-gradient(145deg, #1e1e1e, #2a2a2a);
      border: 1px solid #444;
      max-width: 400px;
      margin: 30px auto;
      padding: 25px;
      border-radius: 20px;
      text-align: left;
      box-shadow: 0 10px 40px rgba(0,0,0,0.6);
    }
    .settings-content h3 {
      color: var(--content-color);
      margin-top: 0;
      text-align: center;
      font-size: 24px;
    }
    .settings-section {
      margin: 20px 0;
      padding: 15px;
      background: rgba(0,0,0,0.3);
      border-radius: 10px;
    }
    .settings-section h4 {
      color: var(--content-color);
      margin: 0 0 10px 0;
      font-size: 14px;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    .settings-content label {
      display: block;
      margin-top: 12px;
      font-weight: 500;
      color: #ccc;
      font-size: 13px;
    }
    .settings-content input,
    .settings-content select {
      width: 100%;
      padding: 10px;
      margin-top: 5px;
      background: #333;
      color: #fff;
      border: 1px solid #555;
      border-radius: 8px;
      box-sizing: border-box;
      font-size: 14px;
    }
    .btn-save {
      background: linear-gradient(145deg, #2ecc71, #27ae60);
      box-shadow: 0 4px 8px rgba(46, 204, 113, 0.3);
      width: 100%;
      margin-top: 25px;
      color: #fff;
    }
    .btn-close {
      background: linear-gradient(145deg, #e74c3c, #c0392b);
      box-shadow: 0 4px 8px rgba(231, 76, 60, 0.3);
      width: 100%;
      margin-top: 12px;
      color: #fff;
    }

    /* Desktop Layout */
    @media (min-width: 1024px) {
      body {
        padding: 15px;
      }
      .section {
        padding: 20px;
        width: 100%;
      }
      h2 {
        margin-bottom: 30px;
      }
      .command-queue {
        margin-bottom: 30px;
      }
      .sections-container {
        flex-direction: row;
        justify-content: center;
        align-items: flex-start;
        gap: 20px;
        padding: 0 20px;
      }
      .section-column {
        flex: 0 1 450px;
        display: flex;
        flex-direction: column;
        gap: 20px;
      }
      .section {
        width: 100%;
        max-width: 450px;
        margin: 0;
      }
    }

    /* Melody Generator Styles */
    .melody-controls {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
      margin-bottom: 15px;
    }
    .melody-controls select, .melody-controls input {
      width: 100%;
      padding: 8px;
      background: #333;
      color: #fff;
      border: 1px solid #555;
      border-radius: 6px;
      box-sizing: border-box;
      font-size: 13px;
    }
    .melody-controls label {
      display: block;
      font-size: 11px;
      color: #aaa;
      margin-bottom: 3px;
    }
    .melody-viz {
      height: 60px;
      background: #222;
      border-radius: 8px;
      border: 1px solid #444;
      margin-bottom: 15px;
      position: relative;
      overflow: hidden;
      display: flex;
      align-items: flex-end;
      padding: 5px;
      gap: 2px;
    }
    .melody-bar {
      flex: 1;
      background: var(--content-color);
      border-radius: 2px;
      opacity: 0.8;
    }
    .btn-melody {
      background: linear-gradient(145deg, #2ecc71, #27ae60);
      padding: 12px;
      font-size: 14px;
      color: #fff;
    }
    .btn-melody-generate {
      background: linear-gradient(145deg, #3498db, #2980b9);
      grid-column: span 2;
    }
    .sheet-music-textarea {
      width: 100%;
      height: 80px;
      background: #222;
      color: #fff;
      border: 1px solid #444;
      border-radius: 6px;
      padding: 8px;
      font-family: monospace;
    }
  </style>
</head>
<body>
  <h2 id="robotTitle">Loading...</h2>
  <div class="command-queue" id="queueStatus">Command Queue: 0/3</div>

  <div class="sections-container">
    <div class="section-column">
      <!-- Custom Sheet Music Section -->
      <div class="section">
        <div class="section-title">Sheet Music</div>
        <div class="melody-controls" style="grid-template-columns: 1fr;">
          <select id="savedMelodies" onchange="loadSavedMelody()"></select>
        </div>
        <textarea id="sheetMusicInput" class="sheet-music-textarea" placeholder="e.g. G4 G4 A4 G4 C5 B4-"></textarea>
        <div style="font-size:10px;color:#888;margin:5px 0 10px 0;">Format: NoteOctave. '-' for double duration (e.g., C4-)</div>
        <div style="display:flex;gap:10px;">
          <button class="btn-melody" style="flex:1" onclick="playSheetMusic()">Play</button>
          <button class="btn-melody" style="flex:1;background:linear-gradient(145deg, #f39c12, #d35400);" onclick="saveSheetMusic()">Save</button>
        </div>
        <div style="margin-top:10px;">
          <button class="btn-melody" style="width:100%;background:linear-gradient(145deg, #c0392b, #a93226);" onclick="deleteSheetMusic()">Delete</button>
        </div>
      </div>

      <!-- Movement Control Section -->
      <div class="section">
    <div class="section-title">Movement Control</div>
    <div class="dpad-container">
      <div class="dpad">
        <div class="spacer"></div>
        <button onmousedown="move('forward')" onmouseup="stop()" ontouchstart="move('forward')" ontouchend="stop()">&#9650;</button>
        <div class="spacer"></div>

        <button onmousedown="move('left')" onmouseup="stop()" ontouchstart="move('left')" ontouchend="stop()">&#9664;</button>
        <button onmousedown="move('backward')" onmouseup="stop()" ontouchstart="move('backward')" ontouchend="stop()">&#9660;</button>
        <button onmousedown="move('right')" onmouseup="stop()" ontouchstart="move('right')" ontouchend="stop()">&#9654;</button>
      </div>
      <button class="btn-stop-all" onclick="stop()">STOP ALL</button>
    </div>
  </div>

      <!-- Poses & Animations Section -->
      <div class="section">
        <div class="section-title">Poses & Animations</div>
        <div class="grid">
          <button class="btn-pose" onclick="pose('rest')">Rest</button>
          <button class="btn-pose" onclick="pose('stand')">Stand</button>
          <button class="btn-pose" onclick="pose('wave')">Wave</button>
          <button class="btn-pose" onclick="pose('dance')">Dance</button>
          <button class="btn-pose" onclick="pose('swim')">Swim</button>
          <button class="btn-pose" onclick="pose('point')">Point</button>
          <button class="btn-pose" onclick="pose('pushup')">Pushup</button>
          <button class="btn-pose" onclick="pose('bow')">Bow</button>
          <button class="btn-pose" onclick="pose('cute')">Cute</button>
          <button class="btn-pose" onclick="pose('freaky')">Freaky</button>
          <button class="btn-pose" onclick="pose('worm')">Worm</button>
          <button class="btn-pose" onclick="pose('shake')">Shake</button>
          <button class="btn-pose" onclick="pose('shrug')">Shrug</button>
          <button class="btn-pose" onclick="pose('dead')">Dead</button>
          <button class="btn-pose" onclick="pose('crab')">Crab</button>
          <button class="btn-pose" onclick="pose('pissleft')">Piss L</button>
          <button class="btn-pose" onclick="pose('pissright')">Piss R</button>
        </div>
      </div>
    </div>

    <div class="section-column">
      <!-- Melody Generator Section -->
      <div class="section">
        <div class="section-title">Melody Generator</div>

        <div class="melody-controls">
          <div>
            <label>Mood</label>
            <select id="melodyMood">
              <option value="cheerful">Cheerful</option>
              <option value="mysterious">Mysterious</option>
              <option value="energetic">Energetic</option>
              <option value="calm">Calm</option>
              <option value="playful">Playful</option>
            </select>
          </div>
          <div>
            <label>Length</label>
            <select id="melodyLength">
              <option value="short">Short</option>
              <option value="medium" selected>Medium</option>
              <option value="long">Long</option>
            </select>
          </div>
          <div>
            <label>BPM</label>
            <input type="number" id="melodyBpm" min="60" max="240" value="120" step="10">
          </div>
          <div>
            <label>Complexity</label>
            <input type="range" id="melodyComplexity" min="1" max="5" value="3" style="width:100%;margin-top:8px;">
          </div>
          <button class="btn-melody btn-melody-generate" onclick="generateMelody()">Generate & Play</button>
        </div>

        <div class="melody-viz" id="melodyViz"></div>

        <div style="display:flex;gap:10px;">
          <button class="btn-melody" style="flex:1" onclick="playCurrentMelody()">Play Again</button>
          <button class="btn-melody" style="flex:1;background:linear-gradient(145deg, #e74c3c, #c0392b);" onclick="stopBuzzer()">Stop Audio</button>
        </div>
      </div>

      <!-- Settings & Status Section -->
      <div class="section">
        <div class="section-title">System</div>
        <button class="btn-settings" onclick="openSettings()">Settings</button>
        <div style="margin-top: 15px;">
          <div id="gamepadStatus" class="gamepad-status">Gamepad disconnected</div>
        </div>
      </div>
    </div>
  </div>

  <div id="settingsPanel" class="settings-panel">
    <div class="settings-content">
      <h3>Settings</h3>

      <div class="settings-section">
        <h4>Animation Parameters</h4>
        <label>Frame Delay (ms):</label>
        <input type="number" id="frameDelay" min="1" max="1000" step="1">
        <label>Walk Cycles:</label>
        <input type="number" id="walkCycles" min="1" max="50" step="1">
      </div>

      <div class="settings-section">
        <h4>Motor Settings</h4>
        <label>Motor Current Delay (ms):</label>
        <input type="number" id="motorCurrentDelay" min="0" max="500" step="1">
        <label>Motor Speed:</label>
        <select id="motorSpeed">
          <option value="slow">Slow</option>
          <option value="medium" selected>Medium</option>
          <option value="fast">Fast</option>
        </select>
      </div>

      <div class="settings-section">
        <h4>Theme</h4>
        <label>Accent Color:</label>
        <select id="themeColor">
          <option value="#ff8c42">Orange (Default)</option>
          <option value="#66d9ef">Cyan</option>
          <option value="#a8dadc">Light Blue</option>
          <option value="#2ecc71">Green</option>
          <option value="#e74c3c">Red</option>
          <option value="#9b59b6">Purple</option>
          <option value="#f39c12">Yellow</option>
          <option value="#e91e63">Pink</option>
          <option value="custom">Custom</option>
        </select>
        <input type="color" id="customColor" value="#ff8c42" style="margin-top: 10px; display: none;">
      </div>

      <div class="settings-section">
        <h4>Device Settings</h4>
        <label>Device Name (mDNS):</label>
        <input type="text" id="deviceName" placeholder="sesame-robot" maxlength="32">
        <div style="font-size:11px;color:#666;margin-top:4px;">Allowed: a-z, 0-9, - (spaces/Polish chars auto-corrected)</div>
        <button onclick="saveDeviceName()" style="width:100%;padding:10px;margin-top:10px;background:#3498db;color:#fff;border:none;cursor:pointer;border-radius:8px;font-size:13px;">Save Name</button>
        <div id="deviceNameStatus" style="margin-top:8px;font-size:12px;color:#888;"></div>

        <label style="margin-top:20px;">AP Password:</label>
        <div style="display:flex;gap:5px;">
          <input type="password" id="apPass" placeholder="Min 8 characters" minlength="8" style="flex:1;">
          <button onclick="toggleApPass()" style="padding:0 12px;background:#444;color:#fff;border:1px solid #555;cursor:pointer;">Show</button>
        </div>
        <button onclick="saveApPassword()" style="width:100%;padding:10px;margin-top:10px;background:#9b59b6;color:#fff;border:none;cursor:pointer;border-radius:8px;font-size:13px;">Save AP Password</button>
        <div id="apPassStatus" style="margin-top:8px;font-size:12px;color:#888;"></div>
      </div>

      <div class="settings-section">
        <h4>WiFi Settings</h4>
        <label>Network Name (SSID):</label>
        <button onclick="scanWiFi()" style="width:100%;padding:10px;margin-bottom:8px;background:#444;color:#fff;border:1px solid #555;cursor:pointer;font-size:13px;">Scan Networks</button>
        <select id="wifiSSID" style="margin-bottom:8px;"></select>
        <label>Password:</label>
        <div style="display:flex;gap:5px;">
          <input type="password" id="wifiPass" placeholder="Network password" style="flex:1;">
          <button onclick="toggleWiFiPass()" style="padding:0 12px;background:#444;color:#fff;border:1px solid #555;cursor:pointer;">Show</button>
        </div>
        <button onclick="connectWiFi()" style="width:100%;padding:12px;margin-top:12px;background:#00b894;color:#fff;border:none;cursor:pointer;font-weight:bold;border-radius:8px;">Connect & Restart</button>
        <button onclick="resetWiFi()" style="width:100%;padding:10px;margin-top:8px;background:#e74c3c;color:#fff;border:none;cursor:pointer;border-radius:8px;font-size:13px;">Reset WiFi</button>
        <div id="wifiStatus" style="margin-top:10px;font-size:12px;color:#888;"></div>
      </div>

      <button class="btn-settings" style="width: 100%; margin-top: 20px;" onclick="openMotorControl()">Manual Motor Control</button>

      <button class="btn-save" onclick="saveSettings()">Save Settings</button>
      <button class="btn-close" onclick="closeSettings()">Close</button>
    </div>
  </div>

  <div id="motorControlPanel" class="settings-panel">
    <div class="settings-content">
      <h3>Manual Motor Control</h3>
      <div class="lock-indicator" id="lockIndicator">Locked during animations</div>

      <div class="settings-section">
        <div class="motor-controls">
          <div class="motor-slider">
            <label><span>S0 R1</span> <span id="m1val">90&deg;</span></label>
            <input type="range" id="motor1" min="0" max="180" value="90" oninput="updateMotor(1, this.value)">
          </div>
          <div class="motor-slider">
            <label><span>S1 R2</span> <span id="m2val">90&deg;</span></label>
            <input type="range" id="motor2" min="0" max="180" value="90" oninput="updateMotor(2, this.value)">
          </div>
          <div class="motor-slider">
            <label><span>S2 L1</span> <span id="m3val">90&deg;</span></label>
            <input type="range" id="motor3" min="0" max="180" value="90" oninput="updateMotor(3, this.value)">
          </div>
          <div class="motor-slider">
            <label><span>S3 L2</span> <span id="m4val">90&deg;</span></label>
            <input type="range" id="motor4" min="0" max="180" value="90" oninput="updateMotor(4, this.value)">
          </div>
          <div class="motor-slider">
            <label><span>S4 R4</span> <span id="m5val">90&deg;</span></label>
            <input type="range" id="motor5" min="0" max="180" value="90" oninput="updateMotor(5, this.value)">
          </div>
          <div class="motor-slider">
            <label><span>S5 R3</span> <span id="m6val">90&deg;</span></label>
            <input type="range" id="motor6" min="0" max="180" value="90" oninput="updateMotor(6, this.value)">
          </div>
          <div class="motor-slider">
            <label><span>S6 L3</span> <span id="m7val">90&deg;</span></label>
            <input type="range" id="motor7" min="0" max="180" value="90" oninput="updateMotor(7, this.value)">
          </div>
          <div class="motor-slider">
            <label><span>S7 L4</span> <span id="m8val">90&deg;</span></label>
            <input type="range" id="motor8" min="0" max="180" value="90" oninput="updateMotor(8, this.value)">
          </div>
        </div>
      </div>

      <button class="btn-close" onclick="closeMotorControl()">Close</button>
    </div>
  </div>

<script>
// Command queue management - max 3 commands
let commandQueue = 0;
const MAX_COMMANDS = 3;
let motorsLocked = false;

// --- Melody Generator Logic ---
let currentMelodySequence = [];

const scales = {
  major: [0, 2, 4, 5, 7, 9, 11],
  minor: [0, 2, 3, 5, 7, 8, 10],
  pentatonicMajor: [0, 2, 4, 7, 9],
  pentatonicMinor: [0, 3, 5, 7, 10],
  dorian: [0, 2, 3, 5, 7, 9, 10]
};

const moods = {
  cheerful: { scale: 'major', octaves: [5, 6], rootMin: 0, rootMax: 11, baseRestProb: 0.1, jumpProb: 0.3 },
  mysterious: { scale: 'dorian', octaves: [4, 5], rootMin: 0, rootMax: 11, baseRestProb: 0.3, jumpProb: 0.4 },
  energetic: { scale: 'pentatonicMajor', octaves: [5, 6, 7], rootMin: 0, rootMax: 11, baseRestProb: 0.05, jumpProb: 0.6 },
  calm: { scale: 'pentatonicMinor', octaves: [4, 5], rootMin: 0, rootMax: 11, baseRestProb: 0.2, jumpProb: 0.1 },
  playful: { scale: 'major', octaves: [5, 6, 7], rootMin: 0, rootMax: 11, baseRestProb: 0.15, jumpProb: 0.7 }
};

// Helper: Convert MIDI note to frequency
function midiToFreq(m) {
  return Math.round(440 * Math.pow(2, (m - 69) / 12));
}

function generateMelody() {
  const moodName = document.getElementById('melodyMood').value;
  const lengthName = document.getElementById('melodyLength').value;
  const bpm = parseInt(document.getElementById('melodyBpm').value) || 120;
  const complexity = parseInt(document.getElementById('melodyComplexity').value) || 3;

  const mood = moods[moodName];
  const scaleDef = scales[mood.scale];

  // Basic length in quarter notes
  let numBeats = lengthName === 'short' ? 4 : (lengthName === 'long' ? 16 : 8);

  // Root note (random)
  const rootNote = Math.floor(Math.random() * (mood.rootMax - mood.rootMin + 1)) + mood.rootMin;

  const msPerBeat = Math.round(60000 / bpm);
  const possibleDurations = [msPerBeat, msPerBeat / 2, msPerBeat / 4]; // Quarter, Eighth, Sixteenth
  if (complexity < 3) possibleDurations.pop(); // Remove 16th notes for simple
  if (complexity > 4) possibleDurations.push(msPerBeat * 1.5); // Dotted quarter

  currentMelodySequence = [];
  let currentBeat = 0;
  let lastDegree = Math.floor(Math.random() * scaleDef.length);

  // We'll generate a motif of 2-4 beats, then repeat or vary it
  const motifLengthBeats = Math.random() > 0.5 ? 2 : 4;
  let motif = [];

  // Generate melody
  while (currentBeat < numBeats) {
    // Decide if we should copy the motif
    if (currentBeat > 0 && currentBeat % motifLengthBeats === 0 && Math.random() < 0.6) {
       // repeat motif
       motif.forEach(n => {
         if (currentBeat < numBeats) {
           currentMelodySequence.push({...n});
           currentBeat += n.beats;
         }
       });
       continue;
    }

    // Generate new note
    // Duration
    let durationMs = possibleDurations[Math.floor(Math.random() * possibleDurations.length)];
    let beats = durationMs / msPerBeat;

    // Ensure we don't go over numBeats
    if (currentBeat + beats > numBeats) {
      beats = numBeats - currentBeat;
      durationMs = beats * msPerBeat;
    }

    // Rest probability
    let isRest = Math.random() < (mood.baseRestProb * (6 - complexity) / 3);

    let f = 0;
    if (!isRest) {
      // Note selection
      let degreeStep = 0;
      if (Math.random() < mood.jumpProb) {
        degreeStep = Math.floor(Math.random() * 5) - 2; // -2 to 2
      } else {
        degreeStep = Math.random() > 0.5 ? 1 : -1;
      }
      lastDegree = (lastDegree + degreeStep + scaleDef.length) % scaleDef.length;

      let octave = mood.octaves[Math.floor(Math.random() * mood.octaves.length)];
      let midiNote = (octave + 1) * 12 + rootNote + scaleDef[lastDegree];
      f = midiToFreq(midiNote);
    }

    let noteObj = {
      f: f,
      d: Math.round(durationMs * 0.8), // 80% duty cycle
      g: Math.round(durationMs * 0.2), // 20% gap
      beats: beats
    };

    currentMelodySequence.push(noteObj);

    if (currentBeat < motifLengthBeats) {
      motif.push(noteObj);
    }

    currentBeat += beats;
  }

  renderMelodyViz();
  playCurrentMelody();
}

function renderMelodyViz() {
  const viz = document.getElementById('melodyViz');
  viz.innerHTML = '';

  let maxFreq = 0;
  let minFreq = 10000;

  currentMelodySequence.forEach(n => {
    if (n.f > 0) {
      if (n.f > maxFreq) maxFreq = n.f;
      if (n.f < minFreq) minFreq = n.f;
    }
  });

  currentMelodySequence.forEach(n => {
    const bar = document.createElement('div');
    bar.className = 'melody-bar';
    bar.style.flex = n.beats;
    if (n.f === 0) {
      bar.style.height = '0%';
      bar.style.background = 'transparent';
    } else {
      const heightPct = 20 + ((n.f - minFreq) / Math.max(1, (maxFreq - minFreq))) * 80;
      bar.style.height = heightPct + '%';
    }
    viz.appendChild(bar);
  });
}

function playCurrentMelody() {
  if (!currentMelodySequence || currentMelodySequence.length === 0) return;

  // Convert to ESP32 API format
  const payload = currentMelodySequence.map(n => ({f: n.f, d: n.d, g: n.g}));

  fetch('/api/playMelody', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(payload)
  }).catch(console.error);
}

function stopBuzzer() {
  fetch('/api/playMelody', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify([{f:0, d:0, g:0}])
  }).catch(console.error);
}

// --- Sheet Music Logic ---
const NOTE_MAP = {
  'C': 0, 'C#': 1, 'DB': 1, 'D': 2, 'D#': 3, 'EB': 3, 'E': 4,
  'F': 5, 'F#': 6, 'GB': 6, 'G': 7, 'G#': 8, 'AB': 8, 'A': 9,
  'A#': 10, 'BB': 10, 'B': 11
};

function parseSheetMusic(text) {
  const tokens = text.toUpperCase().trim().split(/\s+/);
  const sequence = [];
  // Default 120 BPM, quarter note
  const baseDuration = 500;

  for (let token of tokens) {
    if (!token) continue;

    // Parse duration modifiers (e.g. C4- is a half note, C4-- is whole note, C4_ is eighth note)
    let durationMultiplier = 1.0;
    if (token.includes('-')) {
      durationMultiplier = token.split('-').length; // C4- -> 2x, C4-- -> 3x
      token = token.replace(/-/g, '');
    } else if (token.includes('_')) {
      durationMultiplier = 1.0 / (token.split('_').length); // C4_ -> 0.5x
      token = token.replace(/_/g, '');
    }

    // Rest
    if (token === 'R' || token === 'REST') {
      sequence.push({f: 0, d: baseDuration * durationMultiplier, g: 0});
      continue;
    }

    // Extract note and octave
    const match = token.match(/^([A-G][#B]?)([0-9])$/);
    if (match) {
      const noteName = match[1];
      const octave = parseInt(match[2]);

      if (NOTE_MAP[noteName] !== undefined) {
        const midiNote = (octave + 1) * 12 + NOTE_MAP[noteName];
        const freq = midiToFreq(midiNote);
        const totalDur = baseDuration * durationMultiplier;
        sequence.push({
          f: freq,
          d: Math.round(totalDur * 0.8),
          g: Math.round(totalDur * 0.2)
        });
      }
    }
  }
  return sequence;
}

function playSheetMusic() {
  const text = document.getElementById('sheetMusicInput').value;
  currentMelodySequence = parseSheetMusic(text);
  playCurrentMelody();
}

const DEFAULT_MELODIES = {
  "Happy Birthday": "G4_ G4_ A4 G4 C5 B4- G4_ G4_ A4 G4 D5 C5- G4_ G4_ G5 E5 C5 B4 A4- F5_ F5_ E5 C5 D5 C5-"
};

function updateMelodyDropdown() {
  const select = document.getElementById('savedMelodies');
  select.innerHTML = '<option value="">-- Select a Melody --</option>';

  const saved = JSON.parse(localStorage.getItem('sesameMelodies') || '{}');
  const allMelodies = { ...DEFAULT_MELODIES, ...saved };

  for (let name in allMelodies) {
    const opt = document.createElement('option');
    opt.value = name;
    opt.textContent = name;
    select.appendChild(opt);
  }
}

function loadSavedMelody() {
  const select = document.getElementById('savedMelodies');
  if (!select.value) return;

  const saved = JSON.parse(localStorage.getItem('sesameMelodies') || '{}');
  const allMelodies = { ...DEFAULT_MELODIES, ...saved };

  document.getElementById('sheetMusicInput').value = allMelodies[select.value] || "";
}

function saveSheetMusic() {
  const name = prompt("Enter a name for this melody:");
  if (!name) return;

  const text = document.getElementById('sheetMusicInput').value.trim();
  if (!text) {
    alert("Sheet music is empty!");
    return;
  }

  const saved = JSON.parse(localStorage.getItem('sesameMelodies') || '{}');
  saved[name] = text;
  localStorage.setItem('sesameMelodies', JSON.stringify(saved));
  updateMelodyDropdown();
  document.getElementById('savedMelodies').value = name;
}

function deleteSheetMusic() {
  const name = document.getElementById('savedMelodies').value;
  if (!name) { alert("Select a melody to delete."); return; }
  if (DEFAULT_MELODIES[name]) { alert("Cannot delete default melodies."); return; }

  if (confirm(`Delete "${name}"?`)) {
    const saved = JSON.parse(localStorage.getItem('sesameMelodies') || '{}');
    delete saved[name];
    localStorage.setItem('sesameMelodies', JSON.stringify(saved));
    updateMelodyDropdown();
    document.getElementById('sheetMusicInput').value = '';
  }
}

function toggleWiFiPass() {
  const passInput = document.getElementById('wifiPass');
  const btn = event.target;
  if (passInput.type === 'password') {
    passInput.type = 'text';
    btn.textContent = 'Hide';
  } else {
    passInput.type = 'password';
    btn.textContent = 'Show';
  }
}

function toggleApPass() {
  const passInput = document.getElementById('apPass');
  const btn = event.target;
  if (passInput.type === 'password') {
    passInput.type = 'text';
    btn.textContent = 'Hide';
  } else {
    passInput.type = 'password';
    btn.textContent = 'Show';
  }
}

function saveApPassword() {
  const statusEl = document.getElementById('apPassStatus');
  const passInput = document.getElementById('apPass');
  const newPassword = passInput.value;

  if (newPassword.length < 8) {
    statusEl.textContent = 'Password must be at least 8 characters';
    statusEl.style.color = '#e74c3c';
    return;
  }

  statusEl.textContent = 'Saving...';
  statusEl.style.color = '#f39c12';

  fetch('/setApPassword?password=' + encodeURIComponent(newPassword))
    .then(r => r.text())
    .then(msg => {
      statusEl.textContent = 'Saved! Robot will restart...';
      statusEl.style.color = '#2ecc71';
      setTimeout(() => alert('AP password updated. Robot will restart.'), 500);
    })
    .catch(err => {
      statusEl.textContent = 'Error: ' + err;
      statusEl.style.color = '#e74c3c';
    });
}

function validateHostname(name) {
  // Remove Polish characters and diacritics
  let cleaned = name.normalize("NFD").replace(/[\u0300-\u036f]/g, "");
  // Replace spaces with hyphens
  cleaned = cleaned.replace(/\s+/g, "-");
  // Remove invalid characters (only a-z, 0-9, - allowed), keep case
  cleaned = cleaned.replace(/[^a-zA-Z0-9-]/g, "");
  // Remove hyphens at start/end
  cleaned = cleaned.replace(/^-+|-+$/g, "");
  // Limit length
  cleaned = cleaned.substring(0, 32);
  return cleaned || "sesame-robot";
}

function saveDeviceName() {
  const statusEl = document.getElementById('deviceNameStatus');
  const inputEl = document.getElementById('deviceName');
  let originalName = inputEl.value.trim();

  if (!originalName) {
    statusEl.textContent = 'Please enter a device name';
    statusEl.style.color = '#e74c3c';
    return;
  }

  const validatedName = validateHostname(originalName);
  const normalizedOriginal = originalName.toLowerCase().replace(/\s+/g, '-');

  // Only show correction if there are actual invalid characters (not just case difference)
  if (validatedName.toLowerCase() !== normalizedOriginal) {
    statusEl.textContent = 'Name corrected: ' + validatedName;
    statusEl.style.color = '#f39c12';
  } else {
    statusEl.textContent = 'Saving...';
    statusEl.style.color = '#f39c12';
  }

  fetch('/setHostname?hostname=' + encodeURIComponent(validatedName))
    .then(r => r.text())
    .then(msg => {
      statusEl.textContent = 'Saved! Robot will restart...';
      statusEl.style.color = '#2ecc71';
      setTimeout(() => alert('Device name updated to: ' + validatedName + '. Robot will restart.'), 500);
    })
    .catch(err => {
      statusEl.textContent = 'Error: ' + err;
      statusEl.style.color = '#e74c3c';
    });
}

function scanWiFi() {
  const statusEl = document.getElementById('wifiStatus');
  const selectEl = document.getElementById('wifiSSID');
  statusEl.textContent = 'Scanning...';
  statusEl.style.color = '#f39c12';

  fetch('/scan')
    .then(r => r.json())
    .then(networks => {
      selectEl.innerHTML = '';
      networks.forEach(net => {
        const opt = document.createElement('option');
        opt.value = net.ssid;
        opt.textContent = net.ssid + ' (' + net.rssi + ' dBm)';
        selectEl.appendChild(opt);
      });
      statusEl.textContent = 'Found ' + networks.length + ' networks';
      statusEl.style.color = '#2ecc71';
    })
    .catch(err => {
      statusEl.textContent = 'Scan failed: ' + err;
      statusEl.style.color = '#e74c3c';
    });
}

function connectWiFi() {
  const ssid = document.getElementById('wifiSSID').value;
  const pass = document.getElementById('wifiPass').value;
  const statusEl = document.getElementById('wifiStatus');

  if (!ssid) {
    statusEl.textContent = 'Please select a network';
    statusEl.style.color = '#e74c3c';
    return;
  }

  statusEl.textContent = 'Connecting...';
  statusEl.style.color = '#f39c12';

  fetch('/wificonnect?ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass))
    .then(() => {
      statusEl.textContent = 'Saved! Rebooting...';
      statusEl.style.color = '#2ecc71';
      alert('WiFi credentials saved. Sesame will restart and connect to ' + ssid);
    })
    .catch(err => {
      statusEl.textContent = 'Error: ' + err;
      statusEl.style.color = '#e74c3c';
    });
}

function resetWiFi() {
  if (!confirm('Reset saved WiFi credentials?')) return;

  fetch('/resetwifi')
    .then(() => {
      alert('WiFi reset. Sesame will restart in AP-only mode.');
    })
    .catch(err => {
      alert('Error: ' + err);
    });
}

// Load theme on page load
document.addEventListener('DOMContentLoaded', () => {
  loadTheme();
  loadDeviceName();
  updateMelodyDropdown();
});

function loadDeviceName() {
  fetch('/api/status')
    .then(r => r.json())
    .then(data => {
      // Convert hostname to friendly display name
      // sesame-green -> Sesame Green, sesame-robot -> Sesame Robot
      let friendlyName = data.hostname || 'Sesame';
      friendlyName = friendlyName.replace(/-/g, ' ');
      friendlyName = friendlyName.split(' ').map(word =>
        word.charAt(0).toUpperCase() + word.slice(1)
      ).join(' ');

      document.getElementById('robotTitle').textContent = friendlyName;
      document.title = friendlyName + ' Controller';
    })
    .catch(() => {
      document.getElementById('robotTitle').textContent = 'Sesame';
    });
}

function loadTheme() {
  const savedColor = localStorage.getItem('themeColor');
  if (savedColor) {
    applyTheme(savedColor);
  }
}

function applyTheme(color) {
  const root = document.documentElement;
  root.style.setProperty('--content-color', color);

  // Calculate darker shades
  const rgb = hexToRgb(color);
  if (rgb) {
    const dark = `rgb(${Math.max(0, rgb.r - 20)}, ${Math.max(0, rgb.g - 20)}, ${Math.max(0, rgb.b - 20)})`;
    const darker = `rgb(${Math.max(0, rgb.r - 40)}, ${Math.max(0, rgb.g - 40)}, ${Math.max(0, rgb.b - 40)})`;
    const glow = `rgba(${rgb.r}, ${rgb.g}, ${rgb.b}, 0.3)`;

    root.style.setProperty('--content-color-dark', dark);
    root.style.setProperty('--content-color-darker', darker);
    root.style.setProperty('--content-color-glow', glow);
  }
}

function hexToRgb(hex) {
  const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
}

function updateQueueStatus() {
  const queueEl = document.getElementById('queueStatus');
  queueEl.textContent = `Command Queue: ${commandQueue}/${MAX_COMMANDS}`;
  if (commandQueue >= MAX_COMMANDS) {
    queueEl.classList.add('full');
  } else {
    queueEl.classList.remove('full');
  }
}

function canSendCommand() {
  return commandQueue < MAX_COMMANDS;
}

function incrementQueue() {
  commandQueue++;
  updateQueueStatus();
  setTimeout(() => {
    if (commandQueue > 0) {
      commandQueue--;
    }
    updateQueueStatus();
  }, 1000);
}

function lockMotors(duration = 3000) {
  motorsLocked = true;
  document.getElementById('lockIndicator').classList.add('active');
  for (let i = 1; i <= 8; i++) {
    const slider = document.getElementById('motor' + i);
    if (slider) slider.disabled = true;
  }
  setTimeout(() => {
    motorsLocked = false;
    document.getElementById('lockIndicator').classList.remove('active');
    for (let i = 1; i <= 8; i++) {
      const slider = document.getElementById('motor' + i);
      if (slider) slider.disabled = false;
    }
  }, duration);
}

function move(dir) {
  if (!canSendCommand()) return;
  incrementQueue();
  fetch('/cmd?go=' + dir).catch(console.log);
}

function stop() {
  commandQueue = 0;
  updateQueueStatus();
  fetch('/cmd?stop=1').catch(console.log);
}

function pose(name) {
  if (!canSendCommand()) return;
  incrementQueue();
  lockMotors(3000);
  fetch('/cmd?pose=' + name).catch(console.log);
}

function updateMotor(motorNum, value) {
  if (motorsLocked) return;
  document.getElementById('m' + motorNum + 'val').textContent = value + '\u00B0';
  if (!canSendCommand()) return;
  incrementQueue();
  fetch('/cmd?motor=' + motorNum + '&value=' + value).catch(console.log);
}

function openSettings() {
  fetch('/getSettings').then(r => r.json()).then(data => {
    document.getElementById('frameDelay').value = data.frameDelay || 100;
    document.getElementById('walkCycles').value = data.walkCycles || 10;
    document.getElementById('motorCurrentDelay').value = data.motorCurrentDelay || 20;
    document.getElementById('motorSpeed').value = data.motorSpeed || 'medium';

    // Load theme settings
    const savedColor = localStorage.getItem('themeColor') || '#ff8c42';
    const colorSelect = document.getElementById('themeColor');
    const customColorInput = document.getElementById('customColor');

    // Check if saved color matches a preset
    let matchFound = false;
    for (let option of colorSelect.options) {
      if (option.value === savedColor) {
        colorSelect.value = savedColor;
        matchFound = true;
        break;
      }
    }

    if (!matchFound) {
      colorSelect.value = 'custom';
      customColorInput.value = savedColor;
      customColorInput.style.display = 'block';
    }

    document.getElementById('settingsPanel').style.display = 'block';
  }).catch(() => {
    // Fallback if settings endpoint doesn't exist yet
    document.getElementById('frameDelay').value = 100;
    document.getElementById('walkCycles').value = 10;
    document.getElementById('motorCurrentDelay').value = 20;

    const savedColor = localStorage.getItem('themeColor') || '#ff8c42';
    document.getElementById('themeColor').value = savedColor;

    document.getElementById('settingsPanel').style.display = 'block';
  });

  // Add event listener for theme color change
  document.getElementById('themeColor').addEventListener('change', function() {
    const customColorInput = document.getElementById('customColor');
    if (this.value === 'custom') {
      customColorInput.style.display = 'block';
    } else {
      customColorInput.style.display = 'none';
      applyTheme(this.value);
    }
  });

  document.getElementById('customColor').addEventListener('input', function() {
    applyTheme(this.value);
  });
}

function closeSettings() {
  document.getElementById('settingsPanel').style.display = 'none';
}

function openMotorControl() {
  document.getElementById('motorControlPanel').style.display = 'block';
  fetch('/api/servoPositions')
    .then(r => r.json())
    .then(data => {
      for (let i = 1; i <= 8; i++) {
        const slider = document.getElementById('motor' + i);
        const valEl = document.getElementById('m' + i + 'val');
        const servoKey = 's' + (i - 1);
        if (data[servoKey] !== undefined) {
          slider.value = data[servoKey];
          valEl.textContent = data[servoKey] + '\u00B0';
        }
      }
    })
    .catch(console.log);
}

function closeMotorControl() {
  document.getElementById('motorControlPanel').style.display = 'none';
}

function saveSettings() {
  const fd = document.getElementById('frameDelay').value;
  const wc = document.getElementById('walkCycles').value;
  const mcd = document.getElementById('motorCurrentDelay').value;
  const ms = document.getElementById('motorSpeed').value;

  // Save theme color
  const colorSelect = document.getElementById('themeColor');
  const customColorInput = document.getElementById('customColor');
  const themeColor = colorSelect.value === 'custom' ? customColorInput.value : colorSelect.value;
  localStorage.setItem('themeColor', themeColor);
  applyTheme(themeColor);

  fetch(`/setSettings?frameDelay=${fd}&walkCycles=${wc}&motorCurrentDelay=${mcd}&motorSpeed=${ms}`)
    .then(() => closeSettings())
    .catch(() => closeSettings());
}

let activeGamepadIndex = null;
let gamepadPollId = null;
let lastButtonStates = [];
let lastAxisDir = { x: 0, y: 0 };
const axisThreshold = 0.5;
const pollIntervalMs = 80;

const buttonBindings = {
  0: () => pose('stand'),   // A / Cross
  1: () => pose('wave'),    // B / Circle
  2: () => pose('dance'),   // X / Square
  3: () => pose('swim'),    // Y / Triangle
  4: () => pose('point'),   // LB / L1
  5: () => pose('pushup'),  // RB / R1
  6: () => pose('bow'),     // LT / L2
  7: () => pose('shake'),   // RT / R2
  8: () => stop(),          // Back / Share
  9: () => pose('rest'),    // Start / Options
  10: () => pose('cute'),   // L3
  11: () => pose('freaky'), // R3
  12: () => move('forward'),// D-pad up
  13: () => move('backward'),// D-pad down
  14: () => move('left'),   // D-pad left
  15: () => move('right'),  // D-pad right
  16: () => stop(),         // Home / PS
  17: () => pose('worm')    // Touchpad / extra
};

const buttonReleaseStop = new Set([12, 13, 14, 15]);

function updateGamepadStatus(connected) {
  const status = document.getElementById('gamepadStatus');
  if (!status) return;
  if (connected) {
    status.textContent = 'Gamepad connected';
    status.classList.add('connected');
  } else {
    status.textContent = 'Gamepad disconnected';
    status.classList.remove('connected');
  }
}

function handleButtonChange(index, pressed) {
  if (pressed) {
    const action = buttonBindings[index];
    if (action) action();
  } else if (buttonReleaseStop.has(index)) {
    stop();
  }
}

function getAxisDirection(x, y) {
  if (Math.abs(x) < axisThreshold && Math.abs(y) < axisThreshold) return { x: 0, y: 0 };
  if (Math.abs(x) > Math.abs(y)) {
    return { x: x > 0 ? 1 : -1, y: 0 };
  }
  return { x: 0, y: y > 0 ? 1 : -1 };
}

function applyAxisDirection(dir) {
  if (dir.x === 1) move('right');
  else if (dir.x === -1) move('left');
  else if (dir.y === 1) move('backward');
  else if (dir.y === -1) move('forward');
  else stop();
}

function pollGamepad() {
  const pads = navigator.getGamepads ? navigator.getGamepads() : [];
  const pad = pads && activeGamepadIndex !== null ? pads[activeGamepadIndex] : null;
  if (!pad) {
    updateGamepadStatus(false);
    return;
  }
  updateGamepadStatus(true);

  if (!lastButtonStates.length) {
    lastButtonStates = pad.buttons.map(b => !!b.pressed);
  }
  pad.buttons.forEach((btn, i) => {
    const pressed = !!btn.pressed;
    if (pressed !== lastButtonStates[i]) {
      handleButtonChange(i, pressed);
      lastButtonStates[i] = pressed;
    }
  });

  const x = pad.axes[0] || 0;
  const y = pad.axes[1] || 0;
  const dir = getAxisDirection(x, y);
  if (dir.x !== lastAxisDir.x || dir.y !== lastAxisDir.y) {
    applyAxisDirection(dir);
    lastAxisDir = dir;
  }
}

window.addEventListener('gamepadconnected', (e) => {
  activeGamepadIndex = e.gamepad.index;
  lastButtonStates = [];
  lastAxisDir = { x: 0, y: 0 };
  updateGamepadStatus(true);
  if (!gamepadPollId) {
    gamepadPollId = setInterval(pollGamepad, pollIntervalMs);
  }
});

window.addEventListener('gamepaddisconnected', (e) => {
  if (activeGamepadIndex === e.gamepad.index) {
    activeGamepadIndex = null;
    lastButtonStates = [];
    lastAxisDir = { x: 0, y: 0 };
    updateGamepadStatus(false);
  }
});

if (navigator.getGamepads) {
  setInterval(() => {
    if (activeGamepadIndex !== null) return;
    const pads = navigator.getGamepads();
    if (!pads) return;
    for (let i = 0; i < pads.length; i++) {
      if (pads[i]) {
        activeGamepadIndex = pads[i].index;
        updateGamepadStatus(true);
        if (!gamepadPollId) {
          gamepadPollId = setInterval(pollGamepad, pollIntervalMs);
        }
        break;
      }
    }
  }, 1000);
}
</script>
</body>
</html>
)rawliteral";
