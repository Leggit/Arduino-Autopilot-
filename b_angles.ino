//methods for calculating the current pitch and roll angles of the sensor 

/**
 * works out the angles from sensor readings
 */
void calculateAngles() {
  //local variables 
  float accelPitch, accelRoll;
  float yawCorrection;//merged with pitch and roll

  readMPU();//read in raw values 

  //subtract calibration values 
  gyroX -= gyroOffsetX;
  gyroY -= gyroOffsetY;
  gyroZ -= gyroOffsetZ;

  //convert raw values to degrees 
  pitch += gyroX * RATE_TO_DEG;
  roll += gyroY * RATE_TO_DEG;

  //if the RC is pitching up and then yaws, the pitch and roll values should change accordingly 
  yawCorrection = sin(gyroZ * RATE_TO_RAD);
  pitch += roll * yawCorrection;
  roll -= pitch * yawCorrection;

  //work out the current angles according to accel readings
  accelVector = sqrt((accelX * accelX) + (accelY * accelY) + (accelZ * accelZ));
  accelPitch = accelAngle(accelY, accelVector, pitch);
  accelRoll = accelAngle(accelX, accelVector, roll) * -1;

  //  Serial.print("Gyro Pitch: ");
  //  Serial.print(pitch);
  //  Serial.print(" accel Pitch: ");
  //  Serial.print(accelPitch);
  //  Serial.print("Gyro roll: ");
  //  Serial.print(roll);
  //  Serial.print(" accel roll: ");
  //  Serial.print(accelRoll);

  //merge the angles values worked out from Gyro readings with the angle values worked out from accel readings
  pitch = (pitch * 0.996 + accelPitch * 0.004);//this ratio works best to stop drift but not be hugely put off by accel errors due to sudden accelerations
  roll = (roll * 0.996 + accelRoll * 0.004);

}

/**
 * used in debugging to check how sensible the results are
 */
void printAngles(float pitchPrint, float rollPrint) {
  Serial.print(F("Roll: "));
  Serial.print(rollPrint);
  Serial.print(F(", Pitch: "));
  Serial.print(pitchPrint);
  Serial.print(F(" Aileron channel: "));
  Serial.print(rxInput[1]);
  Serial.print(F(" Pitch channel: "));
  Serial.print(rxInput[2]);
  Serial.print(F(" Switch channel: "));
  Serial.println(rxInput[3]);
}

/**
 * work out the current angles according to just the accel readings
 */
float accelAngle(float axis, float vector, float currentAngle) {
  if (abs(axis) < vector) {
    float answer;
    answer = asin((float)axis / vector) * RAD_TO_DEG;
    return answer;
  }
  else {
    return currentAngle;
  }
}

/**
 * get the raw values from the sensor
 */
void readMPU(){ 
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU, 14);
  while (Wire.available() < 14);
  accelX = Wire.read() << 8 | Wire.read();
  accelY = Wire.read() << 8 | Wire.read();
  accelZ = Wire.read() << 8 | Wire.read();
  temp = Wire.read() << 8 | Wire.read();
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();
}
