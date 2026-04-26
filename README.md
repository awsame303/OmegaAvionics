# Omega Avionics — L1 Certification Rocket

<img width="1119" height="426" alt="Screenshot 2026-04-14 at 5 32 18 PM" src="https://github.com/user-attachments/assets/ca16155c-7785-469d-9017-6db219f035bc" />


A custom-built high-power rocket with a fully designed avionics bay, dual-deployment chute release system, and custom PCB — built to earn an **NAR/Tripoli L1 certification** (H & I class motors) :O

> **Stasis Project:** [View on Stasis](https://stasis.hackclub.com)


## Overview

This project combines mechanical engineering, embedded systems, and PCB design into a single flight vehicle. The goal is a rocket capable of flying on H and I class motors with onboard data logging, IMU-based orientation tracking, barometric altitude sensing, and an automated parachute deployment system. I made it because I wanted to expand my love for rocketry and actually learn a lot about data collection, how to make things that can survive high stress environments, and how to use fillets and other building related things. My ultimate goal for this project is to make a cool rocket that flighs high but also gives me valuable data that I can learn a lot about engineering and aerospace from. 


## Hardware

### Avionics PCB

<img width="968" height="410" alt="image" src="https://github.com/user-attachments/assets/d2500769-26f7-4729-8372-586a7c94e16e" />

The custom PCB is the heart of the avionics bay, and I Designed it! from scratch in EasyEDA.


## Firmware

Firmware is written for the ESP32-S3 


## CAD

Full rocket assembly modeled in Onshape

Simulation and stability analysis done in Openrocket

<img width="1405" height="332" alt="image" src="https://github.com/user-attachments/assets/382ac787-cd55-4763-b731-5b9cbfcac9e1" />


## Certification Goal

This rocket is being built to achieve JKUNIOR Level One High Power Cert through NAR or Tripoli. L1 certification allows ME to purchase and fly H and I class motors independently.


## License

MIT — do whatever u want w it

| Item / Part | Designator | Description / Purpose | Qty | Price | Source |
| :--- | :--- | :--- | :--- | :--- | :--- |
| AMS1117-3.3_C22466222 | U3 | Voltage regulator | 1 | $0.00 | LCSC |
| UBEC | RELE1 | Power module | 1 | $0.00 | LCSC |
| TPS62130RGTRF0 | U1 | Step-down converter | 1 | $0.00 | LCSC |
| ESP32-S3-WROOM-1(N8R2) | U4 | Main microcontroller | 1 | $0.00 | LCSC |
| 2.2uH | U2 | Power inductor | 1 | $0.00 | LCSC |
| 19-217/GHC-YR1S2/3T | LED1 | Status indicator | 1 | $0.00 | LCSC |
| TYPE-C 16PIN 2MD(073) | USB1 | USB interface | 1 | $0.00 | LCSC |
| BMP280 | U6 | Altimeter/Pressure sensor | 1 | $0.00 | LCSC |
| LED-0603_G | LED3 | Status indicator | 1 | $0.00 | LCSC |
| 2700Hz | BUZZER1 | Audible alert | 1 | $0.00 | LCSC |
| 1N4148W | D2 | Signal diode | 1 | $0.00 | LCSC |
| KH-6X6X4.3H-TJ | SW1 | Tactile switch | 1 | $0.00 | LCSC |
| D-MMBT2222A-7-F | U8 | NPN Transistor | 1 | $0.00 | LCSC |
| 470uF | C1 | Electrolytic capacitor | 1 | $0.00 | LCSC |
| 1N5819HW | D1 | Schottky diode | 1 | $0.00 | LCSC |
| Apogee 12in Parachute Protector | | For protecting the parachute from the ejection charge | 1 | $9.66 | Apogee Rocketry |
| emma kites 100% Kevlar Braided String 100Lb | | To attach the rocket to the parachute | 1 | $14.95 | Amazon |
| sprookber Stainless Steel Chain Quick Links | | To attach the parachute cord to the rocket | 1 | $8.99 | Amazon |
| Aero Pack 29/38mm Motor Adapter | | To mount mid power motors (G, F) before testing H class | 1 | $31.85 | Apogee Rocketry |
| 36" Printed Nylon Parachute | | To ensure smooth landing and protect electronics | 1 | $18.69 | Apogee Rocketry |
| Aero Pack 38mm Retainer | | Secures rocket motors during flight | 1 | $30.63 | Apogee Rocketry |
| Bob Smith BSI-205 Slow-Cure Epoxy (30-Min) | | Strong adhesive for structural components | 1 | $13.69 | Amazon |
| J-B Weld 8265S Original Cold-Weld | | Adhesive for miscellaneous parts | 1 | $6.54 | Amazon |
| LOC Forte Kit | | Primary airframe and basic rocket components | 1 | $95.97 | LOC Precision |
