# YA-M5StickC-Plus-Nixie-Watch

Here is Yet Another Nixie Watch that takes advantage of more of the M5StickC-Plus features, programmed with UIFlow:
- Extend battery life by turning the M5StickC-Plus off. The M5StickC-Plus battery is so tiny one cannot expect more than a couple hours of battery life, even when using wait() or machine.lightsleep(). Calling machine.deepsleep() from UIFlow will lock up the device. So the best way to deal with this is to just turn the M5StickC-Plus off between clock viewings, and try to make it boot up as quickly as possible. 
- Adjustable brightess and TZ offset, saved in EEPROM.
- Sync RTC to NTP only on demand, so device can be taken out of range of Wifi, and to save power.
- Watch stays on when charging.
- Battery indicator.
- BOM: M5StickC-Plus with the watch band kit from M5Stack.

![Image of wristwatch in action](wrist.jpg)

## Installation and Operation
- Import the .m5f file and touch up the code with your Wifi SSID (no spaces), password, standard and savings time timezones (or any two TZs), adjust text messages for TZ and your language, etc.
- Upload the 60x60 pixel image files [0-9].jpg in this repo to UIFlow using the MANAGER. UIFlow will place them in a directory on the UIFlow filesystem called "res".
- SPECIAL INSTALLATION WORKAROUND! UiFlow puts the call to wifiCfg.doConnect() at the top of the generated python regardless of where you place the block in UIFlow. (Bug? Feature?) After creating the program, you need to manually edit the generated python before you upload it. Move the wifiCfg.doConnect() call just below the CONNECT text message in the multiBtnCb_AB function. See comments in "generated_python.py". 
- Upload the generated python to the M5StickC-Plus in "App Mode". Turn off the boot menu delay for extra speed in booting.
- Operation:
-- A Button: Adjust brightness
-- B Button: Switch back and forth between standard and daylight saving timezones.
-- A+B Button: Sync RTC to NTP. (You won't need to do this often - the M5StickC-Plus RTC seems fairly resistant to drift.)
