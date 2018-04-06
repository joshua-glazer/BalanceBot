/* Copyright (C) 2018 Joshua Glazer, Justin Rochefort. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Joshua Glazer
 Web      :  https://github.com/vashmata
 e-mail   :  vashmata@gmail.com
 */
 
#include <Wire.h> // for communication with IMU
#include <Kalman.h> // Source: https://github.com/TKJElectronics/KalmanFilter
// #include <PID_v1.h> // not implemented in the end, a homemade PID is used

/* IMU Variables */
double accX, accY, accZ;
//int16_t tempRaw; // for temperatures
double gyroX, gyroY, gyroZ;

double gyroXrate, gyroYrate;
double roll, pitch, dt;
uint32_t timer;

double gyroXangle, gyroYangle; // Angle calculate using the gyro only
double compAngleX, compAngleY; // Calculated angle using a complementary filter
double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter

Kalman kalmanX; Kalman kalmanY; // Create the Kalman instances

/* calibration routine */
double neutralPos; // point of equilibrium calculated by calibrateSensor()
int samples = 1000; // number of data points to take. this is a pretty good number

/* Motor control pins */
int pwmA=5, pwmB=6, stby=4; // pwm controls speed, stby allows motors to be used
int motAdir1=7, motAdir2=8;
int motBdir1=11, motBdir2=12;

/* PID Data */ // scrapped because using homemade stuff
//double Setpoint, Input, Output;
//double Kp=1, Ki=.05, Kd=.25; // initial values
//int pPot=0, iPot=1, dPot=2; // to adjust PID in real time
//PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);

/* new PID Variables */
//double Kp=1, Ki=.05, Kd=.25; // initial values

void setup() {
  /* initialize motor pins */
  pinMode(stby, OUTPUT);
  pinMode(motAdir1, OUTPUT);pinMode(motBdir1, OUTPUT);
  pinMode(motAdir2, OUTPUT);pinMode(motBdir2, OUTPUT);
  delay(10);
 
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // for calibration
  
  /* accelerometer/gyroscope setup */
  initMPUcomm(); // initializes I2C communication with MPU6050
  delay(100); // Wait for sensor to stabilize
  initKalman(); // initializes kalman filter
  timer = micros();
  calibrateSensor(); // waits for segway to be within +/- 5 degrees upright and then calculates a neutral position 
  
  /* initialize PID stuff */ // ignore this, our own PID implementation is used
  //Input=roll; Setpoint=0; // this should become whatever we define at boot after calibration
  //myPID.SetMode(AUTOMATIC);

  /* new PID initialization */
  // we use neutralPos as the desired angle. it's calculated by calibrateSensor()
  
  /* initialize motors */ //currently for testing purposes, replace this when motor control is implemented
  digitalWrite(stby,HIGH);
  digitalWrite(motAdir1,HIGH);
  digitalWrite(motAdir2,LOW);
  digitalWrite(motBdir1,HIGH); // motors are physically wired backwards so coding is simpler
  digitalWrite(motBdir2,LOW);
  analogWrite(pwmA,100); analogWrite(pwmB,100);
}

void loop() {
  /* Angle processing */
  getAccelAngle();
  
  gyroXrate = gyroX / 131.0; // Convert to deg/s
  gyroYrate = gyroY / 131.0; // Convert to deg/s
  
  getKalmanAngle();
  getCompAngle();

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;

  /* PID adjustment */ // this is intended for adjustment during segway opetation
  // a value of 512 should correspond to the constants calculated in theory
  /*
  Kp = map(analogRead(pPot),0,1023,x,y);
  Ki = map(analogRead(iPot),0,1023,x,y);
  Kd = map(analogRead(dPot),0,1023,x,y);
  myPID.SetTunings(Kp, Ki, Kd);
  */
  
  /* Motor control */ // this was scrapped, we will be using a homemade implementation
  // motor needs to change direction based on angle!
  /*
  Input=roll;
  myPID.Compute();
  analogWrite(pwmPin,Output);
  Serial.println(Output);
  int motorSpeed=map(roll,0,90,00,200);
  analogWrite(pwmPin,motorSpeed);
  */

  /* New Motor Control */

  /* Print Data */
  //printRawData(); Serial.print("\r\n");
  printAngles(); Serial.print("\r\n");
  delay(2);
}
