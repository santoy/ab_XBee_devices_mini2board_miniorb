/*==============================
Mini-Orb Code
Last Updated - January 2014
==============================*/

boolean swDown = false;            // switch state
boolean swBuzz = false;            // make feedback sound once when switch is pressed
boolean wheelMoved = false;        // wheel state (wheter it was moved or not)
int sw = 0;                        // mini-orb switch
int wheelPos = 0;                  // position of wheel (0-3)
int brightnessBeepPos = 0;         // beep position in the brightness setting 
int mode_matrix[4][3] = {          // sensor values for 3 modes (sensor, self-preference, group-preference)
    {128,128,128},
    {128,128,128},
    {128,128,128},
    {128,128,128}
  };
int matrixLocNumPrev = 0;          // previous sense value of where matrix position autoplay is at
unsigned long wheelTimer = 0;      // timer to monitor change of wheel value
unsigned long ledTimer = 0;        // timer to monitor RGB led transition time
unsigned long actionTimer = -10000; // timer to monitor interaction
int dimgrow = 0;                   // whether RGB LED is growing or dimming
int transition = 0;                // colour state of each RGB LED
int ts = 128;                      // transition speed - defines 1/4 of how long a mode-state is shown
int modeState = 0;                 // which mode (sensor, you, others) in automatic display
int senseState = 0;                // which sensor info (temp, light, sound, social) in automatic display


void controlMiniOrb()
{
  /// read momentary switch state
  pinMode(aa, INPUT);
  pinMode(ab, OUTPUT);
  digitalWrite(ab,1);
  int sw = analogRead(aa);
  
  /// read wheel position
  pinMode(ab, INPUT);
  pinMode(aa, OUTPUT);
  digitalWrite(aa,1);
  wheel = analogRead(ab); //(about 80 ~ 820)
  wheel = map(constrain(wheel,90,820) - 90, 0, 730, 0, 511);
  //Serial.println(wheel);
  
  int rgb[3]; // RGB LED colours
  
  /// response to momentary switch state change
  if(sw>700 && !swDown){ // if switch state changed from off to on
    swDown = true;
    wheelMoved = false;
    actionTimer = millis();
    interacted = 1;
    bDuration = 0;
    playBuzz(5, 50, 2); swBuzz=true; // play feedback sound - (rise, start pitch, duration)
  }else if(sw<200 && swDown){ // if switch state changed from on to off
    if(wheelMoved) _pp[senseState] = wheel;
    swDown = false;
    swBuzz=false;
    wheelMoved = false;
    interacted = 1;
    modeState = 0;
    transition = ts;
    dimgrow = ts;
    if(senseState == 0){ rgb[0]=mode_matrix[0][0]; rgb[1]=0; rgb[2]=0; }
    if(senseState == 1){ rgb[0]=0; rgb[1]=mode_matrix[1][0]; rgb[2]=0; }
    if(senseState == 2){ rgb[0]=0; rgb[1]=0; rgb[2]=mode_matrix[2][0]; }
    if(senseState == 3){ rgb[0]=mode_matrix[3][0]; rgb[1]=mode_matrix[3][0]; rgb[2]=0; }
    matrixLocNumPrev = 0;
    digitalWrite(ledPin1, 0); digitalWrite(ledPin2, 0); // turn middle indicator LED off
    actionTimer = millis();
    playBuzz(5, 100, 1); swBuzz=true; // play feedback sound - (rise, start pitch, duration)
  }
  
  /// respond to wheel position change
  if(abs(wheel - prevWheel) > 30){
    prevWheel = wheel;
    wheelMoved = true;
    actionTimer = millis();
    interacted = 1;
    bDuration = 0;
  }
  
  if(!wheelMoved && !swDown){
    int adjustled = 0; // defines amount of change of LED intensity from previous intensity
    int growled = 0; // current intensity of growing LED
    int dimled = 0;  // current intensity of dimming LED
    
    if(modeState == 0){
      growled = map(dimgrow,0,ts,0,mode_matrix[senseState][modeState]); // set current intensity of growing LED
      dimled = map(dimgrow,0,ts,matrixLocNumPrev,0); // set current intensity of dimming LED
    }else{
      growled = map(dimgrow,0,ts,matrixLocNumPrev,mode_matrix[senseState][modeState]);
      dimled = 0; 
    }
    
    adjustled = map(dimgrow,0,ts,matrixLocNumPrev,mode_matrix[senseState][modeState]);
    
    switch(senseState){
      case 0:
        rgb[0] = adjustled;
        rgb[1] = dimled;
        rgb[2] = 0;
        break;
      case 1:
        rgb[0] = dimled;
        rgb[1] = growled;
        rgb[2] = 0;
        break;
      case 2:
        rgb[0] = 0;
        rgb[1] = dimled;
        rgb[2] = growled;
        break;
      case 3:
        rgb[0] = growled;
        rgb[1] = growled;
        rgb[2] = dimled;
        break;
    }
    
    if(millis() > ledTimer + 10){
      ledTimer=millis();
      //if(transition==ts+(ts/ts) || transition==(ts*2)+(ts/ts) || transition==(ts*3)+(ts/ts) || transition==(ts*4)+(ts/ts)){
      if(transition>=(ts*4)-4){
        digitalWrite(ledPin1, 0); digitalWrite(ledPin2, 0);
      //}else if(transition==ts || transition==(ts*2) || transition==(ts*3) || transition==(ts*4)){
      }else if(transition>=ts){
        if(modeState==0) {digitalWrite(ledPin1, 0); digitalWrite(ledPin2, 1);}
        if(modeState==1) {digitalWrite(ledPin1, 1); digitalWrite(ledPin2, 0);}
        if(modeState==2) {digitalWrite(ledPin1, 1); digitalWrite(ledPin2, 1);}
      }
        
      transition+=1;
      if(transition>(ts*4)-1){
        matrixLocNumPrev = mode_matrix[senseState][modeState];
        mode_matrix[0][0] = map((constrain(temp,500,800) - 500),0,300,0,255);  // limit temperature to 10c ~ 40c before mapping
        mode_matrix[1][0] = map(light,0,1023,0,255);
        mode_matrix[2][0] = map(sound,0,1023,0,255);
        mode_matrix[3][0] = 128;  // not sure how this value (social) should be sensed locally.
        for(int x=0; x<4; x++) mode_matrix[x][2] = _gp[x];
        
        transition=0;
        dimgrow=0;
        modeState += 1;
        if(modeState>2) {
          modeState=0; 
          senseState+=1; 
          if(senseState>3) senseState=0;
        }
      }else if(transition<ts){
        dimgrow+=1;
      }
    }
    
    
  }else if(wheelMoved && !swDown){
    int r=2, p=100, d=2;     // for feedback sound
    int b=15;                 // buffer for mode change
    int t[] = {b*2,256,512-(b*2)};  // wheel position to change mode
    digitalWrite(ledPin1,0); digitalWrite(ledPin2,1); // turn middle indicator LED on
    switch(wheelPos){
      case 0:
        if(wheel > t[0]+b) wheelPos = 1;
        if(wheelPos != 0) playBuzz(r, p, d); // play feedback sound - (rise, start pitch, duration)
        break;
      case 1:
        if(wheel < t[0]-b) wheelPos = 0;
        if(wheel > t[1]+b) wheelPos = 2;
        if(wheelPos != 1) playBuzz(r, p, d); // play feedback sound - (rise, start pitch, duration)
        break;
      case 2:
        if(wheel < t[1]-b) wheelPos = 1;
        if(wheel > t[2]+b) wheelPos = 3;
        if(wheelPos != 2) playBuzz(r, p, d); // play feedback sound - (rise, start pitch, duration)
        break;
      case 3:
        if(wheel < t[2]-b) wheelPos = 2;
        if(wheelPos != 3) playBuzz(r, p, d); // play feedback sound - (rise, start pitch, duration)
        break;
    }
    senseState = wheelPos;
    if(wheelPos == 0){ rgb[0]=mode_matrix[0][0]; rgb[1]=0; rgb[2]=0; }
    if(wheelPos == 1){ rgb[0]=0; rgb[1]=mode_matrix[1][0]; rgb[2]=0; }
    if(wheelPos == 2){ rgb[0]=0; rgb[1]=0; rgb[2]=mode_matrix[2][0]; }
    if(wheelPos == 3){ rgb[0]=mode_matrix[3][0]; rgb[1]=mode_matrix[3][0]; rgb[2]=0; }
    
    if(millis() > actionTimer + 10000){
      wheelMoved = false;
      modeState = 0;
      transition = 0;
      dimgrow = 0;
      //bDuration = 0;
      digitalWrite(ledPin1,0); digitalWrite(ledPin2,0); // turn indicator LEDs off
    }
    
    
  }else if(swDown){
    digitalWrite(ledPin1, 1); digitalWrite(ledPin2, 0); // turn middle indicator LED on
    actionTimer = millis();
    
    if(wheelMoved){ 
      mode_matrix[senseState][1] = map(wheel,0,511,0,255);
    }
    
    switch(senseState){
      case 0:
        rgb[0]=mode_matrix[senseState][1];
        rgb[1]=0;
        rgb[2]=0;
        break;
      case 1:
        rgb[0]=0;
        rgb[1]=mode_matrix[senseState][1];
        rgb[2]=0;
        break;
      case 2:
        rgb[0]=0;
        rgb[1]=0;
        rgb[2]=mode_matrix[senseState][1];
        break;
      case 3:
        rgb[0]=mode_matrix[senseState][1];
        rgb[1]=mode_matrix[senseState][1];
        rgb[2]=0;
        break;
    }
    
    int r=1, p=100, d=1;     // for feedback sound
    int b=5;                 // buffer for mode change
    int t[] = {0,32,96,160,224,288,352,416,480,512};  // wheel position to change mode
    switch(brightnessBeepPos){
      case 3:
        if(wheel < t[brightnessBeepPos]-b) {brightnessBeepPos = brightnessBeepPos-1; playBuzz(r, 800, d);}
        if(wheel > t[brightnessBeepPos+1]+b) {brightnessBeepPos = brightnessBeepPos+1; playBuzz(3, 50, 8);}
        break;
      case 5:
        if(wheel < t[brightnessBeepPos]-b) {brightnessBeepPos = brightnessBeepPos-1; playBuzz(3, 50, 4);}
        if(wheel > t[brightnessBeepPos+1]+b) {brightnessBeepPos = brightnessBeepPos+1; playBuzz(r, 800, d);}
        break;
      default:
        if(wheel < t[brightnessBeepPos]-b) {brightnessBeepPos = brightnessBeepPos-1; playBuzz(r, 800, d);}
        if(wheel > t[brightnessBeepPos+1]+b) {brightnessBeepPos = brightnessBeepPos+1; playBuzz(r, 800, d);}
        break;
    }
  }
  
  analogWrite(rePin,calibrateLed(rgb[0]));
  analogWrite(grPin,calibrateLed(rgb[1]));
  analogWrite(blPin,calibrateLed(rgb[2]));
  
}

int calibrateLed(int _led){
  return map((_led/2)*(_led/2),0,128*128,0,255);
}
