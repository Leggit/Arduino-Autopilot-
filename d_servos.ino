/**
 * used to actually set the physical output of the system via the servo motors which move control surfaces
 */
void setServoAngles(long pulseOne, long pulseTwo) {
  //elevon mixing as I use a flying wing - comment out if you do not
  long servoTwo = pulseOne * 0.5 + pulseTwo * 0.5;
  long servoOne = 1500 - (pulseOne * 0.5 - pulseTwo * 0.5);
  pulseOne = servoOne;
  pulseTwo = servoTwo;

  //set limits - servos only take pulses between 2000 and 1000 microseconds
  if (pulseOne > 2000) pulseOne = 2000;
  if (pulseOne < 1000) pulseOne = 1000;
  if (pulseTwo > 2000) pulseTwo = 2000;
  if (pulseTwo < 1000) pulseTwo = 1000;

  //sometimes useful to check
  Serial.print(pulseOne);
  Serial.print(",");
  Serial.println(pulseTwo);

  loopTime = micros();//record start time 
  PORTD |= B01100000;//set both servo pulses on

  //work the time when each pulse should stop
  pulseTwo += loopTime;
  pulseOne += loopTime;

  while (PORTD >= 32) {//loops until both pulses have stopped
    loopTime = micros();//look at the current time 
    //stop the pulses if the required pulse duration has been reached
    if (pulseTwo <= loopTime)PORTD &= B11011111;//
    if (pulseOne <= loopTime)PORTD &= B10111111;
  }
}
