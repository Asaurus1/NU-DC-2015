//ScoopDump();
#include "DriveSystem.h"
#include "SensorsAndServos.h"
#include <Servo.h>
#include "BumpSensor.h"
#include "FSM.h"

using namespace DriveSystem;

#define TURN_TIME_90 2500
#define MOVE_TIME_1B 2500
#define COLOR_READ_INT 50
#define DEBUG_INT			 1000
long GAME_END_TIME = ((3 * 60 * 1000) - (10 * 1000));
long GAME_ALMOST_OVER_TIME = (GAME_END_TIME - (25 * 1000));

#define AFTER_INIT_STATE s_run

namespace FSM {
  /********************* FSM Actions *********************/

  //Special Init Function
  void s_init()
  {
    const int tolerance = 80;
    st::team_color = digitalRead(TEAMSWITCH_PIN);
    int val = analogRead(COLORSENSE_PIN);
    if (st::team_color == WHITE) //White, thus we're on purple
    {
      st::color_threshold=val+tolerance;
    }
    else
    {
      st::color_threshold=val-tolerance;
    }

    // Read color 10 times to populate color array.
    for (int i = 0; i < COLOR_SENSOR_ARRAY_SIZE; i++)
      ReadColor(st::color_threshold);

    // Wait to start  
    digitalWrite(LED_PIN, HIGH);
    while(!digitalRead(STARTBUTTON_PIN))
    {
      ;
    }

    // Set the official game clock:
    unsigned long start_time = millis();
    Serial.println(GAME_END_TIME);
    Serial.println(start_time);
    GAME_END_TIME +=start_time;
    GAME_ALMOST_OVER_TIME +=start_time;
    
    // drop the scoop
    scoopDown();
    waitTime(500);
    digitalWrite(LED_PIN, LOW);
    next_state = AFTER_INIT_STATE;
    next();
  }

  void s_run()
  {
    // Step 1
    moveForwardNBlocks(3);
    waitTime(100);
    scoopUp();
    waitTime(2500);
    moveTurnRightAsPurple(90);
    scoopDown();
    moveReverse(255); // straighten on wall
    waitTime(750);

    // Step 2
    moveForwardNBlocks(3);

    //Step 3
    scoopUp();
    waitTime(2500);
    moveTurnRightAsPurple(90);
    scoopDown();
    moveReverse(255); //straighten on wall
    waitTime(750); 
    moveForwardNBlocks(1);
    moveTurnRightAsPurple(90);
    waitTime(300);
    moveForwardNBlocks(1);
    scoopUp();
    waitTime(2500);
    moveTurnLeftAsPurple(90);
    scoopDown();
	waitTime(500);

    // Step 4
    moveForward(255);
    waitFrontBump(8000);
    moveReverse(255); // hit the wall
    waitTime(50);
    moveBrake();
    scoopDump();
    moveTurnLeftAsPurple(90); // should be 90...
    waitTime(1000);

    // Step 5
    moveForwardNBlocks(2);
    scoopDump();
    moveBrake();
    waitTime(5000);

    // Finally
    digitalWrite(LED_PIN,LOW);
    //waitTime(5000);
    next_state = s_gameEnd;
    next();
  }

  void s_gameEnd()
  {
    Serial.println("Begin GAME End");
    moveBrake();
    waitTime(500);
    moveForward(255);
    if(!waitColor(st::team_color, 5000))
    {
       Serial.println("TIMEOUT");
      moveBrake();
      waitTime(500);
      moveReverse(255);
      if (!waitColor(st::team_color, 5000))
      {
        moveBrake();
        waitTime(200);
        moveTurnLeftAsPurple(90);
        moveForward(255);
        if (!waitColor(st::team_color, 5000))
        {
          moveBrake();
          waitTime(200);
          moveReverse(255);
          waitColor(st::team_color,3000);
        }
      }
    }
    else
    {
      Serial.println("NO TIMEOUT");
      moveBrake();
      waitTime(500);
      moveForward(255);
      waitTime(300);
      moveBrake();
    }
    DropServo.attach(13);
    DropServo.write(180);
    waitTime(200);
    DropServo.detach();
    scoopDump();
    
    next_state = s_done;
    next();
  }

  void s_done() // also functions as a color-sensing loop, if you just need to check colors
  {
    ;
  }
  
  // TEST ACTIONS -----------------------------------------
  void s_testRectangle()
  {
    while(true)
    {
      moveForwardNBlocks(2);
      waitTime(500);
      moveTurnLeftAsPurple(90);
    }
  }
  
  void s_testTurn()
  {
    while(true)
    {
      moveTurnLeftAsPurple(90);
      waitTime(500);
      moveTurnRightAsPurple(90);
      waitTime(500);
      moveTurnLeftAsPurple(180);
      waitTime(500);
      moveTurnRightAsPurple(180);
      waitTime(500);
    }
  }

  /********************* FSM WaitFunctions *********************/
  bool waitTime(int ms)
  {
    unsigned long start = millis();
    while (millis() <= start + ms)
    {
      check_events();
    }
    return true;
  }

  bool waitColor(int color, int timeout)
  {
    unsigned long start = millis();
    while (st::current_square_color != color)
    {
      check_events();
      if (timeout != 0 && millis() > (start + timeout))
        return false;
    }
    return true;
  }

  bool waitColorCount(int count)
  {
    startColorCount();
    while (colorCount() < count)
    {
      check_events();
    }
    return true;
  }

  bool waitBumps(int count, int timeout)
  {
    unsigned long start = millis();
    startBumpCount();
    while (bumpCount() < count)
    {
      check_events();
      if (timeout != 0 && millis() > (start + timeout))
        return false;
    }
    return true;
  }
  bool waitFrontBump(int timeout)
  {
    unsigned long start = millis();
    while(!digitalRead(FRONTBUMP_PIN))
    {
      check_events();
      if (timeout !=0 && millis() > start+timeout)
      {
        return false;
      }
    }
    return true;

  }

  /*********************** FSM Events ********************/

  void check_events()
  {
    unsigned long now = millis();
    //Color Polling
    if (now > st::last_color_read_time + COLOR_READ_INT && st::bumpSensorColorSenseDelay <= 0)
    {
      st::last_color_read_time = now;
      st::previous_square_color = st::current_square_color;
      st::current_square_color = ReadColor(st::color_threshold);
      /*Serial.print("COLOR "); 
      Serial.print(st::current_square_color ? "P" : "W");*/
      delay(5);

      if (st::current_square_color != st::previous_square_color) // Double check
        st::colorchanges++;

      digitalWrite(LED_PIN,!st::current_square_color); // Write color for debug purpose
    }
    else if (st::bumpSensorColorSenseDelay > 0)
      st::bumpSensorColorSenseDelay--;

      /* Bump count */
      tableBump.poll();
    if (tableBump.pressed())
    {
      st::bumps++;
      st::bumpSensorColorSenseDelay = 70;
    }

    /* Game Timers */
    if ((game_time() > GAME_END_TIME) && (current_state != s_done))
    {
      Serial.print("GAME END! AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH!");
      DropServo.attach(13);
      DropServo.write(180);
      waitTime(200);
      DropServo.detach();
      scoopDump();
      
      next_state = s_done;
      next_immediately();
    }
    else if (game_time() > GAME_ALMOST_OVER_TIME && current_state != s_done && current_state != s_gameEnd)
    {
      next_state = s_gameEnd; // do nothing
      next_immediately(); 
    }

    /* DEBUG CODE ######################################## */
    if (now > st::last_debug_time + DEBUG_INT)
    {
      st::last_debug_time = now;
      Serial.print("Color Read: ");
      Serial.println(st::current_square_color == WHITE ? "WHITE" : "PURPLE");
      Serial.print("Analog Value: ");
      Serial.println(analogRead(COLORSENSE_PIN));
      Serial.print("Color Count: ");
      Serial.print(st::colorchanges);
      Serial.print("Bump Count: ");
      Serial.println(st::bumps);
      Serial.print("x, y: ");
      Serial.print(st::x); 
      Serial.print(", "); 
      Serial.println(st::y);
      Serial.print("Game End Time: ");
      Serial.println(GAME_END_TIME);
      Serial.print("Game Time: ");
      Serial.println(game_time());
      if (current_state == s_run)
        Serial.print("STATE RUN");
      else if (current_state == s_gameEnd)
        Serial.print("STATE GAME END");
        
    }
  }
}


/************************* Main Code ****************************/

using namespace DriveSystem;
using namespace FSM;

void setup()
{
  ServoSensorSetup();
  digitalWrite(LED_PIN,LOW);
  Serial.begin(9600);
  FSM::next_state = FSM::s_init;
  FSM::next();
}

void loop() // Main FSM Loop
{
  FSM::FSM_step();
  
  digitalWrite(LED_PIN,HIGH);
  //DropServo.attach(13);
  //DropServo.write(180);
}





