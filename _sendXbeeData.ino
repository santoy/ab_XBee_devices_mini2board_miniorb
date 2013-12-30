void sendXbeeData(){
  // Bytes to be sent to XBee Master.
  // Processing Application needs to mirror these sequence of bytes.
  payload[0] = deviceType;             // type of device (5=original; 10=mini-orb)
  payload[1] = 8;                      // number of sensors 
  // sensor bytes below (typically 2 bytes per sensor).
  payload[2] = light & 127;            // low 7 bits of light sensor value
  payload[3] = (light>>7) & 15;        // next 4 higher bits of light sensor value
  payload[4] = sound & 127;            // low 7 bits of sound sensor value
  payload[5] = (sound>>7) & 15;        // next 4 higher bits of sound sensor value
  payload[6] = temp & 127;             // low 7 bits of temperature sensor value
  payload[7] = (temp>>7) & 15;         // next 4 higher bits of temperature sensor value
  payload[8] = humid & 127;            // low 7 bits of humidity sensor value
  payload[9] = (humid>>7) & 15;        // next 4 higher bits of humidity sensor value
  
  if(deviceType == 5){
    payload[10] = jsbyte & 127;          // low 7 bits of joystick binary value
    payload[11] = (jsbyte>>7) & 15;      // next 4 higher bits of joystick binary value
    payload[12] = rssi & 127;            // low 7 bits of RSSI value
    payload[13] = (rssi>>7) & 15;        // next 4 higher bits of RSSI value
    payload[14] = pot & 127;             // low 7 bits of potentiometer value
    payload[15] = (pot>>7) & 15;         // next 4 higher bits of potentiometer value
    payload[16] = vm & 127;              // low 7 bits of voltage monitor value
    payload[17] = (vm>>7) & 15;          // high 4 bits of voltage monitor value
    payload[18] = 0;                     // not relevant for non-orb devices
  }else if(deviceType == 10){
    payload[10] = _pp[0] & 127;              // low 7 bits of mini-orb Temperature Preference value
    payload[11] = (_pp[0]>>7) & 15;          // next 4 higher mini-orb Temperature Preference value
    payload[12] = _pp[1] & 127;              // low 7 bits of mini-orb Light Preference value
    payload[13] = (_pp[1]>>7) & 15;          // next 4 higher mini-orb Light Preference value
    payload[14] = _pp[2] & 127;              // low 7 bits of mini-orb Sound Preference value
    payload[15] = (_pp[2]>>7) & 15;          // next 4 higher mini-orb Sound Preference value
    payload[16] = _pp[3] & 127;              // low 7 bits to mini-orb Social Preference value
    payload[17] = (_pp[3]>>7) & 15;          // next 4 higher mini-orb Social Preference value
    payload[18] = interacted & 255;          // mini-orb was interacted (1) or not (2);
    interacted = 0;
  }
  
  light=0;
  pLight=0;
  sound=0;
  pSound=0;
  jsbyte=0;
  swCurrentState=0;
        
  xbee.send(zbTx);
  
  if (xbee.readPacket(500)) {
    // got a response within 500ms!

    // should be a znet tx status                   
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
                
      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        for(int b=0; b<3; b++){
          digitalWrite(led2,1);
          delay(20);
          digitalWrite(led2,0);
          delay(30);
        }
      } else {
        // the remote XBee did not receive our packet. is it powered on?
      }
    }      
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
//    for(int b=0; b<1; b++){
//      digitalWrite(signalGreenPin,1);
//      delay(250);
//      digitalWrite(signalGreenPin,0);
//      delay(250);
//    }
  }
}
