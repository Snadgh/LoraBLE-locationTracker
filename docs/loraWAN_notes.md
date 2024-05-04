# Starting with ESP32 and RN2483

Temporarily using RN2483 module until using the LoraWAN microcontroller.

## Software and documents:

- Account on [Helium LoraWAN network](https://console.helium.com/login)
- LoraWAN network coverage. See coverage map [here](https://explorer.helium.com)
- [Microchip RN2483 datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/50002346C.pdf)
- [Microchip RN2483 command reference user’s guide](https://ww1.microchip.com/downloads/en/DeviceDoc/RN2483-LoRa-Technology-Module-Command-Reference-User-Guide-DS40001784G.pdf)


## Hardware


| ESP32 | RN2483 |
| --- | --- |
| RX2 | Tx |
| TX2 | Rx |
| D23 | RST |
| 3V3 | 3V3 |
| GND | GND |

Using appropriate jumper wires to connect the two.

## Initial setup RN2483

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


## Connecting to Helium using RN2483

- code directory: `rn2483-lora-helium`

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
- According to console: sometimes works - have not seen it work on monitor
- Try either Copenhagen, or by hotspot: Handsome Saffron Hippo

# Using the Heltec LoRa 32 V3 microcontroller


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

Installing the library and board:
```
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

### Testing with Heltec and Helium

- code directory: `heltec-lora-official-library`

Result, trying LoRaWAN example and LoRaWAN-OLED example: 

```
$ arduino-cli compile *.ino && arduino-cli upload *.ino && arduino-cli monitor -p /dev/ttyUSB0 -c 115200
/home/snadgh/git/LoraBLE-locationTracker/slave/LoRaWan/LoRaWan.ino: In function 'void setup()':
/home/snadgh/git/LoraBLE-locationTracker/slave/LoRaWan/LoRaWan.ino:94:13: error: 'HELTEC_BOARD' was not declared in this scope
   Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
             ^~~~~~~~~~~~
/home/snadgh/git/LoraBLE-locationTracker/slave/LoRaWan/LoRaWan.ino:94:26: error: 'SLOW_CLK_TPYE' was not declared in this scope
   Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
                          ^~~~~~~~~~~~~
```

Fix: chose the correct board - not `esp32` but the from the official `heltec` library. 

## Testing with Heltec and CIBICOM

- code directory: `heltec-lora-cibicom`

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

Fails with unofficial library.

```
Radio init
[RadioLib] radio.begin() returned 0 (ERR_NONE)
Joining
[RadioLib] node.beginOTAA(joinEUI, devEUI, nwkKey, appKey) returned -6 (ERR_RX_TIMEOUT)
Next TX in 10 s
Deep sleep in 5 s
```

Works with official library.

## Testing with RN2483 and CIBICOM

- code directory: `rn2483-lora-cibicom`

Works with RN2483 and the code from week 5.


# Other notes

- Heltec: Soldered for HF -> `REGION_EU868 ✔ LORAWAN_REGION=0`

```
$ arduino-cli board details -b esp32:esp32:heltec_wifi_lora_32_V3
Board name:            Heltec WiFi LoRa 32(V3) / Wireless shell(V3) / Wireless stick lite (V3)
FQBN:                  esp32:esp32:heltec_wifi_lora_32_V3
Board version:         2.0.11

Package name:          esp32
Package maintainer:    Espressif Systems
Package URL:           https://dl.espressif.com/dl/package_esp32_index.json
Package website:       https://github.com/espressif/arduino-esp32
Package online help:   http://esp32.com

Platform name:         esp32
Platform category:     ESP32
Platform architecture: esp32
Platform URL:          https://github.com/espressif/arduino-esp32/releases/download/2.0.11/esp32-2.0.11.zip
Platform file name:    esp32-2.0.11.zip
Platform size (bytes): 250401265
Platform checksum:     SHA-256:d15386308dc72f94816ce80b5508af999f2fd0d88eb5e1ffba48316ab0b9c5d6

Required tool: arduino:dfu-util                  0.11.0-arduino5
Required tool: esp32:esptool_py                  4.5.1
Required tool: esp32:mklittlefs                  3.0.0-gnu12-dc7f933
Required tool: esp32:mkspiffs                    0.2.3
Required tool: esp32:openocd-esp32               v0.11.0-esp32-20221026
Required tool: esp32:riscv32-esp-elf-gcc         esp-2021r2-patch5-8.4.0
Required tool: esp32:riscv32-esp-elf-gdb         11.2_20220823
Required tool: esp32:xtensa-esp-elf-gdb          11.2_20220823
Required tool: esp32:xtensa-esp32-elf-gcc        esp-2021r2-patch5-8.4.0
Required tool: esp32:xtensa-esp32s2-elf-gcc      esp-2021r2-patch5-8.4.0
Required tool: esp32:xtensa-esp32s3-elf-gcc      esp-2021r2-patch5-8.4.0

Option:        Upload Speed                                              UploadSpeed
               921600                            ✔                       UploadSpeed=921600
               115200                                                    UploadSpeed=115200
               230400                                                    UploadSpeed=230400
               460800                                                    UploadSpeed=460800
Option:        CPU Frequency                                             CPUFreq
               240MHz (WiFi)                     ✔                       CPUFreq=240
               160MHz (WiFi)                                             CPUFreq=160
               80MHz (WiFi)                                              CPUFreq=80
               40MHz                                                     CPUFreq=40
               20MHz                                                     CPUFreq=20
               10MHz                                                     CPUFreq=10
Option:        Core Debug Level                                          DebugLevel
               None                              ✔                       DebugLevel=none
               Error                                                     DebugLevel=error
               Warn                                                      DebugLevel=warn
               Info                                                      DebugLevel=info
               Debug                                                     DebugLevel=debug
               Verbose                                                   DebugLevel=verbose
Option:        Arduino Runs On                                           LoopCore
               Core 1                            ✔                       LoopCore=1
               Core 0                                                    LoopCore=0
Option:        Events Run On                                             EventsCore
               Core 1                            ✔                       EventsCore=1
               Core 0                                                    EventsCore=0
Option:        Erase All Flash Before Sketch Upload                         EraseFlash
               Disabled                          ✔                       EraseFlash=none
               Enabled                                                   EraseFlash=all
Option:        LoRaWan Region                                            LORAWAN_REGION
               REGION_EU868                      ✔                       LORAWAN_REGION=0
               REGION_EU433                                              LORAWAN_REGION=1
               REGION_CN470                                              LORAWAN_REGION=2
               REGION_US915                                              LORAWAN_REGION=3
               REGION_AU915                                              LORAWAN_REGION=4
               REGION_CN779                                              LORAWAN_REGION=5
               REGION_AS923                                              LORAWAN_REGION=6
               REGION_KR920                                              LORAWAN_REGION=7
               REGION_IN865                                              LORAWAN_REGION=8
               REGION_US915_HYBRID                                       LORAWAN_REGION=9
Option:        LoRaWan Debug Level                                       LoRaWanDebugLevel
               None                              ✔                       LoRaWanDebugLevel=0
               Freq                                                      LoRaWanDebugLevel=1
               Freq && DIO                                               LoRaWanDebugLevel=2
               Freq && DIO && PW                                         LoRaWanDebugLevel=3
Option:        LoRaWan DevEUI                                            LORAWAN_DEVEUI
               CUSTOM                            ✔                       LORAWAN_DEVEUI=0
               Generate By ChipID                                        LORAWAN_DEVEUI=1
Option:        LoRaWan Preamble Length                                   LORAWAN_PREAMBLE_LENGTH
               8(default)                        ✔                       LORAWAN_PREAMBLE_LENGTH=0
               16(For M00 and M00L)                                      LORAWAN_PREAMBLE_LENGTH=1
Programmers:   ID                                Name
               esptool                           Esptool
```

Result: Wrong board selected. Use the official Heltec one.

# Testing again Monday Apr 22 

Pick the correct board for the official library:

```yaml
default_fqbn: Heltec-esp32:esp32:heltec_wifi_lora_32_V3
default_port: /dev/ttyUSB0
```

Now it compiles like a charm.

## Cibicom and Heltec 

```
❯ arduino-cli monitor -p /dev/ttyUSB0 -c 115200
Monitor port settings:
baudrate=115200
Connected to /dev/ttyUSB0! Press CTRL-C to exit.
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3818,len:0x508
load:0x403c9700,len:0x4
load:0x403c9704,len:0xad0
load:0x403cc700,len:0x29d8
entry 0x403c9880

LoRaWAN EU868 Class A start!

+OTAA=1
+Class=A
+ADR=1
+IsTxConfirmed=1
+AppPort=2
+DutyCycle=15000
+ConfirmedNbTrials=4
+ChMask=0000000000000000000000FF
+DevEui=BE7A00000000027D(For OTAA Mode)
+AppEui=BE7A000000001465(For OTAA Mode)
+AppKey=2D0F4D3D8293015A7027F5564C063AD0(For OTAA Mode)
+NwkSKey=0BD22E3C93AA59F3678582DE0D13BABC(For ABP Mode)
+AppSKey=DE298FC41D1C832ECF5C96C28773B03B(For ABP Mode)
+DevAddr=0008AE0D(For ABP Mode)


joining...
joined
confirmed uplink sending ...
confirmed uplink sending ...
confirmed uplink sending ...
```

Works - screenshot taken

## Helium and Heltec 

```
Connected to /dev/ttyUSB0! Press CTRL-C to exit.

LoRaWAN EU868 Class A start!

+OTAA=1
+Class=A
+ADR=1
+IsTxConfirmed=1
+AppPort=2
+DutyCycle=15000
+ConfirmedNbTrials=4
+ChMask=0000000000000000000000FF
+DevEui=6081F9CB3A8A03F9(For OTAA Mode)
+AppEui=6081F98F0D523EDC(For OTAA Mode)
+AppKey=B30D4ADC880A9D73A3602E84100876E7(For OTAA Mode)
+NwkSKey=0BD22E3C93AA59F3678582DE0D13BABC(For ABP Mode)
+AppSKey=DE298FC41D1C832ECF5C96C28773B03B(For ABP Mode)
+DevAddr=0008AE0D(For ABP Mode)


joining...
```

The boys took a walk and got connection a little further than Netto.







