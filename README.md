# ESPVolumeControl - A simple way to control your the Volume of OpenHAB2-Items

Here I created a sketch that is written in the Arduino IDE for ESP8266 to control your speakers via the REST interface of OpenHAB2. The Input will be from a rotary encoder and a few buttons.

Great, now why is everything so weird here?
---

Well since this is my first time publishing any of my work online and working with github I'm still learning how to use everything - so if I'm doing anything wrong just let me know! Any help will be appreciated!

The purpose of this repository is not only to share my code, but also for me to learn how to use github.


Features
---
- Control Volume of speakers with rotary encoder (Or any dimmer Item in OpenHAB2)

What do I need?
---
Software:

- The WiFiManager library from tzapu
- A working OpenHab2 installation 
 
Hardware:
- ESP8266, NodeMCU or similar
- KC-040 rotary encoder

How to use
---
In the Arduino IDE go the top section that is marked "User Settings" and set the variables to match your setup. Then simply upload it to your ESP8266.

The first time in an unknown WiFi the device will start an access point where the WiFi credentials need to be entered and saved (see tzapu/WiFiManager for a more detailed explanation).

What's next?
---
- Next/Previous Buttons
- Play/Pause Button
- maybe a Mute Button
- OLED-display to display artist and track info
- Configuration website so settings don't have to be hard coded to the device