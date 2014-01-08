/* ==================================================
Get Samples of Sensor Values for better result
================================================== */ 

int getSound(int _spin, int _duration) // this deals with sound wave
{
  int sample;
  int myMax=0;
  int myMin=1023;
  unsigned long sampleTimer = millis();
  while(sampleTimer + _duration > millis())
  {
    sample = analogRead(_spin);
    myMax=max(myMax,sample);
    myMin=min(myMin,sample);
  }
  int volume = myMax-myMin;
  return volume;
}

int getLight(int _lpin, int _duration) // this deals with flickering (fluorsecent) light
{
  int sample;
  int myMax=0;
  int myMin=1023;
  unsigned long sampleTimer = millis();
  while(millis() - sampleTimer < _duration)
  {
    sample = analogRead(_lpin);
    myMax=max(myMax,sample);
    myMin=min(myMin,sample);
  }
  int brightness = ((myMax-myMin)/2)+myMin;
  return brightness;
}


/* ==================================================
Used only for the original configuration
================================================== */ 

void checkSensors()
{
  pot = analogRead(potPin);
  vm = analogRead(vmPin);
  for(int x=0; x<5; x++) 
  {
    int sw = digitalRead(jstickpins[x]);
    if(sw == 0)
    {
      jstick[x] += 1;
      if(jstick[x] > 20)
      {
        if(x==0)
        {
          jsbyte = 0;
          jstick[x] = 0;
        }else{
          jsbyte = jsbyte | (1<<x);
          jstick[x] = 0;
        }
        for(int i=0; i<3; i++)
        {
          digitalWrite(led1,1);
          delay(50);
          digitalWrite(led1,0);
          delay(50);
        }
      }
    }else{
      jstick[x] = 0;
    }
  }
}

