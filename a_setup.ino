//methods for setting up the sensor registers, testing the sensor, calibrating the sensor and setting the starting angle of the sensor

/*
 * tests the connection with the sensor
 */
boolean testMPU(){
  int answer;
  Wire.begin();
  Wire.beginTransmission(MPU);
  answer = Wire.endTransmission();//this method will return an integer value of 0 if there is no problem with the connection

  if(answer != 0) return true;//if the answer is not 0, there is an error
  else return false;
  }

/*
 * sets the parameters to be used by the sensor
 */
void setupMPU() {
  //disable sleepmode
  Serial.println(F("Disbaling sleep mode..."));
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0b00000000);
  Wire.endTransmission();

  //set the gyro range to 500 deg/sec
  Serial.println(F("Setting gyro range..."));
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);
  Wire.write(0x08);
  Wire.endTransmission();

  //set the accel range to +-8g
  Serial.println(F("Setting accel range..."));
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();

  Serial.println(F("Setup complete"));
}

/**
 * Works out the average output for each gyro access when it is still
 */
void calibrateGyro() {
  Serial.println(F("Calibrating gyro, please keep very still..."));

  gyroOffsetX = 0;
  gyroOffsetY = 0;
  gyroOffsetZ = 0;

  for (int count = 0; count < 2000; count++) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission();
    Wire.requestFrom(MPU, 6);
    while (Wire.available() < 6);
    gyroOffsetX = gyroOffsetX + Wire.read() << 8 | Wire.read();
    gyroOffsetY = gyroOffsetY + Wire.read() << 8 | Wire.read();
    gyroOffsetZ = gyroOffsetZ + Wire.read() << 8 | Wire.read();
    delay(1);
  }

  gyroOffsetX = gyroOffsetX / 2000;
  gyroOffsetY = gyroOffsetY / 2000;
  gyroOffsetZ = gyroOffsetZ / 2000;

  //sometimes useful to know
  Serial.println(F("Average gyro offsets:"));
  Serial.print(gyroOffsetX);
  Serial.print(F(","));
  Serial.print(gyroOffsetY);
  Serial.print(F(","));
  Serial.println(gyroOffsetZ);
}

/**
 * The accel angles are always based on the orientation to the earth so are used to work out the starting angle of the sensor
 * The sensor must be kept still during this project 
 */
void setStartAngles() {
  //initialise
  pitch = 0;
  roll = 0;

  readMPU();//get the current sensor values

  //work out the current pitch and roll based on the acclermoter readings
  accelVector = sqrt((accelX * accelX) + (accelY * accelY) + (accelZ * accelZ));
  pitch = accelAngle(accelY, accelVector, pitch);
  roll = accelAngle(accelX, accelVector, pitch) * -1;
  
  //sometimes useful to know
  //printAngles(roll,pitch);
}
