/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
#include "WiFi.h"
#include "LoRaWan_APP.h"
#include "HT_SSD1306Wire.h"
#define VBAT_CTRL GPIO_NUM_37
#define VBAT_ADC GPIO_NUM_1

uint8_t devEui[] = { 0x60, 0x81, 0xF9, 0xB5, 0xF5, 0xE0, 0xBE, 0x29 };
uint8_t appEui[] = { 0x60, 0x81, 0xF9, 0x8F, 0x0D, 0x52, 0x3E, 0xDC };
uint8_t appKey[] = { 0x79, 0xE4, 0xD8, 0x74, 0x98, 0xA4, 0xB8, 0xF3, 0x21, 0x78, 0x36, 0x76, 0xAD, 0xAD, 0x3D, 0xCE };

/* ABP para*/
uint8_t nwkSKey[] = { 0x0B, 0xD2, 0x2E, 0x3C, 0x93, 0xAA, 0x59, 0xF3, 0x67, 0x85, 0x82, 0xDE, 0x0D, 0x13, 0xBA, 0xBC };
uint8_t appSKey[] = { 0xDE, 0x29, 0x8F, 0xC4, 0x1D, 0x1C, 0x83, 0x2E, 0xCF, 0x5C, 0x96, 0xC2, 0x87, 0x73, 0xB0, 0x3B };
uint32_t devAddr = (uint32_t)0x008ae0d;

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

int heltec_battery_percent(float vbat = -1) {
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
  scanNetworks();
  appData[48] = heltec_battery_percent();
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
      Serial.print("Go beacon mode bro");
    }
    else if (mcpsIndication->Buffer[i] == 48) {
      Serial.println("Cease the beacon sis");
    }
  }
  Serial.println();
  uint32_t color=mcpsIndication->Buffer[0]<<16|mcpsIndication->Buffer[1]<<8|mcpsIndication->Buffer[2];
#if(LoraWan_RGB==1)
  turnOnRGB(color,5000);
  turnOffRGB();
#endif
}

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  // show battery on display init
  factory_display.init();
  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");

  if (firstrun) {
    LoRaWAN.displayMcuInit();
    firstrun = false;
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
  // Wait a bit before scanning again.
  //delay(10000);
}
