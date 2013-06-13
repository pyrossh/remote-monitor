Remote Monitoring and Control using GSM-SMS
=============
The primary aim of this project is to propose the concept of Development of a Low-Cost GSM-SMS Based Temperature Remote
Monitoring and Control system for Industrial Applications using the combination of a Teensy 3.0 and a GSM Modem SIM300

Check out [Arduino](www.pjrc.com/store/teensy3.html)  
Check out [Teensy 3.0](www.arduino.cc/)

Requirements
------------
1. Teensy 3.0
2. SIM 300
3. LM35
4. SHT75
5. Solid State Relay
6. Bulb
7. Buzzer
8. LED's
9. USB Power Supply


Features
--------
- Temperature Monitoring and Control using LM35
- Humidity Monitoring using SHT75
- AUTOMATIC MODE
  - SETPOINT
- MANUAL MODE
  - ON
  - OFF

OPcodes
---------
- TEMP
- HUM
- SET
- MAN
- AUTO
- ON
- OFF
- MOB

Block Diagram
-------------
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/block.png" width="480" height="320">

Schematic
---------
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/remotemonitor.png" width="480" height="320">

Pin Configuration
-----------------
Pin No.|Connection
--- | ---
VIN | +5V
AGND| GND
9   | TX
10  | RX
11  | BTN1
12  | BTN2
14  | LED1 (BLUE)
15  | LED2 (RED)
16  | LED2 (GREEN)
17  | LM35 Output
19  | Relay Input
20  | SHT75 - DATA
21  | SHT75 - CLK
23  | Transistor Input

Flow Chart
----------
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/flow1.png" width="480" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/flow2.png" width="480" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/flow3.png" width="480" height="320">

Snapshots
---------
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/snap.png" width="480" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/IMG_20130609_195037.jpg" width="480" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor//master/IMG_20130609_195751.jpg" width="480" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor/master/IMG_20130609_195739.jpg" width="480" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor/master/Screenshot_2013-06-07-20-33-12.png" width="240" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor/master/Screenshot_2013-06-07-20-33-31.png" width="240" height="320">
<img src = "https://raw.github.com/Pyros2097/RemoteMonitor/master/Screenshot_2013-06-09-21-22-02.png" width="240" height="320">

Video
-----
