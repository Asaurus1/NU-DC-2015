#ifndef SENSORSANDSERVOS_H
#define SENSORSANDSERVOS_H
#include <Arduino.h>
#include "BumpSensor.h"
#include <Servo.h>
#include "FSM.h"

#define COLOR_SENSOR_ARRAY_SIZE 10

#define COLORSENSE_PIN A1
#define TEAMSWITCH_PIN 18
#define DROPSERVO_PIN 13
#define LED_PIN 14
#define STARTBUTTON_PIN 12
#define BUMPSENSOR_PIN 17
#define SCOOPSERVO_PIN 2
#define BRUSHMOTORS_PIN 1
#define FRONTBUMP_PIN 10
#define CANDYCANE_ENABLE 1
#define CANDYCANE_PHASE 3

#define CANDYCANE_FWD 0

//Peripherals
BumpSensor tableBump(BUMPSENSOR_PIN);
BumpSensor frontBump(FRONTBUMP_PIN);
Servo ScoopServo;
Servo DropServo;

void candyCanesOn(int dir)
{
  digitalWrite(CANDYCANE_PHASE, dir);
  analogWrite(CANDYCANE_ENABLE, 255);
}

void candyCanesOff()
{
  analogWrite(CANDYCANE_ENABLE, 0);
}

void scoopDown() 
{
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(180);
  delay(700);
  ScoopServo.detach();
  candyCanesOn(CANDYCANE_FWD); //Run forwards
}

void scoopUp()
{
  candyCanesOff();
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(0);
}

void scoopDump()
{
  scoopUp();
  delay(3000);
  scoopDown();
}

void ServoSensorSetup()
{
  //DropServo.attach(13);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TEAMSWITCH_PIN, INPUT);
  pinMode(STARTBUTTON_PIN, INPUT);
  pinMode(BRUSHMOTORS_PIN, OUTPUT);
  pinMode(BUMPSENSOR_PIN, INPUT);
  pinMode(FRONTBUMP_PIN, INPUT);
  pinMode(CANDYCANE_PHASE, OUTPUT);
  candyCanesOff();
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

  CSAvg += (val-CSA[CSAIndex])/COLOR_SENSOR_ARRAY_SIZE;
  CSA[CSAIndex] = val;
  CSAIndex = (CSAIndex + 1) % COLOR_SENSOR_ARRAY_SIZE;

  if (CSAvg > ColorThresh)
    return WHITE;
  else
    return PURPLE;  
}

#endif

