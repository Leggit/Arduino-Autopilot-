//include library for I2C communication
#include <Wire.h>

//define some values that will remain constant 
#define MPU 0b1101000 //I2C address of the angle sensor
#define RAD_TO_DEG 57.296 //multiplier for converting from Radians ro Degrees
#define RATE_TO_RAD 0.000001066 //convert angular rate value returned by gyro to radians
#define RATE_TO_DEG 0.0000611 //convert angular rate value returned by gyro to degrees

//variables used in angle measurements 
float pitch, roll;//hold to final calculated angles
int gyroOffsetX, gyroOffsetY, gyroOffsetZ;//calibration values
long accelX, accelY, accelZ, accelVector;
int gyroX, gyroY, gyroZ, temp;

//used when accessing arrays that store both roll and pitch - 
#define PITCH 2 //pitch will always be in position 2
#define ROLL 1 //roll will always be in position 1

//change these if the angles have the wrong signs
#define PITCH_SIGN_CORRECTION 1
#define ROLL_SIGN_CORRECTION -1

//negative these if the correction is in the wrong directions
//multipliers used to tune the outputs for Roll
const float KP_ROLL = -2.5;
const float KI_ROLL = -0.005;
const float KD_ROLL = -2.5;
//multipliers used to tune the output for Pitch
const float KP_PITCH = -2.5;
const float KI_PITCH = -0.005;
const float KD_PITCH = -2.5;

float i[3];//holds the integral values for each axis 
float prevError[3];//used when calculating the derivitive value

//set servo output to zero position (Pulse of 1500 microseconds)
long correctedPitchOutput = 1500;
long correctedRollOutput = 1500;

//variables for measuring receiver input
//for some reason, making the array bigger makes everything work. Probably wastes memory though, so could be changed in the future
int last[4];//used to work out if the rx pulse has just started or just ended
unsigned long timerChannel[4];//holds the length of each pulse
volatile long rxInput[4];//volatile as it makes it quicker to access

//variables used to make sure each loop is 4 microseconds long
long timer;
long loopTime;
int loopCounter = 0;

boolean error = false;

//method prototypes
void setupMPU(void);
void calibrateGyro(void);
void setStartAngles(void);
void calculateAngles(void);
void printAngles(float,float);
void setServoAngles(long,long);
long calculatePID(int,float,float,float,float);

void setup() {
  //initialise integral values 
  i[1] = 0;
  i[2] = 0;

  //set  pins 8,9,10 as interrupts for reading transmitter input
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0);
  PCMSK0 |= (1 << PCINT1);
  PCMSK0 |= (1 << PCINT2);

  //set pins 6 and 5 as output, for controlling servos
  DDRD |= B01100000;

  Serial.begin(115200);//debugging 

  //setServoAngles(2000, 1500);

  //make sure there is a connection to the sensor
  do{
    error = testMPU();
    Serial.println("Dodgy connection with the MPU");//if connected for debugging, the user will be alerted
    delay(1000);
  }while(error == true);

  //set up methods
  setupMPU();
  calibrateGyro();
  setStartAngles();

  setServoAngles(1000, 1500);//this is just a good indicator for the user of when setup is complete
  delay(500);

}

void loop() {
  timer = micros();//remember the start time of the loop

  calculateAngles();//calculate the current pitch and roll of the RC plane

  //calculate the servo output needed to correct the discrepancy between pilot input and actual angle
  correctedPitchOutput = calculatePID(PITCH, (pitch * PITCH_SIGN_CORRECTION), KP_PITCH, KI_PITCH, KD_PITCH);
  correctedRollOutput = calculatePID(ROLL, (roll * ROLL_SIGN_CORRECTION), KP_ROLL, KI_ROLL, KD_ROLL);

  //only set servo angles every 5 iterations as they can only handle an update rate of 50HZ
  if (rxInput[3] > 1500 && loopCounter % 5 == 0) {
    loopCounter = 0;//reset loop counter
    setServoAngles(correctedPitchOutput, correctedRollOutput);
  } else if (rxInput[3] < 1500 && loopCounter % 5 == 0) {
    loopCounter = 0;//reset loop counter
    setServoAngles(rxInput[PITCH], rxInput[ROLL]);
  }

  loopCounter = loopCounter + 1;//incement loop counter
  Serial.println(correctedPitchOutput);//debugging 
  while (micros() - timer < 4000);//wait until 4 milliseconds have happened
}
