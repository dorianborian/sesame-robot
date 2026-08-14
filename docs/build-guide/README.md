# Build Guide

Complete build guide for the Sesame Robot.

Use this walkthrough alongside the BOM, wiring guide, and printing notes to stay organized. The table below previews each phase, its goal, and the doc to reference if you get stuck.

| Phase                     | Goal                                  | Est. Time     | Key References                                                                                 |
| ------------------------- | ------------------------------------- | ------------- | ---------------------------------------------------------------------------------------------- |
| 1. Gather parts           | Print plastics and source electronics | 1–2 sessions | [hardware/bom](../../hardware/bom/README.md), [hardware/printing](../../hardware/printing/README.md) |
| 2. Electronics & wiring   | Build ~90% of the harness             | 1 session     | [docs/wiring-guide](../wiring-guide/README.md)                                                    |
| 3. Hardware pre-assembly  | Prep joints, feet, top cover          | 1 session     | This guide (Phase 3)                                                                           |
| 4. Hardware main assembly | Mount motors + electronics            | 1–2 sessions | This guide (Phase 4)                                                                           |
| 5. Calibrate & finish     | Flash tester, align joints, close up  | 1 session     | Firmware README + remaining sections                                                           |

> [!TIP]
> Treat each phase like a milestone. Snap progress photos and mark issues before moving on so troubleshooting later is painless.

<p align="center">
  <a href="https://www.youtube.com/watch?v=NIgoQVQF_Ng">
    <img src="https://github.com/user-attachments/assets/1663e022-0680-4053-97b4-53e669a6f07d" width="70%" alt="tutorial-button">
  </a>
</p>

## Phase 1: Gathering all the parts.

**Goal:** Print the full shell set and collect every electronic, connector, and fastener before any soldering starts.

**You’ll need:**

- [hardware/bom](../../hardware/bom/README.md) for electronics, power gear, and Amazon search links.
- [hardware/printing](../../hardware/printing/README.md) for STL names, orientations, and support notes.

There are 11 printed parts (internal frame, top/bottom covers, joints R1–R4/L1–L4) plus 6–8 main electronic components depending on the wiring approach.

Here's what a complete set looks like:

<img src="assets/all-hardware.png" alt="all-hardware" width="70%">

**Phase 1 checklist**

- [ ] All plastic parts printed, and cleaned up.
- [ ] MG90 servos tested quickly on a servo tester or Arduino to catch DOA units.
- [ ] Power plan decided (USB-C PD vs. battery + buck) and matching connectors sourced (2× 14500 Li-ion cells in a 2× AAA holder fit the stock battery cavity).
- [ ] Consumables stocked: solder, flux, heat-shrink, zip ties, M2 hardware.

## Phase 2: Electronics and Wiring

**Goal:** Build 90% of the harness on the bench so the frame install is quick.

1. Open the [wiring guide](../wiring-guide/README.md) and pick the section that matches your build (S2 Mini hand-wired, Distro Board V3/V2, or Distro Board V1 legacy).
2. Lay out every connector in the order shown on the wiring diagram before soldering; this keeps the data lines from getting crossed.
3. Tin and solder the rails/buck converter first, then route signal wires. Leave generous length for the motors that terminate near the hips.
4. This is optional but you can also label each servo lead (S0–S7) using tape flags as soon as it is soldered. Future you will thank you.

> [!WARNING]
> Stop before permanently wiring the power switch or OLED. Those final joints happen after the electronics are seated in the frame so you can dial the cable length exactly.

**Phase 2 checklist**

- [ ] Harness built per your wiring diagram with all joints strain-relieved.
- [ ] Buck converter trimmed to 5.1 V output and shrink-wrapped.
- [ ] Servo leads labeled and loosely bundled by destination.
- [ ] Power switch and OLED leads pre-cut but still un-soldered.

## Phase 3: Hardware Pre-Assembly

### Hip Joints

**Goal:** Pre-load the four hip joints (R1, R2, L1, L2) with one-sided servo horns so installation is tool-free later.

1. Lay out the four hip joint parts. Find the one-sided servo horns from the servo motor bags (ignore the included extra screws).
2. Notice the servo horn has a taller side and a shorter side. Press the shorter side into the hip joint. There should be just enough clearance to press this piece in flush.
3. While holding the horn flat, drive an M2 × 5 mm self-threading screw (the ones with a larger gap between thread spikes) through the second hole in the horn and into the plastic until secure.

<img src="assets/joint-horn-install.png" alt="install-horn" width="70%">

> [!TIP]
> Do not over-tighten self-threading screws. Since the threads are plastic, over-tightening will permanently damage the hole.

Repeat for all four hip joints:

<img src="assets/femur-joints.png" alt="hip-joints" width="70%">

> [!IMPORTANT]
> Do **not** mount these joints on the servos yet. Calibration requires every motor shaft to spin freely.

<img src="assets/dont-mount.png" alt="dont-mount" width="70%">

### Leg Joints

Slide each leg shell over its dedicated motor before the frame install. Use the built-in gap to route wires without pinching them.

<img src="assets/servo-install-leg.png" alt="servo-install-leg" width="70%">

- Feed the motor wires through the side slot.
- Push the motor into the side of the leg piece that has holes in it. Make sure the motor shaft is at the very top.
- Confirm the motor sits flush and use self-threading screws to permanently affix the motors into the leg pieces.

Repeat for the remaining legs.

<img src="assets/foot-joints.png" alt="leg-joints" width="70%">

### Top Cover & Soldering

The next step is loading the OLED and power switch into the top cover. At this stage, we also do the minimal soldering required for the build.

1. **Power Switch:** Press the battery switch firmly into the back slot of the top cover. DO NOT connect the wires yet.

<img src="assets/switch-insert.png" alt="insert-rocker-switch" width="70%">

2. **OLED Display:** Apply a small amount of solder to the pin headers on the display (tinning). Using the pre-crimped JST connector wires, match the wires to their respective marks on the display and solder them.

<img src="assets/display-soldering.png" alt="display-soldering" width="70%">

3. **Battery Connectors (Main Board):** Remove sheathing on the ends of the battery connector wires, tin them, and solder them to the battery pads on the main board in an L-shape pointing upwards. Make sure they do not go over the edge of the board, or the top cover won't fit.

<img src="assets/battery-connector-solder.png" alt="battery-connector-solder" width="70%">

4. **Install Display:** Push the wired OLED display gently into its slot inside the top cover. Insert self-threading screws into the small screw holes beside the display to clamp it permanently in place.

<img src="assets/insert-display-enclosed.png" alt="securing-display" width="70%">

**Phase 3 checklist**

- [ ] Hip joints pre-loaded with servo horns, not yet attached to motors.
- [ ] Leg pieces installed on every motor.
- [ ] Power switch and OLED display installed in the top cover.
- [ ] Battery connectors soldered to the main board.

## Phase 4: Hardware Main Assembly

**Goal:** Seat the frame motors, route wiring, and secure the electronics stack so the robot is ready for calibration.

1. **Battery Holder:** Insert the battery holder into the back of the internal frame piece. Route the red and black wires up the slot.
2. **Motor Installation:** While holding the battery wires in place, insert the four remaining side motors. The motor shaft should be facing towards the outside of the internal frame.

- Insert motors at an angle and press them the rest of the way. Make sure not to pinch any wires at the bottom.

3. **Secure Motors:** Use self-threading screws to permanently affix all four side motors to the frame.

<img src="assets/insert-frame.png" alt="insert-motors" width="70%">

<img src="assets/rotate-motor.png" alt="rotate-motor" width="70%">

Make sure the motor shafts are closest to the outer edge of the frame.

<img src="assets/install-frame-motors.png" alt="complete-motors" width="70%">

## Installing the Main Electronics

Before dropping hardware in, trim or bundle any stray wires so nothing can flop into the servo gears and double-check that every connection exits upward.

1. Pre-route any long wires upward so they naturally hug the future top cover.
2. Lower the electronics harness into the cavity while keeping the USB port toward the rear.
3. **S2 Mini build:** Screw the controller (and optional protoboard) directly into the frame using the provided mounting holes. If you built a small protoboard backplane, use the spare holes in the internal frame to secure it so the servo plugs are easy to reach later.

<img src="assets/secure-electronics.png" alt="s2-secure-electronics" width="70%">

4. **Distro Board V1 build (Legacy):** Install four M2.5 × 5 mm male-female standoffs to raise the Sesame Distro Board V1 so it clears the ESP32 DevKit. Then secure the assembly using the top screws.

<img src="assets/secure-distro-board.png" alt="secure-distro-board" width="70%">

5. **Distro Board V3/V2 build (Build Kits):** Mount by gently bringing cables towards the center nicely so that the main board can sit properly. Attach the main board to the internal frame. It has two mounting holes in the corner that you attach using self-threading screws.

<img src="assets/secure-distro-boardv2.png" alt="secure-distro-boardv2" width="70%">

> [!TIP]
> Notch = front. USB port = back. Use these two cues any time the orientation feels ambiguous.

## Calibrating & Running the Testing Firmware

**Goal:** Teach the controller where each motor sits so the walking poses land correctly.

> [!CAUTION]
> Never run calibration with joints attached. A misaligned horn can stall or strip a servo instantly. All motor shafts should be free spinning at this point.

**For S2 Mini and Distro Board V1 (Legacy) Builds:**

1. Inspect the harness to ensure no bare conductors can short during testing. Add heat-shrink or tape where needed.
2. Connect a reliable USB-C cable and flash `sesame-motor-tester.ino` from the [debugging-firmware/sesame-motor-tester](../../firmware/debugging-firmware/sesame-motor-tester/) folder using Arduino IDE. If you have never flashed an ESP32 before, pause here and follow a quick tutorial so you are comfortable resetting/entering boot mode.
3. Open the serial monitor. You should see the tester menu.
4. Command all motors to 90°. Starting from Motor 0, plug its connector into the appropriate header. The servo should immediately whirr into the 90° position. Repeat for Motors 1–7. Skip to Step 4 below.

**For Distro Board V3/V2 (Build Kits):**
If you are using a V3 or V2 Board from a Build Kit, it comes **pre-flashed** with the Sesame firmware! There is no need to plug into a computer and use Arduino IDE to flash firmware.

1. **Power Up:** Plug the board into a strong USB power supply (fast chargers for phones or laptop chargers work great). Note: If you are using the V2 Distro board, it currently has instabilities with the battery system and won't work on batteries, you must plug via USB-C.
2. **Connect to Portal:** Check the Wi-Fi networks on your phone or computer. The Sesame board will create an access point. Connect to it to open the controller portal. (If on desktop and the portal doesn't show, go to `sesame-root.local` in your browser. Mobile is recommended.)
3. **Initialize PWM:** Tap the "Standby" button in the portal to initialize the PWM channels. The moment you plug a motor in, it will go to its correct active position.

**General instructions continuing:**

4. **Refer to Diagram:** Use the diagram below to associate motor numbers with their physical position. Ensure the frames are roughly oriented correctly.

<img src="assets/sesame-angle-guide.png" alt="angle-guide" width="100%">

5. **Plug Motors In:** Starting with Motor 0, plug the connector into the corresponding board header.
   - **Crucial:** Ensure the brown wire aligns with the ground rail! Upon plugging it in, you should hear it whir into life and hold position.
   - Repeat for all 8 motors.

> [!TIP]
> 99% of the time, if your motor is moving in the wrong direction, crashing, or being sporadic, the motor is plugged into the wrong slot. Check your wiring!

## Attaching Hip & Leg Joints

Now we attach the joints while the motors are turned on and holding their "Stand" position.

1. **Hip Joints:** Select the matching hip joint for your motor. While in "Stand" mode, push the hip joint onto the motor shaft at a 45-degree angle.
2. **Test Angles:** Tap "Rest" in the portal. The hip joint should move perfectly parallel to the body. Tap "Stand" again, and it should go back to 45°. Alternate this to ensure it's placed correctly.
3. **Fasten Hip:** If it looks good, fasten the hip joint to the motor using an M2.5 machine screw through the main hole into the motor shaft. Gently tighten (don't over-torque against the motor as this may cause a brownout).
4. **Legs:** Repeat the same process with the leg joints. Check against "Rest" and "Stand", ensure nothing collides, and then affix using screws.

<img src="assets/reference-configuration.png" alt="reference-configuration" width="70%">

## Final Wiring and Top Cover

Now to clean up the robot and secure everything.

1. **Battery Connections:** Insert the battery wires into the screw terminal on the board and use a flathead screwdriver to tighten them securely.
2. **Wire Routing:** Flip the robot over. There are channels for all the wires on the underside. Securely press all of the wires into these channels.
3. **Bundling:** Separate the wires gently into two groups (left and right). Wrap a zip tie around each bundle, fold it once, and tighten the zip tie to create a tight bundle. This helps the top cover fit seamlessly.

<img src="assets/wire-routing.png" alt="wire-routing" width="70%">

<img src="assets/zip-tie-bundles.png" alt="zip-tie-bundles" width="70%">

4. **Testing the Switch:** Make sure the batteries are charged and insert them. Flip the switch and check if the Wi-Fi network appears or if the display lights up. If things aren't working, double-check connections.

> [!NOTE]
> **V2 Board Owners:** If you are using the V2 Distro board from a kit, there are instabilities with the battery system that prevent reliable battery usage. You must run your Sesame via USB-C (fast charger / laptop charger) for now. A free V3 Distro board will be provided to pre-order kit buyers!

5. **Top Cover:** Gently pack all the bundles in and slowly press the top cover down onto the electronic components. Be very gentle to avoid shorting or disconnecting anything.
6. **Securing Top Cover:** Hold the whole robot together and insert the M2 self-threading screws into the bottom of the frame where they thread securely into the mounting holes for the top cover.

<img src="assets/secure-top-cover.png" alt="secure-top-cover" width="70%">

## Bottom Cover & Final Checks

<img src="assets/insert-battery.png" alt="insert-battery" width="70%">

1. Set the bottom cover in place, checking that no wires are trapped. Use the remaining two self-threading screws into the bottom to secure it.

<img src="assets/screw-on-bottom-cover.png" alt="screw-on-bottom-cover" width="70%">

2. You can apply the included sticky pads to the feet if you like!

**Phase 4 checklist**

- [ ] Motors mounted and screwed in on hip and leg joints.
- [ ] Harness routed into channels, zip-tied without pinch points.
- [ ] Top and bottom covers installed.

> [!TIP]
> A common issue on battery power (if you upgrade) is that your Sesame robot will crash on movement. Setting the motor current delay higher in the settings may help, but upgrading to a Lithium Polymer (LiPo) cell often solves it for advanced users!

Great job! The hardware is complete.

<img src="assets/sesame-done.png" alt="sesame-done" width="70%">

## Final Checks and Flashing the Firmware

Now we just need to do a few checks to make sure everything is working and we can flash the firmware onto the microcontroller.

**Sanity checks**

1. Toggle the power switch (if you installed a battery) and confirm it fully disconnects the pack.
2. Confirm the USB-C port is accessible for future firmware updates.
3. Inspect the OLED wiring one last time to ensure SDA/SCL aren’t swapped.

**Flash production firmware (For DIY / S2 Mini / Distro V1)**
If you are using the V2 Build kit board, you can skip this step! It is pre-flashed.

1. Choose the sketch folder from the firmware folder. Customize pin definitions if you're using a different ESP32.
2. Use Arduino IDE to flash the full firmware bundle (motors + faces). Detailed notes live in [firmware/README.md](../../firmware/README.md).
3. Connect to Sesame’s Wi-Fi AP, load the control page, and trigger a pose to verify everything moves as expected.

That’s it. You’ve built Sesame! From here you can design new poses in [Sesame Studio](../../software/sesame-studio/README.md) or tweak the hardware per [hardware/README.md](../../hardware/README.md), or whatever you want!
