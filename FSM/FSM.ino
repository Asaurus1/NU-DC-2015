#include "DriveSystem.h"
#include "SensorsAndServos.h"
#include <Servo.h>
#include "BumpSensor.h"
#include "FSM.h"

using namespace DriveSystem;

#define TURN_TIME_90 2500
#define MOVE_TIME_1B 2500
#define COLOR_READ_INT 100
#define DEBUG_INT			 1000
int GAME_END_TIME = (3 * 60 * 1000);
int GAME_ALMOST_OVER_TIME = (GAME_END_TIME - 30 * 1000);

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
      int start_time = millis();
      GAME_END_TIME +=start_time;
      GAME_ALMOST_OVER_TIME +=start_time;
      
      // drop the scoop
      scoopDown();
      
      digitalWrite(LED_PIN, LOW);
		  next_state = s_run;
		  next();
		}

		void s_InitialSequence()
		{
			Serial.print("IS: ");
		}

		void s_run()
		{
		    moveForward(st::spd);
		    waitColorCount(1);
		    digitalWrite(LED_PIN,LOW);
		    waitColorCount(1);
		    digitalWrite(LED_PIN,HIGH);
		    waitColorCount(1);
		    digitalWrite(LED_PIN,LOW);
		    waitColorCount(1);
		    digitalWrite(LED_PIN,HIGH);
		    moveBrake();
		    ScoopDump();
		    digitalWrite(LED_PIN,LOW);
		    waitTime(5000);
		    next_state = s_Done;
		    next();
		}

    void s_gameEnd()
    {
	    //Check color. If it's not our team color, advance one square and check again
	    moveForward(255);
	    int flag = 0;
	    if (st::current_square_color = st::team_color)
	    {
	    	flag = 1;
	    }

	    if(flag)
	    {
	      moveForward(255);
	      waitTime(500);
	      moveBrake();

	      //drop the back

	      moveTurnLeft(255);
	      waitTime(250);
	      moveTurnRight(255);
	      waitTime(250);
	      moveBrake();
	      waitTime(250);
	      DropServo.attach(13);
	      DropServo.write(180);
	      waitTime(200);
	      DropServo.detach();
	      moveForward(100);
	      waitTime(2000);
	      moveBrake();
	      
	      next_state=s_Done;
	      next();
	   	}
		}
    void s_Done() // also functions as a color-sensing loop, if you just need to check colors
    {
      moveBrake();
    }

		/********************* FSM WaitFunctions *********************/
		void waitTime(int ms)
		{
			 unsigned long start = millis();
			 while (millis() < start + ms)
			 {
			 		check_events();
			 }
		}

		void waitColor(int color)
		{
			 while (st::current_square_color != color)
			 {
			 		check_events();
			 }
		}

		void waitColorCount(int count)
		{
			 startColorCount();
			 while (colorCount() < count)
			 {
			 		check_events();
			 }
		}

		void waitBumps(int count)
		{
			 startBumpCount();
			 while (bumpCount() < count)
			 {
			 		check_events();
			 }
		}

		/*********************** FSM Events ********************/

		void check_events()
		{
			unsigned long now = millis();
      //Color Polling
      if (now > st::last_color_read_time + COLOR_READ_INT)
     	{
     		st::last_color_read_time = now;
				st::previous_square_color = st::current_square_color;
	      st::current_square_color = ReadColor(st::color_threshold);
	      delay(5);

      	if (st::current_square_color != st::previous_square_color) // Double check
      		st::colorchanges++;

      	digitalWrite(LED_PIN,!st::current_square_color); // Write color for debug purposes
    	}

    	/* Bump count */
    	tableBump.poll();
    	if (tableBump.pressed())
    	{
    		st::bumps++;
    	}

		  /* Game Timers */
		  if (game_time() > GAME_END_TIME)
		  {
       next_state = s_Done; // do nothing
       //next();
		  }
		  else if (game_time() > GAME_ALMOST_OVER_TIME)
		  {
		  	digitalWrite(LED_PIN, HIGH);
		  	next_state = s_gameEnd; //Search for a place to drop
		  	//next();  
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
				Serial.print(st::x); Serial.print(", "); Serial.println(st::y);
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
  delay(5000);
  FSM::next_state = FSM::s_init;
  FSM::next();
}

void loop() // Main FSM Loop
{
	FSM::FSM_step();
}




