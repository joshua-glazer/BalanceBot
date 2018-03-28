#ifndef _KalmanJ_h_
#define _KalmanJ_h_

void getAccelAngle(); // loop() only, calculates x/y angles from accelerometer only
void getKalmanAngle(); // loop() only, calculates x/y angles using Kalman filter
void getCompAngle(); // loop() only, calculates x/y angles using complementary filter

void initMPUcomm(); // setup() only, initializes I2C for MPU6050
void initKalman(); // setup() only, takes first MPU6050 values and initializes Kalman filter

void getMPUdata(); // loop() only, reads all 6 data values from MPU6050
void printRawData(); // loop() only, prints raw MPU6050 values
void printAngles(); // loop() only, prints processed angles

#endif
