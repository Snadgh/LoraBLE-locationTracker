/*
 * Author: JP Meijers
 * Date: 2016-10-20
 * Edited: Snaedis Daniels
 * Date: 2024-03-08
 *
 * Transmit a one byte packet via TTN. This happens as fast as possible, while still keeping to
 * the 1% duty cycle rules enforced by the RN2483's built in LoRaWAN stack. Even though this is
 * allowed by the radio regulations of the 868MHz band, the fair use policy of TTN may prohibit this.
 *
 * CHECK THE RULES BEFORE USING THIS PROGRAM!
 *
 * CHANGE ADDRESS!
 * Change the device address, network (session) key, and app (session) key to the values
 * that are registered via the TTN dashboard.
 * The appropriate line is "myLora.initABP(XXX);" or "myLora.initOTAA(XXX);"
 * When using ABP, it is advised to enable "relax frame count".
 *
 * Connect the RN2xx3 as follows:
 * RN2xx3 -- ESP32
 * Uart TX -- RX2
 * Uart RX -- TX2
 * Reset -- GPIO23
 * Vcc -- 3.3V
 * Gnd -- Gnd
 *
 */
#include <rn2xx3.h>          // Needed for certain RN2483 commands
#include <HardwareSerial.h>  // Hardware UART
#include <Arduino.h>         // arduino references

#define RXD2 16                // RX2 is GPIO16
#define TXD2 17                // TX2 is GPIO17
#define RST 23                 // RST is GPIO23
#define RESET 23               // RESET is GPIO23
HardwareSerial loraSerial(1);  // UART1

//create an instance of the rn2xx3 library,
//giving the software UART as stream to use,
//and using LoRa WAN
rn2xx3 myLora(loraSerial);

// the setup routine runs once when you press reset:
void setup() {
  // LED pin is GPIO2 which is the ESP8266's built in LED
  pinMode(2, OUTPUT);
  led_on();

  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  loraSerial.setRxTimeout(1024);
  // Hardware serial setup: data bits 8 and 1 stop bit
  loraSerial.begin(57600, SERIAL_8N1, RXD2, TXD2);
  loraSerial.setTimeout(1000);  // wait for serial data

  delay(1000);  //wait for the arduino ide's serial console to open

  Serial.println("Startup");

  initialize_radio();

  //transmit a startup message
  myLora.tx("TTN Mapper on ESP32 node");

  led_off();
  delay(2000);
}

void initialize_radio() {
  //reset RN2xx3
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(100);
  digitalWrite(RESET, HIGH);

  delay(100);  //wait for the RN2xx3's startup message
  loraSerial.flush();

  //check communication with radio
  String hweui = myLora.hweui();
  while (hweui.length() != 16) {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(hweui);
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;

  //ABP: initABP(String addr, String AppSKey, String NwkSKey);
  //join_result = myLora.initABP("02017201", "8D7FFEF938589D95AAD928C2E2E7E48F", "AE17E567AECC8787F749A62F5541D522");

  //OTAA: initOTAA(String AppEUI, String AppKey);
  join_result = myLora.initOTAA("BE7A000000001465", "8819E56B04A8441BC17B9C1AB26CD8A3");

  while (!join_result) {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000);  //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");
}

// the loop routine runs over and over again forever:
void loop() {
  led_on();

  Serial.println("TXing");
  myLora.tx("A");  //one byte, blocking function

  String rx = myLora.getRx(); // listen to Rx
  if (rx != "") {
    Serial.print("RXing: ");
    Serial.println(rx);
  }

  led_off();
  delay(200);
}

void led_on() {
  digitalWrite(2, 1);
}

void led_off() {
  digitalWrite(2, 0);
}
