#include <Wire.h> // for communication with IMU
#include <I2Cdev.h> // for IMU calibration in particular
#include <Kalman.h> // Source: https://github.com/TKJElectronics/KalmanFilter
#include "KalmanJ.h" // put declarations here but haven't put definitions into .c or .cpp file yet
#include <PID_v1.h>

#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf
#define MPU6050_RA_XA_OFFS_H        0x06 // accel X register offset
#define MPU6050_RA_YA_OFFS_H        0x08 // accel Y register offset
#define MPU6050_RA_ZA_OFFS_H        0x0A // accel Z register offset

/* IMU Data */
extern const uint8_t IMUAddress;
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
double gyroXrate, gyroYrate;
double roll, pitch, dt;
int16_t tempRaw;

double gyroXangle, gyroYangle; // Angle calculate using the gyro only
double compAngleX, compAngleY; // Calculated angle using a complementary filter
double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data

Kalman kalmanX; Kalman kalmanY; // Create the Kalman instances

/* PID Data */
double Setpoint, Input, Output;
double Kp=1, Ki=.05, Kd=.25;
PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);

int pwmPin=6;
int motAdir1=7; int motAdir2=8;
int motBdir1=12; int motBdir2=13;
int stby=4;

void setup() {
  Serial.begin(115200);
  initMPUcomm();

  // calibration based on horizontal
  I2Cdev::writeWord(IMUAddress, MPU6050_RA_XA_OFFS_H, -3565);
  I2Cdev::writeWord(IMUAddress, MPU6050_RA_YA_OFFS_H, 250);
  I2Cdev::writeWord(IMUAddress, MPU6050_RA_ZA_OFFS_H, 750);
  
  delay(100); // Wait for sensor to stabilize
  initKalman();
  timer = micros();

    //initialize PID stuff
  Input=roll; Setpoint=0; // this should become whatever we define at boot after calibration
  myPID.SetMode(AUTOMATIC);

  // initialize motor?
  /*
  digitalWrite(stby,HIGH);
  digitalWrite(motAdir1,HIGH);
  digitalWrite(motAdir2,LOW);
  digitalWrite(motBdir1,HIGH); // motors are physically wired backwards so coding is simpler
  digitalWrite(motBdir2,LOW);
  analogWrite(pwmPin,155);
  */
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

void initMPUcomm(){
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
}

void initKalman(){
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

void getMPUdata(){
  /* Update all the values */
  while (i2cRead(0x3B, i2cData, 14))
    ;
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
  gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
  gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
  gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);
}

void getAccelAngle(){
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

void getKalmanAngle(){
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

void getCompAngle(){
  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;
}

void printRawData(){
  Serial.print(accX); Serial.print("\t");
  Serial.print(accY); Serial.print("\t");
  Serial.print(accZ); Serial.print("\t");

  Serial.print(gyroX); Serial.print("\t");
  Serial.print(gyroY); Serial.print("\t");
  Serial.print(gyroZ); Serial.print("\t");

  Serial.print("\t");
}

void printAngles(){
  Serial.print(roll); Serial.print("\t");
  //Serial.print(gyroXangle); Serial.print("\t");
  Serial.print(compAngleX); Serial.print("\t");
  Serial.print(kalAngleX); Serial.print("\t");

  Serial.print("\t");

  Serial.print(pitch); Serial.print("\t");
  //Serial.print(gyroYangle); Serial.print("\t");
  Serial.print(compAngleY); Serial.print("\t");
  Serial.print(kalAngleY); Serial.print("\t");
}
