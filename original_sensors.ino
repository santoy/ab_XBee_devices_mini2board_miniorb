// Used only for the original configuration

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

