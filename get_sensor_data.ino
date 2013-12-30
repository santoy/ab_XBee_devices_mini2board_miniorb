int getSound(int _spin, int _duration)
{
  int sample;
  int myMax=0;
  int myMin=1023;
  unsigned long sampleTimer = millis();
  while(millis() - sampleTimer < _duration)
  {
    sample = analogRead(_spin);
    myMax=max(myMax,sample);
    myMin=min(myMin,sample);
  }
  int volume = myMax-myMin;
  return volume;
}

int getLight(int _lpin, int _duration)
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
