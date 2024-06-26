/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
#include "WiFi.h"
#include "LoRaWan_APP.h"

uint8_t devEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; 
uint8_t appKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* ABP para*/
uint8_t nwkSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint32_t devAddr = (uint32_t)0x0000000;

/*LoraWan channelsmask*/
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 3*60*1000;

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
  appData[48] = 0xff;

}

RTC_DATA_ATTR bool firstrun = true;


bool checkSecondLSB(uint8_t value) {
  // Mask with 0b00000010 (2 in binary) to isolate the second least significant bit
  // Then check if it's equal to 0
  return ((value & 0b00000010) == 0);
}

bool checkIfMatches(uint8_t val1, uint8_t val2, uint8_t val3) {
  // Convert the values to hexadecimal characters
  char hexChars[7]; // 6 characters for "00005E" + 1 for null terminator
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
            Serial.printf("%2d",i + 1);
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
            
            Serial.printf("%02X%02X%02X%02X%02X%02X", *WiFi.BSSID(i),*(WiFi.BSSID(i)+1),*(WiFi.BSSID(i)+2),*(WiFi.BSSID(i)+3),*(WiFi.BSSID(i)+4),*(WiFi.BSSID(i)+5));

            Serial.print(" | ");
          
            // Check condition 1: Second character must be 0 or 2
            if (checkSecondLSB(*WiFi.BSSID(i)) == true) {
              if (checkIfMatches(*WiFi.BSSID(i), *(WiFi.BSSID(i)+1), *(WiFi.BSSID(i)+2) == true)) {
                Serial.print("Passed");
                if (addedNetworks < 8) {
                  for (int j = 0; j < 6; j++) {
                    macList[addedNetworks][j] = *(WiFi.BSSID(i)+j); 
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
        appData[i*6+j] = macList[i][j];
      }
      
      Serial.printf("%02X%02X%02X%02X%02X%02X", macList[i][0],macList[i][1],macList[i][2],macList[i][3],macList[i][4],macList[i][5]);
      Serial.println();
    }
    

    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();
}


void setup()
{
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
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

void loop()
{
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
        LoRaWAN.displayJoining();
        LoRaWAN.join();
        break;
      }
    case DEVICE_STATE_SEND:
      {
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
