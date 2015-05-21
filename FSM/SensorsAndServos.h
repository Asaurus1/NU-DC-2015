#ifndef SENSORSANDSERVOS_H
#define SENSORSANDSERVOS_H
#include <Arduino.h>
#include "BumpSensor.h"
#include <Servo.h>

#define COLORSENSE_PIN A1 
#define SWEEPSENSE_PIN A4
#define TEAMSWITCH_PIN 18
#define SWEEPSERVO_PIN 3
#define SCOOPSERVO_PIN 8
#define DROPSERVO_PIN 13
#define LED_PIN 14

// Define some colors, just for the hell of it
#define WHITE 0
#define PURPLE 1
#define BLACK 2

//Peripherals
BumpSensor tableBump(5);
Servo SweepServo;
Servo ScoopServo;
Servo DropServo;

void scoopDown() 
{
  ScoopServo.write(13);
  delay(1000);
  ScoopServo.detach();
}

void scoopUp()
{
  ScoopServo.attach(SCOOPSERVO_PIN);
  ScoopServo.write(160);
  delay(1000);
}

void ServoSensorSetup()
{
  //DropServo.attach(13);
  SweepServo.attach(3);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TEAMSWITCH_PIN, INPUT);
}

int ReadColor(int ColorThresh)
{
  int color;
  int val = analogRead(COLORSENSE_PIN);
  if (val>ColorThresh){
    color = WHITE; // white
  }
  else {
    color = 1; //purple
  }
  return color;
  
}

#endif

