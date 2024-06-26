/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
 
#include "WiFi.h"
#include "LoRaWan_APP.h"
#include "HT_SSD1306Wire.h"
#include "sys/time.h"
#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEBeacon.h>
#include "esp_sleep.h"
#define VBAT_CTRL GPIO_NUM_37
#define VBAT_ADC GPIO_NUM_1
#define PRINT true

#define DEVICE_NAME            "Simon"
#define BEACON_UUID            "e6e9a717-9cc0-4485-8fb1-941f05273c8d"
#define BEACON_DURATION        10 // seconds
#define GPIO_DEEP_SLEEP_DURATION 5        // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;         // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount;  // remember number of boots in RTC Memory

//Here the RTC_DATA_ATTR is defined. Note that if you define a global variable with RTC_DATA_ATTR attribute, the variable will be placed into RTC_SLOW_MEM memory.
//So the structure declared as RTC_DATA_ATTR and copying the dynamic memory to this structure before a deep sleep helps recovering this into dynamic memory after wake up.
//In simple words we are saving the time in static memory from dynamic memory to recover it again after a deep sleep. Here the two variables are defined.
//The ‘last’ is used to get the last time when the ESP32 went to deep sleep and bootcount is used count number of resets.

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
struct timeval now;
//(may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/), we randomly generated one
                                                            //careful, advertised uuid is inversed, bit order is LSB first, meaning that 8d will appear first here
                                                            //#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8)) <-- how it's being done

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* ABP para*/
uint8_t nwkSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint32_t devAddr = (uint32_t)0x0000000;

// battery stuff
const float min_voltage = 3.04;
const float max_voltage = 4.26;
const uint8_t scaled_voltage[100] = {
  254, 242, 230, 227, 223, 219, 215, 213, 210, 207,
  206, 202, 202, 200, 200, 199, 198, 198, 196, 196,
  195, 195, 194, 192, 191, 188, 187, 185, 185, 185,
  183, 182, 180, 179, 178, 175, 175, 174, 172, 171,
  170, 169, 168, 166, 166, 165, 165, 164, 161, 161,
  159, 158, 158, 157, 156, 155, 151, 148, 147, 145,
  143, 142, 140, 140, 136, 132, 130, 130, 129, 126,
  125, 124, 121, 120, 118, 116, 115, 114, 112, 112,
  110, 110, 108, 106, 106, 104, 102, 101, 99, 97,
  94, 90, 81, 80, 76, 73, 66, 52, 32, 7,
};

/*LoraWan channelsmask*/
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 0.5 * 60 * 1000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = true;

RTC_DATA_ATTR bool beaconMode = false;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;


SSD1306Wire factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);  // addr , freq , i2c group , resolution , rst

// battery stuff
float heltec_vbat() {
  pinMode(VBAT_CTRL, OUTPUT);
  digitalWrite(VBAT_CTRL, LOW);
  delay(5);
  float vbat = analogRead(VBAT_ADC) / 238.7;
  // pulled up, no need to drive it
  pinMode(VBAT_CTRL, INPUT);
  return vbat;
}

uint8_t heltec_battery_percent(float vbat = -1) {
  if (vbat == -1) {
    vbat = heltec_vbat();
  }
  for (int n = 0; n < sizeof(scaled_voltage); n++) {
    float step = (max_voltage - min_voltage) / 256;  //0.004765; 1.22/256

    if (vbat > min_voltage + (step * scaled_voltage[n])) {
      float result = min_voltage + (step * scaled_voltage[n]);

      return 100 - n;
    }
  }
  return 0;
}

/* Prepares the payload of the frame */
static void prepareTxFrame(uint8_t port) {
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
	*appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
	*if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
	*if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
	*for example, if use REGION_CN470, 
	*the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
	*/
  appDataSize = 49;
  if(beaconMode) {

  }
  else {
    scanNetworks();
  }
  appData[48] = map(heltec_battery_percent(), 0, 100, 0, 255);
}

RTC_DATA_ATTR bool firstrun = true;

// wifi scanner
bool checkSecondLSB(uint8_t value) {
  // Mask with 0b00000010 (2 in binary) to isolate the second least significant bit
  // Then check if it's equal to 0
  return ((value & 0b00000010) == 0);
}

bool checkIfMatches(uint8_t val1, uint8_t val2, uint8_t val3) {
  // Convert the values to hexadecimal characters
  char hexChars[7];  // 6 characters for "00005E" + 1 for null terminator
  snprintf(hexChars, sizeof(hexChars), "%02X%02X%02X", val1, val2, val3);

  // Compare the string with "00005E"
  if ((strcmp(hexChars, "00005E") == 0) == 1) {
    return true;
  }
}

void scanNetworks() {
  Serial.println("Scan start");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  int addedNetworks = 0;
  Serial.println("Scan done");

  uint8_t macList[8][6];

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH |        MAC        | Encryption");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4d", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2d", WiFi.channel(i));
      Serial.print(" | ");

      //MAC adress sniffing
      //BSSID returns a pointer to the memory location where the BSSID is saved (uint8_t arry)
      //This means that the next memory locations contain the next "packets" of the MAC address

      Serial.printf("%02X%02X%02X%02X%02X%02X", *WiFi.BSSID(i), *(WiFi.BSSID(i) + 1), *(WiFi.BSSID(i) + 2), *(WiFi.BSSID(i) + 3), *(WiFi.BSSID(i) + 4), *(WiFi.BSSID(i) + 5));

      Serial.print(" | ");

      // Check condition 1: Second character must be 0 or 2
      if (checkSecondLSB(*WiFi.BSSID(i)) == true) {
        if (checkIfMatches(*WiFi.BSSID(i), *(WiFi.BSSID(i) + 1), *(WiFi.BSSID(i) + 2) == true)) {
          Serial.print("Passed");
          if (addedNetworks < 8) {
            for (int j = 0; j < 6; j++) {
              macList[addedNetworks][j] = *(WiFi.BSSID(i) + j);
            }
            addedNetworks++;
          }
        }
      }
      Serial.println();
      delay(10);
    }
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 6; j++) {
      appData[i * 6 + j] = macList[i][j];
    }

    Serial.printf("%02X%02X%02X%02X%02X%02X", macList[i][0], macList[i][1], macList[i][2], macList[i][3], macList[i][4], macList[i][5]);
    Serial.println();
  }


  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();
}

// showing the battery stuff
void show_bat(){
  float voltage = heltec_vbat();
  int voltage_percentage = heltec_battery_percent(voltage);
  String strPercentage = String(voltage_percentage);
  String strVoltage = String(voltage, 2);

  // debug
  Serial.println(strVoltage);
  Serial.println(strPercentage);
  float vbat = analogRead(VBAT_ADC);
  Serial.println(vbat);
  
  // show on display
  factory_display.clear();
  factory_display.drawString(0, 0, strVoltage + "V");
  factory_display.drawString(0, 10, strPercentage + "%");
  factory_display.display();
}

//downlink data handle function example
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  Serial.printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
  Serial.print("+REV DATA:");
  for(uint8_t i=0;i<mcpsIndication->BufferSize;i++)
  {
    Serial.printf("%02X",mcpsIndication->Buffer[i]);
    Serial.print("  |  ");
    if (mcpsIndication->Buffer[i] == 49) {
      beaconMode = true;
    }
  }
}

void init_beacon() {
  BLEAdvertising* pAdvertising;
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanFilter(false, true);
  pAdvertising->stop();
  // iBeacon
  BLEBeacon myBeacon;
  myBeacon.setManufacturerId(0xFFFF);
  myBeacon.setMajor(1234);
  myBeacon.setMinor(4321);
  myBeacon.setSignalPower(-58);
  myBeacon.setProximityUUID(BLEUUID(BEACON_UUID));

  BLEAdvertisementData advertisementData;
  advertisementData.setFlags(0x06);
  advertisementData.setManufacturerData(myBeacon.getData());
  pAdvertising->setAdvertisementData(advertisementData);

}

void BLEAdvertise() {
  Serial.println("Address: ");
  Serial.println(WiFi.macAddress());
  if (PRINT) Serial.println("Advertizing started...");
  //if (PRINT) Serial.printf("enter deep sleep\n");
  //esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
  //if (PRINT) Serial.printf("in deep sleep\n");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Running setup");
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  // show battery on display init
  factory_display.init();
  
  BLEDevice::init(DEVICE_NAME);
  init_beacon();
  gettimeofday(&now, NULL);

  if (PRINT) Serial.printf("start ESP32 %d\n", bootcount++);

  if (PRINT) Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n", now.tv_sec, now.tv_sec - last);

  last = now.tv_sec;
  

                // Start advertising
  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");

  if (firstrun) {
    LoRaWAN.displayMcuInit();
    firstrun = false;
  }

  if (beaconMode) {
    BLEAdvertising* pAdvertising;
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();
    BLEAdvertise();
    unsigned long tempTime = millis();
    while (millis()-tempTime < BEACON_DURATION*1000) {}
    beaconMode = false; 
    pAdvertising->stop();
  }
}

void loop() {
  switch (deviceState) {
    case DEVICE_STATE_INIT:
      {
        #if (LORAWAN_DEVEUI_AUTO)
                LoRaWAN.generateDeveuiByChipID();
        #endif
        LoRaWAN.init(loraWanClass, loraWanRegion);
        //both set join DR and DR when ADR off
        LoRaWAN.setDefaultDR(3);
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        //show_bat();
        LoRaWAN.displayJoining();
        LoRaWAN.join();
        break;
      }
    case DEVICE_STATE_SEND:
      {
        //show_bat();
        LoRaWAN.displaySending();
        prepareTxFrame(appPort);
        LoRaWAN.send();
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
        //show_bat();
        LoRaWAN.displayAck();
        LoRaWAN.sleep(loraWanClass);
        break;
      }
    default:
      {
        deviceState = DEVICE_STATE_INIT;
        break;
      }
  }
}
