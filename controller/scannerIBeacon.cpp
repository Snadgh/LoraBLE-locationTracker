/**
 * 
 * A BLE scanner for IBEACON, using HELTECH wifi-lora-32-V3
 * 
 * Scan for advertisers, compare adress to find predetermined beacon in order to extract RSSI (signal received strength), minor and Major (see Ibeacon protocol)
 * Then, we correlate RSSI with a predefined table in order to get an approximate distance
 * This distance depends on environment (obstacle, relative altitude between devices....) and on the end devices (emitter/receiver)
 * In this case, we use for the emitter an ESP32-DEVKITV1 and for the receiver a HELTECH wifi-lora-32-V3
 * THE HELTECH wifi-lora-32-V3 is equipped with a builtin screen, we print certains informations about the beacon on it
 * 
 * code by Simon Langlais, based on:
 * author unknown
 * updated by chegewara
 * 
 * 
 * TODO: 
 * -explain two last hex number in manufacturer packet??
 * -make the code to use the OLED screen and display wanted data
 * look at tradeoff between power consumption and reactivity for both side, there is room for improvement regarding the respective delay of scanning(receiver) and deep sleep(emitter)
 */
#include <Arduino.h>
#include "BLEDevice.h"
#include "BLEUtils.h"


const bool PRINT =true; 
BLEScan* pBLEScan=NULL;
std::string BLEDeviceAdress ="24:62:ab:cb:1b:6a"; //the adresse of the BLE emitter, use an mobile APP to determine it 
static boolean beaconFound = false; 
static BLEAdvertisedDevice* myDevice;                   //the advertised device by the remote server




std::string toHex(const std::string& input) { //could use BLEUtils function but this works well
    std::string hexString;
    hexString.reserve(input.length() * 2);
    for (unsigned char c : input) {
        hexString.push_back("0123456789ABCDEF"[c / 16]);
        hexString.push_back("0123456789ABCDEF"[c % 16]);
    }
    return hexString;
}


void printManufacturerData(){
       //one example:
      //see : https://en.wikipedia.org/wiki/IBeacon
      // getManufacturerData() : FFFF02158D3C27051F94B18F8544C09C17A7E9E604D210E1C6: 
      //FFFF: Manufacturer Id (we took the reserved one, see the official list)
      //02: length???
      //15: List of 128-bit Service Solicitation UUIDs see: https://www.bluetooth.com/wp-content/uploads/Files/Specification/Assigned_Numbers.html#bookmark43
      // then comes the 128-UUID (inversed by packet of two hex number, (=endian change ))
      //04D2: major
      //10E1: minor
      //C6 ??
      Serial.println("Manufacturer data of the beacon: ");
      //Serial.println(myDevice->getManufacturerData().c_str());
      Serial.println(toHex(myDevice->getManufacturerData().c_str()).c_str());
}
void printPayload(){
        /*
        The pay load is a buffer of bytes that is either 31 bytes long or terminated by
        a 0 length value.  Each entry in the buffer has the format:
        [length][type][data...]
        */
        Serial.println("PAYLOAD PRINTED:-----------------------------------------------------------------------");
        uint8_t* msgPayload =myDevice->getPayload();
        size_t  total_len=myDevice->getPayloadLength();

        bool finished =false;
        uint8_t length;
        uint8_t ad_type;
        uint8_t sizeConsumed = 0;

        while(!finished){
            length = *msgPayload;          // Retrieve the length of the record.
            msgPayload++;                  // Skip to type  
            sizeConsumed += 1 + length; // increase the size consumed, honestly I don't understand the logic behind
            if(length!=0){  // A length of 0 indicates that we have reached the end.
                ad_type = *msgPayload;//get the type
                msgPayload++;//move to the data
                Serial.print("length:");
                Serial.print(length);
                  length--;// I think it's because it starts at zero
                Serial.print("  type: ");
                //Serial.print(BLEUtils::advTypeToString(ad_type));
                Serial.print(toHex(std::to_string(ad_type)).c_str());
                Serial.print(" (");
                Serial.print(ad_type);
                Serial.print(")");
                Serial.print("  data:");
                //Serial.println(BLEUtils::buildPrintData(msgPayload,length).c_str());
                Serial.println(BLEUtils::buildHexData(nullptr,msgPayload,length));

                msgPayload += length;
           }//length <> 0
           if (sizeConsumed >= total_len) finished = true;
        }//while(!finished)
    
}

std::string getMajor(){ //could be used to send some data such as battery level of the beacon
    std::string str =myDevice->getManufacturerData();
    str=toHex(str);
    size_t start_pos = 40; // Starting position of the substring (0-based index) of the major
    size_t length = 4;     // Length of the substring
    std::string hex_substr="";
       
    try{ hex_substr = str.substr(start_pos, length);
    }
    catch(const std::out_of_range& e){
      if(PRINT)Serial.println("out of range in getMajor");
      return "error";
    }
   std::string dec_substr ="";
    try{
      uint16_t dec=std::stoull(hex_substr,nullptr,16);
      dec_substr=std::to_string(dec);
    }
    catch(const std::invalid_argument& e) {
         if(PRINT)Serial.println("invalid argument in getMajor");
        return "error";
    } catch (const std::out_of_range& e) {
           if(PRINT)Serial.println("out of range in getMajor");
        return "error";
    }
    
    return dec_substr+" (0x"+hex_substr+")";
}

std::string  getMinor(){ //could be used to send some data such as battery level of the beacon
    std::string str =myDevice->getManufacturerData();
      str=toHex(str);
    //FFFF02158D3C27051F94B18F8544C09C17A7E9E6/04D210E1C6  <_example of manufacturer data
    std::size_t start_pos = 44; // Starting position of the substring (0-based index) of the minor
    std::size_t length = 4;     // Length of the substring

    std::string hex_substr="";
    try{ hex_substr = str.substr(start_pos, length);
    }
    catch(const std::out_of_range& e){
      if(PRINT)Serial.println("out of range in getMinor");
      return "error";
    }
    std::string dec_substr ="";
    try{
      uint16_t dec=std::stoull(hex_substr,nullptr,16);
      dec_substr=std::to_string(dec);
    }
    catch(const std::invalid_argument& e) {
         if(PRINT)Serial.println("invalid argument in getMinor");
        return "error";
    } catch (const std::out_of_range& e) {
           if(PRINT)Serial.println("out of range in getMinor");
        return "error";
    }
    
    return dec_substr+" (0x"+hex_substr+")";

}
std::string getDistance(int RSSI){
  /*Based on the average RSSI strength measured, cannot be considered as accurate or reliable

  */
 if(RSSI>-70){
  return "Very Close !";
 }else if(RSSI>80){
  return "Around you";
 }else if(RSSI>90){
  return "Within 20 m";
 }
}
/**
 * Scan for BLE devices and find the first one that corresponds to the adress we registered
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) { 
    if (advertisedDevice.haveManufacturerData() ) { //IBeacon protocol is basically a certain way to build manufacturer data packet
     if(PRINT) Serial.print("BLE Advertised Device found: ");
     if(PRINT) Serial.print(advertisedDevice.getAddress().toString().c_str());
    if(advertisedDevice.getAddress().toString()== BLEDeviceAdress){//adresses match
        BLEDevice::getScan()->stop(); //getscan: static function that return the BLESCAN object pointer, we stop it since we found the device we need
        myDevice = new BLEAdvertisedDevice(advertisedDevice); 
        beaconFound= true; 
        if(PRINT)  Serial.println("------- BEACON FOUND ------------");
        //if(PRINT) printManufacturerData();
        if(myDevice->haveRSSI()) {
          //if(PRINT)Serial.print("RSSI:");
          //if(PRINT)Serial.println( myDevice->getRSSI()); //the value correlated with the distance
          Serial.print("Distance:");
          Serial.println(getDistance(myDevice->getRSSI()).c_str());
        }
        //Serial.println(myDevice->getTXPower()); //it's the power of the transmission source, since we didn't define , it's equal to zero
        //if(PRINT) printPayload(); 
        if(PRINT)Serial.print("Major: ");
        if(PRINT)Serial.print(getMajor().c_str());
        if(PRINT)Serial.print("   Minor:");
        if(PRINT)Serial.println(getMinor().c_str());
        
      }// same adress 
      else{
          if(PRINT) Serial.println("---- does not match 24:62:AB:CB:1B:6A");
        }
   }//have manufacturer data
  } // onResult
}; // MyAdvertisedDeviceCallbacks

void setupBLE(){

  if(PRINT) Serial.println("Starting Arduino BLE Scanner application...");
  BLEDevice::init(""); //no need for a name
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); 
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(false); //we do not wish a scan response, we just want the rssi strengh of the signal
}

void setup() {
   
  if(PRINT) Serial.begin(115200);
  setupBLE();
} // End of setup.


void loop() {
  pBLEScan->start(5, false); //5 is the scan time in seconds
  //the scan is automatically stopped when we discover the right device
  //printInfoOnScreen();  <---should be the function printing the info on the screen at that time
  pBLEScan->clearResults();  //for refreshing discovered advertised devices, as we want to adapt the distance displayed
  delay(6000); // Delay in second between loops.
} // End of loop
