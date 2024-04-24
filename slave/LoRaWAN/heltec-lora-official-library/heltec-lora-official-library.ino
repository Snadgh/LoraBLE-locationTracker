/* Heltec Automation LoRaWAN communication example
 *
 * Function:
 * 1. Upload node data to the server using the standard LoRaWAN protocol.
 * 2. The network access status of LoRaWAN is displayed on the screen.
 * 
 * Description:
 * 1. Communicate using LoRaWAN protocol.
 * 
 * HelTec AutoMation, Chengdu, China
 * 成都惠利特自动化科技有限公司
 * www.heltec.org
 *
 * this project also realess in GitHub:
 * https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
 * */

#include "LoRaWan_APP.h"

// CIBICOM stuff:
/* OTAA para*/
uint8_t devEui[] = { 0xBE, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x02, 0x7D };
uint8_t appEui[] = { 0xBE, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x14, 0x65 };
uint8_t appKey[] = { 0x2D, 0x0F, 0x4D, 0x3D, 0x82, 0x93, 0x01, 0x5A, 0x70, 0x27, 0xF5, 0x56, 0x4C, 0x06, 0x3A, 0xD0 };

/* ABP para*/
uint8_t nwkSKey[] = { 0x0B, 0xD2, 0x2E, 0x3C, 0x93, 0xAA, 0x59, 0xF3, 0x67, 0x85, 0x82, 0xDE, 0x0D, 0x13, 0xBA, 0xBC };
uint8_t appSKey[] = { 0xDE, 0x29, 0x8F, 0xC4, 0x1D, 0x1C, 0x83, 0x2E, 0xCF, 0x5C, 0x96, 0xC2, 0x87, 0x73, 0xB0, 0x3B };
uint32_t devAddr =  ( uint32_t )0x008ae0d;


// HELIUM stuff:
///* OTAA para*/
//uint8_t devEui[] = { 0x60, 0x81, 0xF9, 0xCB, 0x3A, 0x8A, 0x03, 0xF9 };
//uint8_t appEui[] = { 0x60, 0x81, 0xF9, 0x8F, 0x0D, 0x52, 0x3E, 0xDC };
//uint8_t appKey[] = { 0xB3, 0x0D, 0x4A, 0xDC, 0x88, 0x0A, 0x9D, 0x73, 0xA3, 0x60, 0x2E, 0x84, 0x10, 0x08, 0x76, 0xE7 };
//
///* ABP para*/
//uint8_t nwkSKey[] = { 0x0B, 0xD2, 0x2E, 0x3C, 0x93, 0xAA, 0x59, 0xF3, 0x67, 0x85, 0x82, 0xDE, 0x0D, 0x13, 0xBA, 0xBC };
//uint8_t appSKey[] = { 0xDE, 0x29, 0x8F, 0xC4, 0x1D, 0x1C, 0x83, 0x2E, 0xCF, 0x5C, 0x96, 0xC2, 0x87, 0x73, 0xB0, 0x3B };
//uint32_t devAddr = (uint32_t)0x008ae0d;


/*LoraWan channelsmask*/
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 30000;

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
  appDataSize = 5;
  appData[0] = 0x8c;
  appData[1] = 0xff;
  appData[2] = 0xee;
  appData[3] = 0x60;
  appData[4] = 0xff;
}

RTC_DATA_ATTR bool firstrun = true;

void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

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
}
