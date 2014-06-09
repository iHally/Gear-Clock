/*
Hallys Kludge Gear Clock Code

This Code has been tested on an uno(R3) and a Pro-Mini Clone(16Mhz 328P) and used with the following components.
The RTC: http://www.adafruit.com/product/264
The Stepper motor driver: http://www.adafruit.com/products/807 or http://www.adafruit.com/datasheets/l293d.pdf
The Stepper motor: http://www.adafruit.com/products/858


Make sure the follwing library is installed before compiling
The Adafruit RTC library can be found on the Adafruit website: https://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit/arduino-library
*/

#include <Stepper.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;

int in1Pin = 12;
int in2Pin = 11;
int in3Pin = 10;
int in4Pin = 9;
int motorenable = 13;
//loop time int
int x = 0;
//set how many steps the motor needs to turn every 5 min. My motor has 512 steps per revolution which is 42.6666 every 5 min so we round up
int fivemin = 43;
//the correction value for the rounding error every hour 512-(43*12) = -4
int fiveminerror = -4;
int motorsteps = 0;

//set stepper motor speed
int motorspeed = 5;

Stepper motor(512, in1Pin, in2Pin, in3Pin, in4Pin);  

void setup()
{
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  //set enable pins for stepper driver chip
  pinMode(motorenable, OUTPUT);

  // this line is for Leonardo's, it delays the serial interface
  // until the terminal window is opened
  while (!Serial);
  
Serial.begin(57600);
    Wire.begin();
    RTC.begin();

  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

//we need to find out what time it is now in 5 min steps and set the correct x value for later in the code
DateTime now = RTC.now();
if(now.minute()>=5){
  x = 10;
}
if(now.minute()>=10){
  x = 15;
}
if(now.minute()>=15){
  x = 20;
}
if(now.minute()>=20){
  x = 25;
}
if(now.minute()>=25){
  x = 30;
}
if(now.minute()>=30){
  x = 35;
}
if(now.minute()>=35){
  x = 40;
}
if(now.minute()>=40){
  x = 45;
}
if(now.minute()>=45){
  x = 50;
}
if(now.minute()>=50){
  x = 55;
}
if(now.minute()>=55){
  x = 0;
}


  motor.setSpeed(motorspeed);
  //x=now.second();
  //x=0;



}

void loop()
{
  //check for serial input
  if (Serial.available())
  {
    //this will get an integer from serial and move the motor accordingly ie -5 or 512
    int steps = Serial.parseInt();
    //enable the stepper motor chip
    digitalWrite(motorenable, HIGH);
    //move the motor
    motor.step(steps);
    //dissable the stepper motor chip
    digitalWrite(motorenable, LOW);
  }
  
  //get the current time from rtc
  DateTime now = RTC.now();
  
  //debugging line
  //Serial.println(now.minute(), DEC);
  
  //compare our steped time agains the actual time 
  if(x==now.minute()){
    //if the step increment has passed then move the step forward again
    x=5+now.minute();
    //set motor step increment
    motorsteps = fivemin;
    
    //if we count to or past 60 set back to zero
      if(x>=60){
        x=0;
        //correct the position of the stepper motor every hour
        motorsteps = fivemin + fiveminerror;
      }
    
    //enable the stepper motor chip
    digitalWrite(motorenable, HIGH);
    //move the motor
    motor.step(motorsteps);
    //dissable the stepper motor chip
    // since there is almost no force on the stepper motor turning it off will save LOTS of power
    digitalWrite(motorenable, LOW);
    
    /*more debugging
    //print the time to serial
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    */
  }
  //chill out for 10 seconds before running the loop again. no need to keep annoying the RTC constantly
  delay(10000);
}
