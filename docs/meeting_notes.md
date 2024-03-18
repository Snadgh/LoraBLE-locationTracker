# Meeting notes

## Meeting 18 March

- updating Sebastian
- network scanning
- online databases with location
- [precisely](https://www.precisely.com/)
- Problems with MAC addresses, each network has it's own? Hardware or WiFi's MAC address?

### For today:

- delegate tasks
- make github (snaedis)
- battery (bjarni and dagur)
- BLE (Simon)
- Tilt sensor: save the previous state, have on interrupt pin, touch pin
- Platform IO recommended: pseudo code [platformio](https://platformio.org/)
- Display: updated coordinates on text - location finding mode
- Switch: To turn on and off the module
- RTC: ESP's built-in RTC, deep sleep uses almost no power (eeeven smaller)
- WiFi scanning
- CAD not priority at the moment
- PCD also not a priority


### Split up for the two devices: Controller and slave device

- Both need uplink and downlink
- Toggle switch only on master
- LEDs only on master
- Battery system for both (and voltage divider for the slave)

### More notes

- Master device downlink using WiFi
- Slave device uplink using Lora 
- Microsoft Azure often used as backend
- IoT Prototyping used something else
- Helium sends stuff to Azure
