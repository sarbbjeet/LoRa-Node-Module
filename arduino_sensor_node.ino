#include <SPI.h>              // include libraries
#include <LoRa.h>

//temperature sensors
#include <OneWire.h>
#include <DallasTemperature.h>


#ifdef ARDUINO_SAMD_MKRWAN1300
#error "This example is not compatible with the Arduino MKR WAN 1300 board!"
#endif

const int csPin = 10;          // (NSS) LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset 
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin



//input sensor and relay pins 
#define sensor1 A0;  //moisture sensor  
const int relay1= 3;   //relay pin 
const int SENSOR_PIN = 4; // Arduino pin connected to DS18B20 sensor's DQ pin

OneWire oneWire(SENSOR_PIN);         // setup a oneWire instance
DallasTemperature tempSensor(&oneWire); // pass oneWire to DallasTemperature library

float tempCelsius;    // temperature in Celsius
 float tempFahrenheit; // temperature in Fahrenheit


//sensors handling variables
//const int AirValue = 620;   //you need to replace this value with Value_1
const int AirValue = 210;   //you need to replace this value with Value_1
//const int WaterValue = 310;  //you need to replace this value with Value_2
const int WaterValue = 530;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;

//parameters to define sender/receiver packet 
String outgoing;              // outgoing message
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xBB;     // address of this device
byte destination = 0xAA;      // destination to send to
byte deviceModel = 0;         //deviceModel=0 ->2sensors and 1 relay, deviceModel=1 ->4 sensors and 2 relays circuit  
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

//get sensors input 
String sensorsData(){  
  //moisture sensor reading
  String message =""; 
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 100, 0);
  message +='i'; //sensor prefix
  message =message + String(soilmoisturepercent); 
  message +=','; //multiple values splitter
  message +='i'; //sensor prefix
  message +=String(tempCelsius,2); 
  //message +=String(35.90); 
  message +=','; //multiple values splitter
  message +='o'; //relay prefix
  message +=digitalRead(relay1); //read relay state
  return message; 
}

//method to split string 
String splitStr(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
       found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
   return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void getDataFromTemperatureSensor(){
  tempSensor.requestTemperatures();             // send the command to get temperatures
  tempCelsius = tempSensor.getTempCByIndex(0);  // read temperature in Celsius
  tempFahrenheit = tempCelsius * 9 / 5 + 32; // convert Celsius to Fahrenheit
}

void setup() {
  Serial.begin(115200);                   // initialize serial
  pinMode(relay1, OUTPUT);  //define relay
  digitalWrite(relay1,LOW); //relAy inactive
  tempSensor.begin();    // initialize the sensor

  while (!Serial);
  Serial.println("LoRa Duplex with callback");
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  if (!LoRa.begin(400E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    // Serial.println("Sending data = " + sensorsData());
    sendMessage(sensorsData());
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;     // 2-3 seconds
    LoRa.receive(); // go back into receive mode    
  }
  getDataFromTemperatureSensor();
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(deviceModel);              //model of transmitter 
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int gateway = LoRa.read();          // recipient address
  byte node = LoRa.read();            // sender address
  byte receiverDeviceModel = LoRa.read();      //recipient device model
  byte incomingLength = LoRa.read();    // incoming msg length
  byte relayValue = LoRa.read();    // device 0 means only 1 relay 

  String incoming = "";                 // payload of packet
  // if the recipient isn't this device or broadcast,
  Serial.println(node,HEX);
  if (node != localAddress) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }
  if(receiverDeviceModel != deviceModel){ //check for device model match 
   Serial.println("error: data received by unknown device model");
   return; 
  }  
  //relay control
  Serial.println(relayValue);
  if(relayValue !=0 && relayValue !=1){
    Serial.println("incorrect relay value");
    return;
  }
  digitalWrite(relay1,relayValue);


  // while (LoRa.available()) {            // can't use readString() in callback, so
  //   incoming += (char)LoRa.read();      // add bytes one by one
  // }

  // if (incomingLength != incoming.length()) {   // check length for error
  //   Serial.println("error: message length does not match length");
  //   return;                             // skip rest of function
  // }


  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(gateway, HEX));
  Serial.println("Sent to: 0x" + String(node, HEX));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();

}

