# The Sesame Robot Project 
<img width="95%" height="1851" alt="sesame-wide" src="https://github.com/user-attachments/assets/84ea4e49-6fb9-46db-a336-ad07526de633" />


**Greetings, from your new best friend.**

Sesame is an accessible Open-Source robotics project based on the ESP32 microcontroller system, with an emphasis on expression and movement. This project is designed for makers and engineers of all skill levels. Sesame offers a dynamic platform designed to start working with walking robots. To build a sesame robot, you will need soldering skills, a soldering iron with a small tip, various hardware components, access to a 3D printer, and a basic understanding of Arduino IDE and how to communicate with the ESP32 over serial and WiFi.

This repository contains the CAD design files, STL files for the newest hardware revision, a list of all needed and alternate components, and the base/expanded firmware for the ESP32-based controller. There is also some included debugging firmware that may be helpful in getting your Sesame up and running.

> [!WARNING]
> This Repository is under Construction! Details are subject to change.

## Features

*   **Quadruped Design:** Uses 8 servo motors (2 per leg) to achieve roughly 8 total degrees of freedom.
*   **Emotive Display:** Features a 128x64 OLED screen acting as a reactive face that syncs with movement.
*   **Fully Printable:** Designed entirely for 3D printing in PLA with minimal supports.
*   **Serial CLI:** Control the robot and trigger animations via a Serial Command Line Interface or the web UI.
*   **Pre-programmed Emotes:** Includes animations for Walking, Waving, Dancing, Pointing, Resting, and more.

## Hardware Requirements

The Sesame Robot Project is split into two main versions, one using a Custom PCB and one using a proto-board distributor. 
For more details on which version of Sesame is best for you, see [timestamp] in the sesame build tutorial.

*   **Microcontroller:** ESP32 S2 Mini Development Board with WiFi connectivity
*   **Actuators:** 8x MG90 Micro Metal Gear Servo Motors.
*   **Display:** 0.96" I2C OLED Display (SSD1306 driver, 128x64).
*   **Power:** 5V Power Supply suitable for 8 servos (3s 450mah Li-Po, or 3x 10440 Li-Ion cells).
*   **Body:** 3D Printed Parts (Files available in the `/Hardware` directory).
> [!NOTE]
> Sesame requires a minimum power source of 5v 3A. This can be provided by most modern Type-C PD cables and will run Sesame tethered to a computer or power supply. To run Sesame wireless, a battery rated for 5-12v can be connected to the power terminal and switch on the Custom PCB or the buck converter and switch in the non-pcb version. Sesame will not work on USB-A 5v 1.5A, or 3xAAA 4.5v due to limitations in current draw for all motors. See the wiring diagram for both versions of Sesame for more details. Details with visuals are also available in the Sesame build tutorial.


## Fabrication

Sesame is designed to be printed in **PLA**. Most parts are designed to be printed without supports, but the internal frame and body cover both require supports in specific spots. The 3D Printing guide shows exactly where to place these supports for each part, and some recommended settings to get the best results.

*   **Material:** PLA / PLA+
*   **Infill:** 15-20%
*   **Supports:** Minimal (Check individual file notes)

## Firmware

The firmware provided in this repository controls the servo positions and the OLED face updates.

> [!NOTE]
> **Note from the Creator:** I am a hardware and design specialist, not a software wizard. This firmware works and provides a great platform, but it is basic. The community is highly encouraged to contribute, refactor, and improve the code logic. Additionally, this firmware is reliant on calls to servo angles and does not involve any kinematics or positional systems.

### Dependencies

You will need the following libraries installed in your Arduino IDE:
`#include <WiFi.h>`
`#include <WebServer.h>`
`#include <DNSServer.h>`
`#include <Wire.h>`
`#include <ESP32Servo.h>`
`#include <Adafruit_GFX.h>`
`#include <Adafruit_SSD1306.h>`

### Setup

1.  **Board Setup:** Select your specific ESP32 board in the Arduino IDE Tools menu. For the Custom PCB, this should be ESP32-WROOM32, and for non-pcb this should be LOLIN S2 Mini.
2.  **Connections:**
    *   See wiring diagram for specific connections
3.  **Network Configuration:** Open the firmware file and configure your desired Access Point name (SSID) and password, or set it to connect to your home router.
4.  **Upload:** Upload the firmware to the ESP32.
5.  **Monitor:** Open the Serial Monitor at **115200 baud** to see the assigned IP address.
`Try enabling "USB CDC on Boot" in the Arduino IDE settings if you can't see info in the Serial Monitor`

### WiFi Controller Usage

Once the robot is powered on:

1.  Connect your phone or computer to the Robot's WiFi network (or the network the robot is connected to).
2.  Enter the IP address (e.g., `192.168.4.1`) printed in the Serial Monitor into your web browser, or simply click the "Log-in to network" prompt when connecting.
3.  A graphical interface will load, featuring buttons for all available poses, movements, and settings to tweak certain settings.

## Customizing Faces

The faces are stored as byte arrays in `PROGMEM` within the firmware file. You can create your own faces using [this handy 128x64 bitmap converter](https://javl.github.io/image2cpp/).
You can also find [Kaomoji Faces here](https://emojicombos.com/)

1.  Create your image (128x64 pixels, monochrome).
2.  Convert image to a hex array via image2cpp.
3.  Paste the array into the top of the firmware file.
4.  Call `updateFaceBitmap(your_new_bitmap_name)` in the code.

## Kits & Pre-Orders

If you do not have a 3D printer or prefer to get all the hardware in one box, limited pre-orders for full Sesame Build Kits may be available soon. These kits include all the proper servos, drivers, fasteners, and printed parts to build your own Sesame.

*   **Shipping:** [TBA]
*   **Store Link:** [TBA]

## Contributing

This robot is a platform for building new features and ideas. Since the current firmware is a basic implementation, pull requests are very welcome for:
*   Kinematics improvements
*   New animations
*   Improved Web UI/UX
*   Sensor integration (Ultrasonic, Gyro, etc.)

---

*Created by [Dorian Todd]. Need help with your Sesame Robot? Send me a message on Discord, my username is "starphee"
