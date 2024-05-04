## Project ideas

- Not LoRa - must be tested by DTU
- Location finder (apple airtag / Tile)
- Bluetooth LE (20m radius tracker)
- Find phone / keys / small things
- "Technically could make an app" - not recommended
- Another "mother"/detection device to find the little 
- Row of LED's
- LCD indicating devices close by
- Webserver: storing the latest received info
- GPS module?
- sigfox has good coverage in Denmark
- BLE for actual location
- Two different protocols: SigFox (idea) global and the BLE
- With sigfox, the coverage becomes greater than just loosing keys in your home, but also in vehicles, atv's etc.
- Downlink/uplink: Have the searchable device beeb
- 3D print case

## Group members

GROUP 4
s232718 Bjarni Arason
s223425 Snaedis
s232720 Dagur Mooney
s204722 Oscar T S
s222593 Simon L
s173931 Sebastian Rydahl

# more notes

## March 14

### pitch

- changing to LoraWAN from SigFox
- could use gps - but takes a lot of power
- accelerometer to see if the object is moving (can be either digital or analog)
- photoresistor (analog) - is the item hidden, in the pocket or out in the open - hinting location if BLE is not helping with the last centimeters

### talking with Martin

- SigFox precision down to 1-4km
- we need 4 gateways 
- alternative is to use GPS
- put it in the report, why and why not we can use sigfox/lora, gps etc
- write all that stuff
- master device on WiFi, for downlink
- Lora to get geolocation, store that on a database accessable by the master device
- use case: find backpack
- Martin: wifi scanning - scan for access points, you can always see the MAC address without connecting to the wifi, mac addresses are stored with geolocation in a database, phones are scanning for the access points, all that info is sent to the database when you use the phone. only power needed is wifi scanning. Indoor purposes, before GPS, "any wifi any mac addresses, no? okay GPS then (geolocation) as a backup" Wifi scanner uses a lot less power than GPS, Lora still uses less.
- 3 protocols? wifi scanner, gnss, ble beacon
- Project wise, wifi scanigFox precision down to 1-4km
- we need 4 gateways 
- alternative is to use GPS
- put it in the report, why and why not we can use sigfox/lora, gps etc
- write all that stuff
- master device on WiFi, for downlink
- Lora to get geolocation, store that on a database accessable by the master device
- use case: find backpack
- Martin: wifi scanning - scan for access points, you can always see the MAC address without connecting to the wifi, mac addresses are stored with geolocation in a database, phones are scanning for the access points, all that info is sent to the database when you use the phone. only power needed is wifi scanning. Indoor purposes, before GPS, "any wifi any mac addresses, no? okay GPS then (geolocation) as a backup" Wifi scanner uses a lot less power than GPS, Lora still uses less.
- 3 protocols? wifi scanner, gnss, ble beacon
- Project wise, wifi scanner more interesting - gps as an addon, depending on motication, will to live
- regarding sensors: Analog - measure the battery using ADC (voltage divider and stuff, put that to the ADC) People have done it, not straight forward.
- Digital sensor: Accelerometer (that could be something)
- Look into sleep mode/stand by mode - that could work well with a accelerometer to wake the device up. Proper (xyz) may be an overkill - tilt switch might do the trick - metal tube when the thing inside moves gives an signal - interrupt (wake up signal). When it moves, send a geolocation
- Cannot use Lora downlink to wake up slave device
- RTC can put the module into standby - every hour or twice a day, send battery status, and geolocation if for example it has not been moved.
- downlink would be the master device
- beacon, waste of power? look into that. activation signal?
- geolocation with the last known location
- nice discussion in the report regarding power - what happens over time power wise? 
- 9V is bad choice. Too high voltage. Find lithium-ion rechargable maybe. 9V is made out of 1.5V batteries - not energy efficient. Primaries, 1.5V. Can we run it on 3V? Maybe not Lora - challenge.
- Have look! Argue the choices in the report, both ideal and non-ideal choices

## March 21

- BLE for local
- WiFi scanning 
- Two different types of uplinks
- Wake up the device, shortly activate the BLE

## March 18

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

## April 11

- Martin suggests an app: for example to use the Blynk.io thing we used previously
- The app would be to help us get into the building, when we are there, the close approximity is needed with the led's 
- what kind of packages - what sort of format - how can we cut the packages, what if we find only two wifi points, will the rest be just dummy data?
- room for enclosure
- energy optimization and battery consumption properly, otherwise it becomes a pretty easy task
