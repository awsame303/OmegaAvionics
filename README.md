# 🚀 Omega Avionics — L1 Certification Rocket

A custom-built high-power rocket with a fully designed avionics bay, dual-deployment chute release system, and custom PCB — built to earn an **NAR/Tripoli L1 certification** (H & I class motors).

> **Status:** Design Phase · 74+ hours logged  
> **Stasis Project:** [View on Stasis](https://stasis.hackclub.com)

---

## Overview

This project combines mechanical engineering, embedded systems, and PCB design into a single flight vehicle. The goal is a rocket capable of flying on H and I class motors with onboard data logging, IMU-based orientation tracking, barometric altitude sensing, and an automated parachute deployment system.

---

## Hardware

### Avionics PCB

The custom PCB is the heart of the avionics bay. Designed from scratch in EasyEDA.

| Component | Role |
|---|---|
| ESP32-S3-WROOM-1 | Main microcontroller |
| TPS62130 | Buck converter — 7.4V → 5V |
| AMS1117-3.3 | LDO regulator — 5V → 3.3V |
| ICM-42688P | 6-axis IMU (SPI) |
| BMP280 | Barometric pressure & temperature (I²C) |
| USB Type-C | Power input & programming |

**Power architecture:** 7.4V LiPo → TPS62130 (5V) → AMS1117 (3.3V)  
**Comms:** SPI for IMU · I²C for barometer · USB-C for flashing

### Rocket Airframe (LOC Forte Kit)

| Part | Purpose |
|---|---|
| LOC Forte Kit | Base airframe — cardboard tube, plywood fins |
| Aeropack 38mm Retainer | Secures motor so it doesn't eject mid-flight |
| 36" Nylon Parachute | Smooth, safe recovery |
| BSI Slow-Cure Epoxy | Structural bonding |
| J-B Weld Epoxy | Miscellaneous reinforcement |

---

## Firmware

Firmware is written for the ESP32-S3 and handles:

- **Quaternion filter** — orientation tracking using IMU data
- **Kalman filter** — sensor fusion for accurate state estimation
- **Deployment logic** — triggers chute release at apogee detection

> The Kalman + quaternion pipeline is still in active development. Currently hitting a quaternion infinity error in simulation — fix in progress.

Hackatime tracked: **60.6h** of firmware time on the `rocketry-pcb` project.

---

## CAD

Full rocket assembly modeled in **Onshape**.

Key design challenges:
- **Nose cone** — Ogive profile for aerodynamic stability, achieved using revolve + sketch workflow
- **Avionics bay** — Sliding mate connector so the PCB sled can be inserted and removed cleanly
- **Fins** — Cutting slots into a cylindrical body tube (turns out, non-trivial)
- **Rail buttons** — Offset mate connectors to mount flush to the body tube exterior

Simulation and stability analysis done in **OpenRocket**.

---

## Build Log

| Session | Hours | What Got Done |
|---|---|---|
| Rocket CAD | 6h | Full Onshape assembly — nose cone, avionics bay, fins, rail buttons |
| PCB Design | 7h | Complete schematic — power tree, USB-C, IMU, barometer, ESP32 |
| OpenRocket Sim + Updates | 0.5h | Flight simulation, general progress update |
| Firmware (Hackatime) | 60.6h | Quaternion filter, Kalman filter, deployment logic |

**Total logged: 74.1h**

---

## Certification Goal

This rocket is being built to achieve **Level 1 High Power Rocketry certification** through NAR or Tripoli. L1 certification allows the holder to purchase and fly H and I class motors independently.

---

## License

MIT — do whatever you want
