//interrupt subroutine for reading the time between rx pulses which is used to work out pilot input
ISR(PCINT0_vect) {
  unsigned long currentTime = micros();//this is more efficient than calling the micros() method multiple times in this subroutine

  //Channel 1 - the aileron channel/roll channel:
  if (last[1] == 0 && PINB & B00000001) {//if the pulse has just started 
    last[1] = 1;
    timerChannel[1] = currentTime;//start timing
  } else if (last[1] == 1 && !(PINB & B00000001)) {//if the pulse has just ended
    last[1] = 0;
    rxInput[1] = currentTime - timerChannel[1];//work out pulse duration
  }

  //channel 2, pitch/elevator channel
  if (last[2] == 0 && PINB & B00000010) {
    last[2] = 1;
    timerChannel[2] = currentTime;
  } else if (last[2] == 1 && !(PINB & B00000010)) {
    last[2] = 0;
    rxInput[2] = currentTime - timerChannel[2];
  }

  //switch channel, actually channel 5 or 6
  if (last[3] == 0 && PINB & B00000100) {
    last[3] = 1;
    timerChannel[3] = currentTime;
  } else if (last[3] == 1 && !(PINB & B00000100)) {
    last[3] = 0;
    rxInput[3] = currentTime - timerChannel[3];
  }
}
