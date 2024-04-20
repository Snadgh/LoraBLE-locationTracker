## Software and documents:

- Account on [Helium LoraWAN network](https://console.helium.com/login)
- LoraWAN network coverage. See coverage map [here](https://explorer.helium.com)
- [Microchip RN2483 datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/50002346C.pdf)
- [Microchip RN2483 command reference user’s guide](https://ww1.microchip.com/downloads/en/DeviceDoc/RN2483-LoRa-Technology-Module-Command-Reference-User-Guide-DS40001784G.pdf)


## Hardware

Temporarily using RN2483 module until using the LoraWAN microcontroller.

| ESP32 | RN2483 |
| --- | --- |
| RX2 | Tx |
| TX2 | Rx |
| D23 | RST |
| 3V3 | 3V3 |
| GND | GND |

Using appropriate jumper wires to connect the two.

## Initial setup

- Add the board manager to `~/.arduino15/arduino-cli.yaml`:

```yaml
board_manager:
additional_urls:
- http://arduino.esp8266.com/stable/package_esp8266com_index.json
- https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
- https://dl.espressif.com/dl/package_esp32_index.json
```

- Add the library `https://github.com/jpmeijers/RN2483-Arduino-Library` using the guide for [arduino-cli](https://arduino.github.io/arduino-cli/1.36/commands/arduino-cli_lib_install/) or [Arduino IDE](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries/):

```
$ arduino-cli lib install --git-url https://github.com/jpmeijers/RN2483-Arduino-Library
--git-url and --zip-path flags allow installing untrusted files, use it at your own risk.
Enumerating objects: 45, done.
Counting objects: 100% (45/45), done.
Compressing objects: 100% (41/41), done.
Total 45 (delta 11), reused 17 (delta 1), pack-reused 0
Library installed
```

- Check:

```
$ arduino-cli lib list | grep RN
RN2xx3 Arduino Library  1.0.1       -         LIBRARY_LOCATION_USER -
$ arduino-cli lib examples | grep RN2xx3
Examples for library RN2xx3 Arduino Library
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/ArduinoUnoNano-basic
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/ArduinoUnoNano-downlink
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/ESP8266-RN2483-basic
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/SodaqAutonomo-basic
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/SodaqOne-TTN-Mapper-ascii
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/SodaqOne-TTN-Mapper-binary
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/TheThingsUno-basic
  - /home/snadgh/Arduino/libraries/RN2xx3_Arduino_Library/examples/TheThingsUno-GPSshield-TTN-Mapper-binary

```

- Connect the hardware if not done already (*NOTE:* Make sure the antenna is connected before applying voltage to the board) 


## Connecting to Helium

- Use the edited script from lecture 5: `ESP8266-RN2483-basic.ino`

- Attach the board: (*Note:* Might change with the different board)

```
$ arduino-cli board attach -p /dev/ttyUSB0 -b esp32:esp32:esp32doit-devkit-v1 *.ino
Default port set to: /dev/ttyUSB0
Default FQBN set to: esp32:esp32:esp32doit-devkit-v1
```

- Run the code and note down the `DevEUI`:

```
When using OTAA, register this DevEUI: 
0004A30B00F2142F
RN2xx3 firmware version:
RN2483 1.0.5 Oct 31 2018 15:06:52
Trying to join Helium 
```

- Make an account and log into **Helium**: [Helium](https://console.helium.com/login)
- Add a new device: [New Device](https://console.helium.com/devices/new)

![New device added](../figures/helium-new-device.png)

![Device details](../figures/helium-device-details.png)

```
App EUI: 6081F98F0D523EDC
App key: 7CF34F4A3DC9A6C611837C5EBD97BBB6
```

### Testing the coverage 

```
When using OTAA, register this DevEUI: 
0004A30B00F2142F
RN2xx3 firmware version:
RN2483 1.0.5 Oct 31 2018 15:06:52
Trying to join Helium
Unable to join. Are your keys correct, and do you have Helium coverage?
```

![Helium Coverage Lyngby](../figures/helium-coverage-lyngby.png)

- Next time, try in the covered spot in Lyngby. Classroom not working.

## Using the Heltec microcontroller

[Heltec quick start](https://docs.heltec.org/en/node/esp32/esp32_general_docs/quick_start.html)

Install the last ESP32 package URL: https://resource.heltec.cn/download/package_heltec_esp32_index.json


`.arduino15/arduino-cli.yaml`:
```yaml
board_manager:
    additional_urls:
    - http://arduino.esp8266.com/stable/package_esp8266com_index.json
    - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
    - https://dl.espressif.com/dl/package_esp32_index.json
    - https://resource.heltec.cn/download/package_heltec_esp32_index.json
```

```
$ arduino-cli core update-index
Downloading index: package_index.tar.bz2 downloaded
Downloading index: package_esp8266com_index.json downloaded
Downloading index: package_esp32_index.json downloaded
Downloading index: package_esp32_index.json downloaded
Downloading index: package_heltec_esp32_index.json downloaded
```

Not needed, the board was already in the ESP32 package from previous exercises of the course:

```
$ arduino-cli board listall heltec
Board Name                                                              FQBN
Heltec WiFi Kit 32                                                      esp32:esp32:heltec_wifi_kit_32
Heltec WiFi Kit 32(V3)                                                  esp32:esp32:heltec_wifi_kit_32_V3
Heltec WiFi LoRa 32                                                     esp32:esp32:heltec_wifi_lora_32
Heltec WiFi LoRa 32(V2)                                                 esp32:esp32:heltec_wifi_lora_32_V2
Heltec WiFi LoRa 32(V3) / Wireless shell(V3) / Wireless stick lite (V3) esp32:esp32:heltec_wifi_lora_32_V3
Heltec Wireless Stick                                                   esp32:esp32:heltec_wireless_stick
Heltec Wireless Stick Lite                                              esp32:esp32:heltec_wireless_stick_lite

$ arduino-cli lib search heltec | grep Name:
Name: "Heltec ESP32 Dev-Boards"
Name: "Heltec ESP8266 Dev-Boards"
Name: "Heltec_ESP32_LoRa_v3"
Name: "TTN_esp32"
Name: "heltec-eink-modules"

$ arduino-cli lib search "Heltec ESP32 Dev-Boards"
Name: "Heltec ESP32 Dev-Boards"
  Author: Heltec Automation
  Maintainer: HelTec <support@heltec.cn>
  Sentence: Library for Heltec ESP32 (or ESP32+LoRa) based boards
  Paragraph: Includes: WiFi Kit 32, WiFi LoRa 32, Wireless Stick, Wireless Shell, see more on http://heltec.cn
  Website: https://github.com/HelTecAutomation/Heltec_ESP32.git
  Category: Device Control
  Architecture: esp32
  Types: Contributed
  Versions: [1.0.2, 1.0.3, 1.0.4, 1.0.5, 1.0.6, 1.0.7, 1.0.8, 1.0.9, 1.1.0, 1.1.1, 1.1.2, 1.1.5, 2.0.1]
  Provides includes: heltec.h

$ arduino-cli lib install "Heltec ESP32 Dev-Boards"
Downloading Heltec ESP32 Dev-Boards@2.0.1...
Heltec ESP32 Dev-Boards@2.0.1 downloaded
Installing Heltec ESP32 Dev-Boards@2.0.1...
Installed Heltec ESP32 Dev-Boards@2.0.1
```

Result, trying LoRaWAN example: 
```
❯ arduino-cli compile *.ino && arduino-cli upload *.ino && arduino-cli monitor -p /dev/ttyUSB0 -c 115200
/home/snadgh/git/LoraBLE-locationTracker/slave/LoRaWan/LoRaWan.ino: In function 'void setup()':
/home/snadgh/git/LoraBLE-locationTracker/slave/LoRaWan/LoRaWan.ino:94:13: error: 'HELTEC_BOARD' was not declared in this scope
   Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
             ^~~~~~~~~~~~
/home/snadgh/git/LoraBLE-locationTracker/slave/LoRaWan/LoRaWan.ino:94:26: error: 'SLOW_CLK_TPYE' was not declared in this scope
   Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
                          ^~~~~~~~~~~~~


Used library            Version Path
Heltec ESP32 Dev-Boards 2.0.1   /home/snadgh/Arduino/libraries/Heltec_ESP32_Dev-Boards
SPI                     2.0.0   /home/snadgh/.arduino15/packages/esp32/hardware/esp32/2.0.11/libraries/SPI
Wire                    2.0.0   /home/snadgh/.arduino15/packages/esp32/hardware/esp32/2.0.11/libraries/Wire

Used platform Version Path
esp32:esp32   2.0.11  /home/snadgh/.arduino15/packages/esp32/hardware/esp32/2.0.11
Error during build: exit status 1
```

### Testing unoffical board

[github unofficial Heltec library](https://github.com/ropg/Heltec_ESP32_LoRa_v3)

```
$ arduino-cli lib search Heltec_ESP_32_LoRa_v3
Downloading index: library_index.tar.bz2 downloaded                                                    
Name: "Heltec_ESP32_LoRa_v3"
  Author: Rop Gonggrijp <rop@gonggri.jp>
  Maintainer: Rop Gonggrijp <rop@gonggri.jp>
  Sentence: Proper working library for "Heltec ESP32 LoRa v3" and "Heltec Wireless Stick v3" boards.
  Paragraph: No more frustration, no more puzzling it all together. Everything works. Uses RadioLib.
  Website: https://github.com/ropg/Heltec_ESP32_LoRa_v3
  Category: Device Control
  Architecture: esp32
  Types: Contributed
  Versions: [0.2.0, 0.3.0, 0.3.1, 0.3.2, 0.3.3, 0.3.4, 0.4.0, 0.5.0]
  Provides includes: heltec.h
  Dependencies: HotButton

$ arduino-cli lib install Heltec_ESP32_LoRa_v3
Downloading Heltec_ESP32_LoRa_v3@0.5.0...
Heltec_ESP32_LoRa_v3@0.5.0 downloaded
Installing Heltec_ESP32_LoRa_v3@0.5.0...
Installed Heltec_ESP32_LoRa_v3@0.5.0
Downloading HotButton@0.1.1...
HotButton@0.1.1 downloaded
Installing HotButton@0.1.1...
Installed HotButton@0.1.1
```

Works, it's cool. But does it include Lora stuff?

Btw, both libraries are named `heltec.h`, uninstall the other to try this one, and vice versa.

```
$ arduino-cli lib uninstall "Heltec ESP32 Dev-Boards"
Uninstalling [Heltec ESP32 Dev-Boards@2.0.1]...
$ arduino-cli lib install Heltec_ESP32_LoRa_v3
Already installed HotButton@0.1.1
Downloading Heltec_ESP32_LoRa_v3@0.5.0...
Heltec_ESP32_LoRa_v3@0.5.0 Heltec_ESP32_LoRa_v3@0.5.0 already downloaded
Installing Heltec_ESP32_LoRa_v3@0.5.0...
Installed Heltec_ESP32_LoRa_v3@0.5.0
```

Results fails:
```
Radio init
[RadioLib] radio.begin() returned 0 (ERR_NONE)
Joining
[RadioLib] node.beginOTAA(joinEUI, devEUI, nwkKey, appKey) returned -6 (ERR_RX_TIMEOUT)
Next TX in 10 s
Deep sleep in 5 s
```

## Testing with Heltec and CIBICOM

[CIBICOM](https://iotnet.teracom.dk/login)

```
Name      BE-7A-00-00-00-00-02-7D
EUI       BE7A00000000027D
JoinEUI   BE7A000000001465
DevAddr   00C8AE0D
Appkey    2D0F4D3D8293015A7027F5564C063AD0
NwkSKey   0BD22E3C93AA59F3678582DE0D13BABC
AppSKey   DE298FC41D1C832ECF5C96C28773B03B
```

Same thing, fails...

Works with RN2483 and the code from week 5.


## Testing with 

[RadioLib](https://github.com/ropg/heltec_esp32_lora_v3?tab=readme-ov-file#radiolib) [notes](https://github.com/jgromes/RadioLib/blob/master/examples/LoRaWAN/LoRaWAN_Starter/notes.md)










