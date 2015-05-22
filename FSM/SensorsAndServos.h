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
#define STARTBUTTON_PIN 12
#define SCOOPSERVO_PIN 2
#define BRUSHMOTORS_PIN 1

Servo SweepServo;
Servo DropServo;
Servo ScoopServo;

//namespace ServosAndSensors
//{


void ServoSensorSetup();

void ServoSensorSetup()
{
  //DropServo.attach(13);
  SweepServo.attach(3);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TEAMSWITCH_PIN, INPUT);
  pinMode(STARTBUTTON_PIN, INPUT);
  pinMode(BRUSHMOTORS_PIN, OUTPUT);
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

int ReadColor(int ColorThresh)
{
  while(1)
  {
    int color;
    int val = analogRead(COLORSENSE_PIN);
    if (val>ColorThresh){
      color = 0; // white
    }
    else {
      color = 1; //purple
    }
    delay(100);
    int color2;
    val = analogRead(COLORSENSE_PIN);
    if (val>ColorThresh){
      color2 = 0; // white
    }
    else {
      color2 = 1; //purple
    }
    
    if (color==color2)
    {
    return color;
    break;
    }
  }
  
}

//} // end namesapce
#endif
