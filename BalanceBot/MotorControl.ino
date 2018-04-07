/* Copyright (C) 2018 Justin Rochefort. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Justin Rochefort
 Web      :  xxxxxxxxxxx
 e-mail   :  xxxxxxxxxxx
 */

/*error variables*/
volatile float output = 0;
volatile float error = 0;
volatile float errorSum = 0;
volatile float previousError = 0;

/*Battery Voltage*/
int batteryV = 12;

void PIDcontrol(){
  
  //error
  error = neutralPos - kalAngleY;
  errorSum = errorSum+error;
  //Serial.print("error: ");
  //Serial.println(error);

  //Set motor direction
  if (error > 0){
    //Clockwise  
    digitalWrite(motAdir1, LOW);
    digitalWrite(motAdir2, HIGH);    
    digitalWrite(motBdir1, HIGH);
    digitalWrite(motBdir2, LOW);
    //Serial.println("positive error");
  }
  else{
    //counter Clockwise  
    digitalWrite(motAdir1, HIGH);
    digitalWrite(motAdir2, LOW);
    digitalWrite(motBdir1, LOW);
    digitalWrite(motBdir2, HIGH);
  }

  //Output
  Serial.print("error: ");Serial.print(error);Serial.print("\t");
  output = Kp*error + Ki*errorSum + Kd*(error - previousError);
  output=abs(output)/1000;
  if (output>12) output=12;
  if (output<2) output=0;
  Serial.print("output: ");Serial.print(output);Serial.print("\t");
  float pwmSignal = (255/batteryV)*output;
  analogWrite(PWMA,pwmSignal);
  analogWrite(PWMB,pwmSignal);
  Serial.print("pwm: ");Serial.println(pwmSignal);
  //Reset error
  previousError = error;
}

