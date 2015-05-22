#ifndef DRIVESYSTEM_H
#define DRIVESYSTEM_H
#include <Arduino.h>
#include "FSM.h"

using namespace FSM;

int TURN_TIME_180 = 1050;

namespace DriveSystem
{
  // Drive/Turn directions
  enum Direction { FWD, REV, LEFT, RIGHT, BRAKE, COAST};

  // Pin Configuration
  #define MOTOR_L_PWM      6
  #define MOTOR_R_PWM       8
  #define MOTOR_L_DIR   11
  #define MOTOR_R_DIR   9
  
  // Motor structures
  struct Motor {
    byte pwm_pin;
    byte dir_pin;
    Direction dir;
    byte speed;
    bool fwd_state;
  };
  
  Motor motorL {MOTOR_L_PWM, MOTOR_L_DIR, FWD, 0, HIGH};
  Motor motorR {MOTOR_R_PWM, MOTOR_R_DIR, FWD, 0, LOW};
  
  /*      Motor Functions      */
  void runMotor(Motor&, Direction, byte);
  
  // Run a motor with a given speed (0 - 255) and direction (FWD,REV).
  void runMotor(Motor& m, Direction dir, byte spd)
  {
    m.dir = dir;
    m.speed = spd;
    analogWrite(m.pwm_pin,spd);
    digitalWrite(m.dir_pin,(dir != REV) ? m.fwd_state : !m.fwd_state);
  }
  
  void stopMotor(Motor& m)
  {
    // Not sure if this is how you brake.
    m.dir = BRAKE;
    m.speed = 0;
    analogWrite(m.pwm_pin,0);
    digitalWrite(m.dir_pin,0);
  }
  
  void coastMotor(Motor& m)
  {
    stopMotor(m); // for right now
  }
  
  void moveForward(byte spd)
  {
    runMotor(motorL,FWD,spd);
    runMotor(motorR,FWD,spd);
  }
  
  void moveReverse(byte spd)
  {
    runMotor(motorL,REV,spd);
    runMotor(motorR,REV,spd);
  }
  
  void moveTurnLeft(byte spd)
  {
    runMotor(motorL,REV,spd);
    runMotor(motorR,FWD,spd);
  }
  
  void moveTurnRight(byte spd)
  {
    runMotor(motorL,FWD,spd);
    runMotor(motorR,REV,spd);
  }
  void moveBrake() {stopMotor(motorL); stopMotor(motorR);}
  void moveCoast() {coastMotor(motorL); coastMotor(motorR);}
  void moveSetSpeed(byte spd)
  {
    runMotor(motorL,motorL.dir,motorL.speed);
    runMotor(motorL,motorL.dir,motorL.speed);
  }
  



  /* ######################## HELPER FUNCTIONS ########################## */

  // Some functions to make mirroring easy
  inline void moveTurnLeftAsPurple() { 
    if (st::team_color == PURPLE) 
      moveTurnLeft(st::spd);
    else
      moveTurnRight(st::spd);
  }

  inline void moveTurnRightAsPurple() { 
    if (st::team_color == PURPLE) 
      moveTurnRight(st::spd);
    else
      moveTurnLeft(st::spd);
  }

  void moveTurnRightAsPurple(float deg)
  {
     moveTurnRightAsPurple();
     waitTime((deg / 180.0) * TURN_TIME_180);
     moveBrake();
  }
  void moveTurnLeftAsPurple(float deg)
  {
    moveTurnLeftAsPurple();
    waitTime((deg / 180.0) * TURN_TIME_180);
    moveBrake();
  }

  //Move forward block backwards by squares

  void moveForwardNBlocks(int n)
  {
    moveForward(st::spd);
    waitBumps(n);
    waitTime(500);
    moveBrake();
  }

  void moveReverseNBlocks(int n)
  {
    moveReverse(st::spd);
    waitBumps(n);
    waitTime(500);
    moveBrake();
  }
}
#endif




