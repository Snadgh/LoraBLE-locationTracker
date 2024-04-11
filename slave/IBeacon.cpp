/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux
   Adapted for specific use by Simon Langlais (broadcast battery level in minor/Major)
   use ESP32-devKitV1
*/

 
/*
   Create a BLE server that will send periodic iBeacon frames.
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create advertising data
   3. Start advertising.
   4. wait
   5. Stop advertising.
   6. deep sleep
   
*/
 //some usefuls links
 //https://www.bluetooth.com/wp-content/uploads/Files/Specification/Assigned_Numbers.html#bookmark43
 //https://www.bluetooth.com/specifications/css-11/
 //
#include "sys/time.h"
#include "Arduino.h"
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"

#define PRINT true

#define GPIO_DEEP_SLEEP_DURATION    2 // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

//Here the RTC_DATA_ATTR is defined. Note that if you define a global variable with RTC_DATA_ATTR attribute, the variable will be placed into RTC_SLOW_MEM memory. 
//So the structure declared as RTC_DATA_ATTR and copying the dynamic memory to this structure before a deep sleep helps recovering this into dynamic memory after wake up. 
//In simple words we are saving the time in static memory from dynamic memory to recover it again after a deep sleep. Here the two variables are defined. 
//The ‘last’ is used to get the last time when the ESP32 went to deep sleep and bootcount is used count number of resets.
#ifdef __cplusplus
extern "C" {
#endif

uint8_t temprature_sens_read();//??
//uint8_t g_phyFuns;

#ifdef __cplusplus
}
#endif

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
BLEAdvertising *pAdvertising;
struct timeval now;
#define BEACON_UUID           "e6e9a717-9cc0-4485-8fb1-941f05273c8d" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/), we randomly generated one
                              //careful, advertised uuid is inversed, bit order is LSB first, meaning that 8d will appear first here
                              //#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8)) <-- how it's being done
void setBeacon() {            

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0xFFFF); // ex: Apple= 0x004C LSB (ENDIAN_CHANGE_U16!), let's use a reserved one 0xFFFF as we are no company..
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID)); 

 oBeacon.setMajor(1234); //We can put a 4 hex digit number, which allows a max of 65535
 oBeacon.setMinor(4321); //same
  oBeacon.setSignalPower(-58);//useless, cannot detec it on the scanner, don't know why
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData(); //we create our own Advertised data and populate it later according to our payload and IBeacon specs
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData(); //In my understanding, must create a scan response even if we never scan
  
  oAdvertisementData.setFlags(0x06); // BR_EDR_NOT_SUPPORTED 0x04 ,device does not support Bluetooth Basic Rate/Enhanced Data Rate (BR/EDR) functionality -LE only device, see official documentation for FLAGS
                                     
  
  std::string strServiceData = "";
  
  strServiceData += (char)26;     // Len  //to respect IBeacon Spec
  strServiceData += (char)0xFF;   // Type (=manufacturer data)
  strServiceData += oBeacon.getData();  //return every attributes of the beacon in the form of a string, encoding information in hexadecimal 


  oAdvertisementData.addData(strServiceData);  // add data to the payload of the advertised packet
  
  pAdvertising->setAdvertisementData(oAdvertisementData); // Set the advertisement data that is to be published in a regular advertisement.
  pAdvertising->setScanResponseData(oScanResponseData);  //Set the advertisement data that is to be published in a scan response, empty here
  pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND); //specify the advertysing type, this one is for Non connectable devices, advertising information to any listening device -> beacon

}

void setup() {

    
 if(PRINT) Serial.begin(115200);
  // Create the BLE Device
  BLEDevice::init("simon"); //name is sometumes appearing,but most of the time not, don't know why

  
  // Create the BLE Server is no longer required to create beacon
   //BLEServer *pServer = BLEDevice::createServer();

  gettimeofday(&now, NULL);

   if(PRINT) Serial.printf("start ESP32 %d\n",bootcount++);

   if(PRINT) Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n",now.tv_sec,now.tv_sec-last);

  last = now.tv_sec;
  


  pAdvertising = BLEDevice::getAdvertising();

  setBeacon(); //perform the low level implementation of the Beacon by refining the advertised packet using the ibeacon protocol
   // Start advertising
  pAdvertising->start();
   if(PRINT) Serial.println("Advertizing started...");
  delay(100);
  pAdvertising->stop();
  if(PRINT)  Serial.printf("enter deep sleep\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
   if(PRINT) Serial.printf("in deep sleep\n");


}

void loop() {

}
