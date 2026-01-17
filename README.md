# The Sesame Robot Project 
___
![License](https://img.shields.io/badge/License-APACHE2.0-yellow)
![Microcontroller](https://img.shields.io/badge/Microcontroller-ESP32-blue)
![Firmware](https://img.shields.io/badge/Firmware-C%2B%2B-blue?logo=c%2B%2B)
![IDE](https://img.shields.io/badge/IDE-Arduino-00979D?logo=arduino&logoColor=white)
![GitHub stars](https://img.shields.io/github/stars/dorianborian/sesame-robot?style=social)
![GitHub forks](https://img.shields.io/github/forks/dorianborian/sesame-robot?style=social)

<img width="100%" height="728" alt="sesame-cover" src="https://github.com/user-attachments/assets/f0cc6ad0-135b-4515-8750-900f224ed7ae" />

___

**Greetings, from your new best friend.**

Sesame is an accessible Open-Source robotics project based on the ESP32 microcontroller system, with an emphasis on expression and movement. 
This project is designed for makers and engineers of all skill levels! Sesame offers a dynamic platform designed to start working with walking robots. 
To build a sesame robot, you will need basic soldering skills, $50-60 in hardware components, access to a 3D printer, and a basic understanding of Arduino IDE.

This repository contains the CAD design files, STL files, build and wiring guides, and the base/expanded firmware for the ESP32-based controller. 
There is also some included debugging firmware that may be helpful in getting your Sesame up and running.

## Features

*   **Quadruped Design:** Uses 8 servo motors (2 per leg) to achieve roughly 8 total degrees of freedom.
*   **Emotive Display:** Features a 128x64 OLED screen acting as a reactive face that syncs with movement.
*   **Fully Printable:** Designed entirely for 3D printing in PLA with minimal supports.
*   **Sesame Studio:** New animation composer software to easily create custom movements.
*   **Serial CLI:** Control the robot and trigger animations via a Serial Command Line Interface or the web UI.
*   **Pre-programmed Emotes:** Includes animations for Walking, Waving, Dancing, Pointing, Resting, and more.


## Watch the launch video on YouTube

<a href="https://www.youtube.com/watch?v=1UDsWkcQZhc"><img src="https://github.com/user-attachments/assets/710cb5a6-163e-47e7-a294-5e2d2ab07627" width="70%" alt="thumb-youtube"></a>

## Getting Started

Follow these steps to build your own Sesame Robot:

### 1. Gather Parts 
Check the **[Bill of Materials (BOM)](hardware/bom/README.md)** for a complete list of required electronics and hardware.
*   Microcontroller: ESP32 S2 Mini
*   Actuators: 8x MG90 Servos
*   Power: 5V 3A source

### 2. Print Parts 
Download the STLs and follow the **[Printing Guide](hardware/printing/README.md)**.
*   Designed for PLA
*   Minimal supports required

### 3. Build & Wire 
Follow the **[Build Guide](docs/build-guide/README.md)** and **[Wiring Guide](docs/wiring-guide/README.md)** to assemble the frame and connect the electronics.

### 4. Flash Firmware 
Upload the code from the **[Firmware Directory](firmware/README.md)**.
*   Requires Arduino IDE
*   Configure WiFi AP settings

### 5. Create Animations 
Use **[Sesame Studio](software/sesame-studio/README.md)** to visually design poses and sequences for your robot.

---

## Software & Firmware

### Sesame Studio
Sesame Studio is a standalone desktop application included in `software/sesame-studio/`. It allows you to:
*   Visually pose the robot using a schematic interface.
*   Generate C++ code for servo angles automatically.
*   Sequence frames into complex animations.

[**> Go to Sesame Studio**](software/sesame-studio/README.md)

<img width="50%" height="925" alt="sesamestudio-preview" src="https://github.com/user-attachments/assets/589de8ec-5895-4d5a-aadf-dd944e42172f" />

### Firmware
The ESP32 firmware handles the kinematics, face display, and WiFi control interface.
*   **Web UI:** Control the robot from your phone via the built-in Access Point.
*   **Custom Faces:** Add your own bitmaps (guide in firmware docs).

[**> Go to Firmware Docs**](firmware/README.md)

<img width="50%" height="1164" alt="firmware-preview" src="https://github.com/user-attachments/assets/31da32af-55a4-45e9-99ca-ff8f9b4767cd" />

---

## Contributing

This robot is a platform for building new features, cosmetics, tools, and ideas. Since the current firmware is a basic implementation, pull requests are very welcome for:
*   Kinematics improvements
*   New animations
*   Improved Web UI/UX
*   Sensor integration (Ultrasonic, Gyro, etc.)

I would also love to see forks of this project with new hardware, software, faces, etc. Be sure to send me a message if you end up building one, and I might feature you on my website or channel!
  
<img width="100%" height="1205" alt="sesame-army" src="https://github.com/user-attachments/assets/63dcb22d-bac7-4052-b3a1-251ad635d380" />

---

*Created by [Dorian Todd](https://www.doriantodd.com/). Need help with your Sesame Robot? Send me a message on Discord, my username is "starphee"*
