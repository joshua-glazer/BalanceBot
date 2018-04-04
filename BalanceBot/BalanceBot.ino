/* Copyright (C) 2018 Joshua Glazer. All rights reserved.

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
#include <PID_v1.h>

/* IMU Data */
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

/* PID Data */
double Setpoint, Input, Output;
double Kp=1, Ki=.05, Kd=.25; // initial values
//int pPot=0, iPot=1, dPot=2; // to adjust PID in real time
PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);

int pwmA=5, pwmB=6, stby=4; // pwm controls speed, stby allows motors to be used
int motAdir1=7, motAdir2=8;
int motBdir1=12, motBdir2=13;

void setup() {
  Serial.begin(115200);
  initMPUcomm();  
  delay(100); // Wait for sensor to stabilize
  initKalman();
  timer = micros();

  /* calibration loop */
  
  /* initialize PID stuff */
  Input=roll; Setpoint=0; // this should become whatever we define at boot after calibration
  myPID.SetMode(AUTOMATIC);

  /* initialize motor */
  digitalWrite(stby,HIGH);
  digitalWrite(motAdir1,HIGH);
  digitalWrite(motAdir2,LOW);
  digitalWrite(motBdir1,HIGH); // motors are physically wired backwards so coding is simpler
  digitalWrite(motBdir2,LOW);
  //analogWrite(pwmPin,155);
}

void loop() {
  /* Angle processing */
  getMPUdata();
  dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();
  getAccelAngle();

  // TODO: Make routine to define a 0 angle
  // loop the part above in setup and average the result to find neutral position?
  
  gyroXrate = gyroX / 131.0; // Convert to deg/s
  gyroYrate = gyroY / 131.0; // Convert to deg/s
  getKalmanAngle();
  getCompAngle();

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;

  /* PID adjustment */
  // a value of 512 should correspond to the constants calculated in theory
  /*
  Kp = map(analogRead(pPot),0,1023,x,y);
  Ki = map(analogRead(iPot),0,1023,x,y);
  Kd = map(analogRead(dPot),0,1023,x,y);
  myPID.SetTunings(Kp, Ki, Kd);
  */
  
  /* Motor control */
  // motor needs to change direction based on angle!
  /*
  Input=roll;
  myPID.Compute();
  analogWrite(pwmPin,Output);
  Serial.println(Output);
  int motorSpeed=map(roll,0,90,00,200);
  analogWrite(pwmPin,motorSpeed);
  */

  /* Print Data */
  //printRawData();
  printAngles();
  Serial.print("\r\n");
  delay(2);
}
