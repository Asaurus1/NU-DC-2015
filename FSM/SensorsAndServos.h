#ifndef SENSORSANDSERVOS_H
#define SENSORSANDSERVOS_H
#include <Arduino.h>
#include "BumpSensor.h"
#include <Servo.h>
#include "FSM.h"

#define COLOR_SENSOR_ARRAY_SIZE 10

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

//Peripherals
BumpSensor tableBump(BUMPSENSOR_PIN);
Servo SweepServo;
Servo ScoopServo;
Servo DropServo;

void scoopDown() 
{
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(180);
  delay(700);
  ScoopServo.detach();
}

void scoopUp()
{
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(0);
}

void ServoSensorSetup()
{
  //DropServo.attach(13);
  SweepServo.attach(SWEEPSERVO_PIN);
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
  scoopUp();
  delay(3000);
  scoopDown();
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

int CSA[COLOR_SENSOR_ARRAY_SIZE] = {};
int CSAIndex = 0;
int CSAvg = 0;

int ReadColor(int ColorThresh)
{
  int color;
  int val = analogRead(COLORSENSE_PIN);

  CSAvg -= CSA[CSAIndex]/COLOR_SENSOR_ARRAY_SIZE;
  CSAvg += val/COLOR_SENSOR_ARRAY_SIZE;
  CSA[CSAIndex] = val;
  CSAIndex = (CSAIndex + 1) % COLOR_SENSOR_ARRAY_SIZE;

  if (CSAvg > ColorThresh)
    return WHITE;
  else
    return PURPLE;  
}

#endif

