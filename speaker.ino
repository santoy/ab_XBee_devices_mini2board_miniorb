void playBuzz(int _rise, int _pitch, int _duration){
  for(int y=0;y<_rise;y++){
    for(int x=0; x<_duration; x++){
      int pitch=(y+1)*_pitch;
      digitalWrite(buzPin, 1);
      delayMicroseconds(pitch);
      digitalWrite(buzPin,0);
      delayMicroseconds(pitch);
    }
  }
}

