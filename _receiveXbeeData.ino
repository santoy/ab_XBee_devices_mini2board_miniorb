void receiveXbeeData()
{
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable()) { // got something
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) { // got a zb rx packet
      // now fill our zb rx class
      xbee.getResponse().getZBRxResponse(rx);

      //      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) { // the sender got an ACK
      //        //flashLed(statusLedPin, 2, 100);
      //      } else { // we got it (obviously) but sender didn't get an ACK
      //        //flashLed(statusLedPin, 4, 100);
      //      }

      remoteData[0] = rx.getData(0);
      
      if(remoteData[0] > 0) for(int x=0; x<remoteData[0]; x++) remoteData[x+1] = rx.getData(x+1);

      switch(remoteData[0]){
        case 0:
          sendXbeeData();
          break;
        case 6:
          //sendXbeeData();
          if(remoteData[3] > 3) bDuration = remoteData[3]; // 0 and 1 do not change alarm status
          if(remoteData[3] == 1) bDuration = 0;            // 1 means force alarm to stop remotely
          bRise = remoteData[1];
          bPitch = remoteData[2];
          break;
      }
    } 
//    else if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) { 
//      xbee.getResponse().getZBTxStatusResponse(txStatus);
//      // get the delivery status, the fifth byte
//      if (txStatus.getDeliveryStatus() == SUCCESS) {
//        // success.  time to celebrate
//      } else {
//        // the remote XBee did not receive our packet. is it powered on?
//      }
//    } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
//      xbee.getResponse().getModemStatusResponse(msr);
//      // the local XBee sends this response on certain events, like association/dissociation
//      if (msr.getStatus() == ASSOCIATED) {
//        // yay this is great.  flash led
//        //flashLed(statusLedPin, 5, 100);
//      } 
//      else if (msr.getStatus() == DISASSOCIATED) {
//        // this is awful.. flash led to show our discontent
//        //flashLed(statusLedPin, 10, 20);
//      } else {
//        // another status
//        //flashLed(statusLedPin, 10, 20);
//      }
//    } 
    else {
      // not something we were expecting
      //flashLed(statusLedPin, 1, 300);    
    }
  } else if (xbee.getResponse().isError()) {
    //nss.print("Error reading packet.  Error code: ");  
    //nss.println(xbee.getResponse().getErrorCode());
  }
}

