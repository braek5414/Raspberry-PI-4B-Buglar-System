# Raspberry-PI-4B-Buglar-System
# 🛡️ Raspberry Pi Burglar Alarm System (UDP)

A simple, practical burglar alarm system built for the Raspberry Pi.  
It watches a door sensor, a PIR motion sensor, and an arm/disarm button, and sends short UDP messages (`"door"`, `"motion"`, `"armed"`, `"disarmed"`, `"pulse"`) to a remote receiver whenever something happens. Great for learning GPIO, callbacks, timers, and basic network messaging.

---

## 📦 What’s Included

- `balarm_client.c` — main application (GPIO + UDP logic)
- `udp3.c` / `udp3.h` — helper functions for UDP communication
- `kbhit.h` — keyboard input helper
- `README.md` — this file

---

## 🧰 Hardware & Software Requirements

- Raspberry Pi (any model with GPIO headers)
- pigpio library (`pigpiod` daemon) for GPIO callbacks and timers
- A door sensor (e.g. magnetic reed switch)
- A PIR motion sensor
- A push button for arm/disarm
- C compiler (e.g. `gcc`)
- A remote machine (server/computer) that will receive UDP messages on port `4096`
- Root (sudo) to run the program (required for GPIO access)

---

## ⚙️ Pin & Wiring Notes

- The code uses **Broadcom (BCM)** GPIO numbering. Update the constants in `balarm_client.c` to match your wiring:
```c
#define DOOR       2    // GPIO pin for door sensor (BCM numbering)
#define PIR        14   // GPIO pin for PIR sensor
#define ARM_DISARM 3    // GPIO pin for arm/disarm button
