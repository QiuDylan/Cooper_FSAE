# Precharge Firmware

The purpose of the ESP32 microcontroller on the precharge board is to control AIR+, AIR-, and precharge relays given input from the HV/LV board.  

**Description of Code**

Microcontroller Inputs 
- HVLV_In: indicates if precharge has completed 
    - Active low signal (when precharge has completed, HVLV_In = 0) 
- RTDS: high if ready to drive, starts precharge if high  

Microcontroller Outputs: IO1, IO2, and IO3 are put into the base of an NPN transistor, so a high value will close the switch, while a low value opens the switch 
- IO1: Controls AIR+ switch 
- IO2: Controls AIR- switch 
- IO3: Controls Precharge Relay 

**ESP32 Board Schematic**

<img src="https://i.imgur.com/lqslIgH.png" width=600 height=400>

 
  