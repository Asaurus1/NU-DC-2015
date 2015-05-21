#ifndef SENSORSANDSERVOS_H
#define SENSORSANDSERVOS_H
#include <Arduino.h>
#include "BumpSensor.h"
#include <Servo.h>

#define COLORSENSE_PIN A1 
#define SWEEPSENSE_PIN A4
#define TEAMSWITCH_PIN 18
#define SWEEPSERVO_PIN 3
#define DROPSERVO_PIN 13
#define LED_PIN 14
#define STARTBUTTON_PIN 12
#define BUMPSENSOR_PIN 17
#define SCOOPSERVO_PIN 2
#define BRUSHMOTORS_PIN 1

// Define some colors, just for the hell of it
#define WHITE 0
#define PURPLE 1
#define BLACK 2

//Peripherals
BumpSensor tableBump(BUMPSENSOR_PIN);
Servo SweepServo;
Servo ScoopServo;
Servo DropServo;

void scoopDown() 
{
  ScoopServo.write(13);
  delay(2000);
  ScoopServo.detach();
}

void scoopUp()
{
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(160);
  delay(2000);
}

void ServoSensorSetup()
{
  //DropServo.attach(13);
  SweepServo.attach(3);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TEAMSWITCH_PIN, INPUT);
  pinMode(STARTBUTTON_PIN, INPUT);
  pinMode(BRUSHMOTORS_PIN, OUTPUT);
  pinMode(BUMPSENSOR_PIN, INPUT);
}

void ScoopServoWrite(int angle)
{
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(angle);
}

void ScoopDump()
{
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(0);
  delay(3000);
  ScoopServo.write(180);
  delay(200);
  ScoopServo.detach();
}

void ScoopServoOff()
{
  ScoopServo.detach();
}

inline int ReadStartButton()
{
  int val=digitalRead(STARTBUTTON_PIN);
  return val;
  
}

int CSA[COLOR_SENSOR_ARRAY_SIZE];
int CSAIndex = 0;
int CSAvg = 60;

int ReadColor(int ColorThresh)
{
  int color;
  int val = analogRead(COLORSENSE_PIN);

  CSAvg -= CSA[CSAIndex]/COLOR_SENSOR_ARRAY_SIZE;
  CSAvg += val/COLOR_SENSOR_ARRAY_SIZE;
  CSAIndex = (CSAIndex + 1) % COLOR_SENSOR_ARRAY_SIZE;

  if (CSAvg > COlorThresh)
    return WHITE;
  else
    return PURPLE;  
}

#endif

