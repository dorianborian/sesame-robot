# Wiring Guide

Complete wiring guides for the Sesame Robot.

---

## Choosing a Wiring Approach

The project supports two wiring strategies that suit different build constraints:

- **S2 Mini / hand wiring:** Uses an ESP32-S2 Mini, loose headers, and point-to-point wiring. It is the lowest-cost option and easy to source, but it demands patience to keep the harness tidy and leaves less room for mistakes.
- **Sesame Distro Board / ESP32 DevKit:** Uses the custom distro PCB stacked on an ESP32-DevKitC-32E. It front-loads the work into soldering headers and the buck converter but dramatically simplifies routing, labeling, and maintenance once assembled. Please note the V1 board is a prototype and may not have full functionality. I will be updating the design to improve it very soon!

Pick the approach that matches your component availability and comfort with managing wire bundles; the remainder of this guide dives into both workflows.

## How to wire the S2 Mini / Hand Wiring

The S2 Mini method is a traditional protoboard build. Consult the S2 Mini wiring diagram below while reading this section; it captures every motor, sensor, OLED, and button lead even if the photos only show a subset of the connections. Expect a high connection count, so plan wire lengths and colors before soldering to avoid confusion later.

<img src="s2-mini-wiring-guide.png" alt="S2 Mini wiring diagram" width="70%">

### Prep

1. **Secure and tin** all of the connections on the board before starting
2. To connect all the 3-pin headers together and to the board, first attach them to a small section of protoboard

<img src="assets/proto-headers.png" alt="proto-headers" width="70%">

### Building Power and Ground Rails

1. Solder one wire to one end of the pins, then guide it along and remove the insulation
2. Solder this exposed wire to every middle pin (this creates the **5V rail**)
3. Do the same for the ground lane (this creates the **ground rail**)

<img src="assets/proto-power-rails.png" alt="proto-power-rails" width="70%">

### Data Lines

You can now cut eight equal-length wires for data connections.

**Wire Recommendations:**
- **Data lines:** 30AWG silicone wire
- **Power and ground:** 22AWG silicone wire
- **Important:** Things will get super cluttered if you use large gauge wire

<img src="assets/proto-datalines.png" alt="proto-data-lines" width="70%">

Alongside the eight motor data leads, the diagram also calls out the OLED connections that branch from the microcontroller.

### Packing Electronics

When packing electronics into the frame, it's difficult with the hand wiring setup because there are a lot of stray wires. My advice is to work slowly and methodically:

1. Group wires with similar destinations
2. Use zip ties and heat shrink tubing to make them as compact as possible
3. Consider connecting the power switch after finishing most other wiring, since it's attached to the top cover

<img src="assets/wire-managment.png" alt="wire-managment" width="70%">

### Safety and Testing

> [!CAUTION]
> Do not solder and de-solder connections with power connected!

**Before powering on:**
- Double check your power and ground lines before turning the robot on
- Make sure to cover any exposed wires as they can touch the pin headers and fry the ESP32 (especially when cramming the cover on)

> [!TIP]
> To make fishing out the wires for the OLED display easier, you can temporarily twist them together into a group and then guide them through the opening in the top cover.

---

## How to wire the Sesame Distro Board / ESP32-DevKitC-32E

### Overview

> [!CAUTION]  
> UPDATE 1/20/25: Upon further testing, the Sesame distro board V1 will work, but it has a few issues that make it a little harder to assemble and will not run on teathered power (eg. USB C). Until V2 is released, I recommend using the S2 Mini / Hand Wiring approach. If you ordered a distro board V1, it will still be supported with wiring guides and firmware for the foreseeable future <3.

This option provides a cleaner, more organized wiring solution.
Consult the distro-board wiring diagram below for exact connection locations.

<img src="distro-board-wiring-guide.png" alt="distro-board wiring diagram" width="70%">

### Component Installation

**Optional components** (you can solder directly to pads instead):
- 4-pin JST connector
- 2-pin screw terminal

**Pin header installation tip:** If you're having difficulties keeping the pin headers in place to solder, try placing them in a protoboard first, then transferring them over.

### Buck Converter Setup

A buck converter takes any voltage (5V-12V) and drops it to a stable 5V for the motors and ESP32.

**If using a battery:**
1. Make sure to solder the buck converter enable pads
2. The buck converter is **required** for battery operation
3. 2× 10440 Li-ion cells in a 2× AAA holder (2S, ~7.4 V nominal) work well with the buck and fit the chassis battery slot

**Alternative power options:**
- If you're using a benchtop supply or have another regulated 5V source
- You can override the buck converter and take voltage directly from the screw terminal by soldering the override pads

### Battery Connection
> [!CAUTION]
> If your battery has an XT30 or JST RCY connector, don't cut it off and wire the battery directly to the distro board! This is unsafe and means you can't charge your battery anymore.

**Proper method:**
1. Get a female XT30 or JST RCY connector (matching your battery)
2. Wire it to two wires that lead to the power terminal
3. This allows you to safely connect and disconnect the battery

**Note on 2× AAA holders with 10440 Li-ion cells:**
- Treat this as a 2S Li-ion pack; wire the holder leads through the switch and buck converter.
- Recharge the 10440 cells outside the holder using a proper Li-ion charger unless the holder explicitly supports charging.

### Sourcing the Distro Board

The Sesame Distro Board is a custom PCB designed specifically for this project. It mounts on top of a ESP32-DevKitC-32E.

More information is avalible in the [PCB](../../hardware/pcb/README.md) section of the documentation.

---

## General Wiring Tips

- Work slowly and methodically
- Test connections before applying power
- Keep wire runs as short and neat as possible
- Label wires if helpful for troubleshooting
- Take photos during assembly for reference 
