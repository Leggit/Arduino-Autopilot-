
/*
 * function that returns an output which is used to set the servo angles
 * The ouput should be a pulse length between 1000 and 2000 microseconds
 */
long calculatePID(int axis,float currentAngle, float pGain, float iGain, float dGain) {
  int desiredAngle;//worked out from pilot input
  int error;
  long output = 0;
  float p,d, pid;//proportial, derivative (integral is a global variable as it needs to retain its value) and all three together

  //deadband of pilot input - if it is between these the desired angle is 0
  if (rxInput[axis] > 1450 && rxInput[axis] < 1550) {
    desiredAngle = 0;
  } else {//convert pilot input in the form of a pulse length to a value in degrees approx between 75 and -75deg
    desiredAngle = (rxInput[axis] - 1500) / 9;
  }

  error = desiredAngle - currentAngle;//work out the discrepancy between desired angle and the actual angle

  p = pGain * error;//proportional term is pretty simple to work out 
  
  if(error > -20 && error < 20){//limit integral to only work for small errors or it can build up too quick and cause problems 
    i[axis] = i[axis] + error * iGain;
    //i[axis] = 0;
  }else{
    i[axis] = 0;
    }
  
  d = dGain * (error - prevError[axis]);//derivative is to do with the rate of change 
  prevError[axis] = error;//store the error of this loop for use in the next derivative calculation 
  //d = 0;
  pid = p + i[axis] + d;//add them all together

  output = 1500 - pid * 8;//convert back to a pulse length value 

  //limiting is done in the setServoAngles method, which gives better results with elevon mixing
//  if(output < 1000) output = 1100;
//  if(output > 2000) output = 1900;
  
  return (output);

}
