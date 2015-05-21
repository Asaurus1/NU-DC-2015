#ifndef SENSORSANDSERVOS_H
#define SENSORSANDSERVOS_H
#include <Arduino.h>
#include <Servo.h>

#define COLORSENSE_PIN A1 
#define SWEEPSENSE_PIN A4
#define TEAMSWITCH_PIN 18
#define SWEEPSERVO_PIN 3
#define DROPWERVO_PIN 13
#define LED_PIN 14

Servo SweepServo;
Servo DropServo;

//namespace ServosAndSensors
//{


void ServoSensorSetup();

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
    color = 0; // white
  }
  else {
    color = 1; //purple
  }
  return color;
  
}

//} // end namesapce
#endif
