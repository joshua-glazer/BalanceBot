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
 
#include <I2Cdev.h> // for IMU calibration in particular

#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf
#define X_REGISTER_OFFSET 0x06 // accel X register offset
#define Y_REGISTER_OFFSET 0x08 // accel Y register offset
#define Z_REGISTER_OFFSET 0x0A // accel Z register offset
#define X_VALUE_OFFSET -3565 // accel X offset value
#define Y_VALUE_OFFSET 250 // accel Y offset value
#define Z_VALUE_OFFSET 750 // accel Z offset value, not 100% perfect

uint8_t i2cData[14]; // Buffer for I2C data

void initMPUcomm(){ // initializes I2C communication with the MPU6050. Uses multiple I2C libraries.
  Wire.begin();
#if ARDUINO >= 157
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz
#else
  TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
#endif

  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode

  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor"));
    while(1)
      ;
  }

  // calibration based on horizontal
  I2Cdev::writeWord(IMUAddress, X_REGISTER_OFFSET, X_VALUE_OFFSET);
  I2Cdev::writeWord(IMUAddress, Y_REGISTER_OFFSET, Y_VALUE_OFFSET);
  I2Cdev::writeWord(IMUAddress, Z_REGISTER_OFFSET, Z_VALUE_OFFSET);
}

void initKalman(){ // initializes the kalman filter
  /* Set kalman and gyro starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);

  getAccelAngle();

  kalmanX.setAngle(roll); // Set starting angle
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;
}

void getMPUdata(){ // obtains accel/gyro data from the MPU6050
  /* Update all the values */
  while (i2cRead(0x3B, i2cData, 14))
    ;
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
  //tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]); // for temperatures
  gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
  gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
  gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);
}

void getAccelAngle(){ // calculates the angle directly from the accelerometer
  getMPUdata();
  dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();
  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  roll  = atan2(accY, accZ) * RAD_TO_DEG;
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif
}

void getKalmanAngle(){ // calculates the angle using a kalman filter with inputs from external variables gyroXrate/gyroYrate
#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  }
  else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  }
  else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
#endif

  gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * dt;
  //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  //gyroYangle += kalmanY.getRate() * dt;
}

void getCompAngle(){ // calculates the angle using a complementary filter with inputs from external variables gyroXrate/gyroYrate
  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;
}

void calibrateSensor(){ // waits for segway to be within +/- 5 degrees upright and then calculates a neutral position
  while(abs(pitch)>5) getAccelAngle(); // wait for bot to be upright
  neutralPos=pitch;
  for (int i=0;i<samples;i++){
    getAccelAngle();
    neutralPos += pitch;
    neutralPos /= 2;
    Serial.print("pitch: ");Serial.print(pitch);Serial.print("\t");
    Serial.print("average: ");Serial.println(neutralPos);
    delay(2);
  }
  // blinks to alert user that calibration is complete
  int blinkTime = 125;
  digitalWrite(LED_BUILTIN, HIGH); delay(blinkTime);
  digitalWrite(LED_BUILTIN, LOW); delay(blinkTime);
  digitalWrite(LED_BUILTIN, HIGH); delay(blinkTime);
  digitalWrite(LED_BUILTIN, LOW); delay(blinkTime);
  digitalWrite(LED_BUILTIN, HIGH); delay(blinkTime);
  digitalWrite(LED_BUILTIN, LOW); delay(blinkTime);
}
void printRawData(){
  //Serial.print(accX); Serial.print("\t");
  Serial.print("accY raw: "); Serial.print(accY); Serial.print("\t");
  //Serial.print(accZ); Serial.print("\t");

  //Serial.print(gyroX); Serial.print("\t");
  Serial.print("gyroY raw: "); Serial.print(gyroY); Serial.print("\t");
  //Serial.print(gyroZ); Serial.print("\t");

  Serial.print("\t");
}

void printAngles(){
  //Serial.print(roll); Serial.print("\t");
  //Serial.print(gyroXangle); Serial.print("\t");
  //Serial.print(compAngleX); Serial.print("\t");
  //Serial.print(kalAngleX); Serial.print("\t");

  //Serial.print("\t");

  Serial.print("Accel: "); Serial.print(pitch); Serial.print("\t");
  Serial.print("Gyro: "); Serial.print(gyroYangle); Serial.print("\t");
  Serial.print("Comp: "); Serial.print(compAngleY); Serial.print("\t");
  Serial.print("Kalman: "); Serial.print(kalAngleY); Serial.print("\t");
}
