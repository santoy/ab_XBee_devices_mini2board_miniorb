 /* 
======================================================
  Active Buildings
  Device-side (XBee router) Program for Arduino
  with Network communication with XBee
  *** This code is for Mini2 Printed Circuit Board. 

  Author: Yasu Santo 2013 (yasu@santo.com)
  ==================================================
  DHT.cpp - DHT sensor library
  https://github.com/ringerc/Arduino-DHT22
======================================================
*/

#include <DHT22.h>
#include <XBee.h>
#include "_includes.h"

const byte deviceType = 10;  
        //  5 = original mini-board 2
        //      -light, sound, temperature, humidity, joystick, signal-strength, potentiometer, voltage
        // 10 = modified version for mini-orb
        //      - light, sound, temperature, humidity, orb-state-1, orb-state-2, orb-state-3, state-position

void setup(){
  Serial.begin(9600);
  xbee.setSerial(Serial);
  for(int x=0; x<devicePayLoad; x++) payload[x] = 0;
  
  if(deviceType == 5){
    pinMode(led1,1); digitalWrite(led1,0);
    pinMode(led2,1); digitalWrite(led2,0);
    pinMode(xreset, 1); digitalWrite(xreset, 1);
    for(int i=0; i<5; i++){
      pinMode(jstickpins[i],INPUT_PULLUP); // not used with mini-orb
    }
  }else if(deviceType == 10){
    pinMode(led2,1); digitalWrite(led2,0);
    pinMode(ledPin1,1); digitalWrite(ledPin1, 0);
    pinMode(ledPin2,1); digitalWrite(ledPin2, 0);
    pinMode(buzPin,1);
  }
  // initialise wheel value to the current position of wheel
  pinMode(ab, INPUT);
  pinMode(aa, OUTPUT);
  digitalWrite(aa,1);
  wheel = constrain(analogRead(ab),200,711) - 200;
  prevWheel = wheel;
}

void loop(){
  receiveXbeeData();
  
  if(millis() > sensorTimer + sensorInterval || millis() < sensorTimer){
    sensorTimer = millis();
    //===== store previous sensor values
    pLight = light;
    pSound = sound;
    
    //===== check all sensors
    light = max(getLight(lightPin,40),pLight);
    sound = max(getSound(soundPin,50),pSound);
    temp = (myDHT22.getTemperatureC()+40)*10; // (conert temperature, -40c - 80c with 0.1 increment,  to 10bit value)
    humid = (myDHT22.getHumidity())*10;       // (convert humidity, 0 - 100% with 0.1 increment, to 1bit value)
    
    playBuzz(bRise, bPitch, bDuration);
  }
  
  if(millis() > dhtTimer + dhtInterval || millis() < dhtTimer){
    dhtTimer = millis();
    errorCode = myDHT22.readData();
  }
  
  if(deviceType == 5){ // for original mini-board v3
    checkSensors();
  }else if(deviceType == 10){ // for board with mini-orb
    controlMiniOrb();
  }
}

