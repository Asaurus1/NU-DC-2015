#include "DriveSystem.h"
#include "SensorsAndServos.h"
#include <Servo.h>
//#include "ActionQueue.h" // Was planning on implementing queues, but that might not work easily in the time we have.
#include "BumpSensor.h"

using namespace DriveSystem;

#define GAME_END_TIME (3 * 60 * 1000)
#define GAME_ALMOST_OVER_TIME	(GAME_END_TIME - 30 * 1000)
#define TURN_TIME_90 2500
#define MOVE_TIME_1B 2500

//Now create a structure to hold all data about the game state space
namespace st
{
	int  color_threshold = 650;
	byte team_color; 						// Holds the color of the team that I am on when I start
	byte current_square_color; 	// Holds the color of the current square
	byte previous_square_color; // Holds the color of the previous square
	unsigned long last_color_read_time = 0;

	bool box_full = false; 							// Holds whether the box is full
	bool box_dropped =false;						// Holds whether the box has already been dropped.

	//coordinates (if they work)
	byte x = 0;
	byte y = 0;
	byte dir = 0; // 0 = x, 1 = y;
        byte spd = 255;

	//bump counters
	int bumps = 0;
	int previous_bumps = 0;

	//color counters 
	int colorchanges = 0;
	int previous_colorchange = 0;

	// Timer counter
	unsigned long timer_prev = 0;	
};

// Some functions to make mirroring easy
inline void moveTurnLeftAsPurple(byte spd) { 
	if (st::team_color == PURPLE) 
		moveTurnLeft(spd);
	else
		moveTurnRight(spd);
}
inline void moveTurnRightAsPurple(byte spd) { 
	if (st::team_color == PURPLE) 
		moveTurnRight(spd);
	else
		moveTurnLeft(spd);
}

// Functions for bump sensing
inline void startBumpCount() { st::previous_bumps = st::bumps; }
inline int  bumpCount()	{ return st::bumps-st::previous_bumps; }

// Functions for timing
inline void startTimerDelay() { st::timer_prev = millis(); }
inline unsigned long timerDelay() { return millis() - st::timer_prev; }

// Functions for color counting
void startColorCount() { st::previous_colorchange = st::colorchanges; }
int  colorCount() { return st::colorchanges - st::previous_colorchange; }

namespace FSM {
		void s_init();
		void s_run();
		void s_action1();
		void s_shake_fb();
		void s_gameEnd();
    void s_Done();
		void FSM_step();
		void next();
		void check_events();

		//Action and interrupt queues
		//ActionQueue<(void *), 20> Actions;
		//ActionQueue<(void *), 10> Interrupts;

		// State Timers
	  unsigned long previous_millis;
	  inline long state_time() { return millis() - previous_millis; };
	  inline long game_time() { return millis(); };

		// State Transition Variables
		void (*previous_state)() = 0;
		void (*current_state)() = 0;
		void (*next_state)() = 0;
		bool state_init = 0;

		//Create a task array to hold a sequence of actions
		int current_step = 0;

		void next()
		{
			previous_state = current_state;
			current_state = next_state;
			previous_millis = millis();
		}

		inline void FSM_step()
		{
			(*current_state)();
			check_events();
		}

		/********************* FSM Actions *********************/

	  //Special Init Function
		void s_init()
	  {
      st::team_color = digitalRead(TEAMSWITCH_PIN);
      int val = analogRead(COLORSENSE_PIN);
      if (st::team_color == WHITE) //White, thus we're on purple
        {
          st::color_threshold=val+100;
        }
      else
        {
          st::color_threshold=val-100;
        }
		  next_state = s_run;
		  next();
		}

		void s_InitialSequence()
		{
			Serial.print("IS: "); Serial.println(current_step);
			switch(current_step)
			{
				case 0: {// Move forward
					scoopDown();
					moveForward(255);
					startColorCount();
					current_step++;
					break; }
				case 1: {// Stop after two squares
					if (1)//(colorCount() >= 2 && st::current_square_color != st::team_color) // %% color sensor reads ~team_color
					{
						st::y = 2;
						st::x = 0;
						scoopUp();
						moveTurnRightAsPurple(st::spd);
						startTimerDelay();
						current_step++;
					}
					break; }
				case 2: {// Stop turning left and put the scoop down.
					if (timerDelay() >= TURN_TIME_90)
					{
						moveBrake();
						scoopDown();
						delay(100);
						moveForward(st::spd);
						startColorCount();
						current_step++;
					} break; }
				case 3: {
					if (true) //(colorCount() >= 2) && (st::current_square_color != st::team_color)) // %% color sensor reads ~team_color
					{
						st::y = 2;
						st::x = 2;
						scoopUp();
						moveTurnRightAsPurple(st::spd);
						startTimerDelay();
						current_step++;
					} break; }
				case 4: {
					if (timerDelay() >= TURN_TIME_90)
					{
						moveBrake();
						scoopDown();
						delay(100);
						moveForward(st::spd);
						startColorCount();
						current_step++;
					} break; }
				case 5: {
					if (colorCount() >= 2 && st::current_square_color != st::team_color) // %% color sensor reads ~team_color
					{
						st::y = 0;
						st::x = 2;
						scoopUp();
						moveTurnLeftAsPurple(st::spd);
						startTimerDelay();
						current_step++;
					} break; }
				case 6: {
					if (timerDelay() >= TURN_TIME_90)
					{
						moveBrake();
						scoopDown();
						delay(100);
						moveForward(st::spd);
						startColorCount();
						current_step++;
					} break; }
				case 7: {
					if (colorCount() >= 2 && st::current_square_color != st::team_color) // %% color sensor reads ~team_color
					{
						st::y = 0;
						st::x = 4;
						scoopUp();
						delay(500);
						scoopDown();
						next_state = s_gameEnd;
						next();
					} break; }
			}
		}

		void s_run()
		{
	    moveForward1Block();
	      
//			moveForward(sin(state_time()/(0.01)));
//			if (state_time() > 50000)
//			{
//				next_state = s_action1; //Actions.push(s_action1);
//				next();
//			}
		}

		/*void s_action1()
		{
			moveForward(100);
			if (state_time() > 5000)
			{
				next_state = s_shake_fb; //Actions.push(s_gameEnd);
				next();
			}
		}*/

		void s_shake_fb()
		{
			if (state_time() % 10000 > 5000 )
			{
				moveForward(7);
			}
			else
			{
				moveReverse(7);
			}

			if (state_time() > 100)
			{
				next_state = s_action1;
				next();
			}

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

      if(flag){
        moveForward(255);
        delay(500);
        moveBrake();

        //drop the back

        moveTurnLeft(255);
        delay(250);
        moveTurnRight(255);
        delay(250);
        moveBrake();
        delay(250);
        DropServo.attach(13);
        DropServo.write(180);
        delay(200);
        DropServo.detach();
        moveForward(100);
        delay(2000);
        moveBrake();
        
        next_state=s_Done;
        next();
     	}
		}
    void s_Done() // also functions as a color-sensing loop, if you just need to check colors
    {
      moveBrake();
      next_state=s_Done;
      next();
      
      
    }

		/********************* FSM Events *********************/
		void check_events()
		{
      //polling to show color:
      if (millis() > st::last_color_read_time + 100)
     	{
				st::previous_square_color = st::current_square_color;
	      st::current_square_color = ReadColor(st::color_threshold);
	      delay(5);
	      digitalWrite(LED_PIN,!st::current_square_color);

      	if (st::current_square_color != st::previous_square_color && ReadColor(st::color_threshold) == st::current_square_color) // Double check
      		st::colorchanges++;
    	}

         
		  /* Game Timers */
		  if (game_time() > GAME_END_TIME)
		  {
       next_state = s_Done; // do nothing
       next();
		  }
		  else if (game_time() > GAME_ALMOST_OVER_TIME)
		  {
		  	next_state = s_gameEnd; //Search for a place to drop
		  	next();  
      }
		}
}


/************************* Main Code ****************************/

using namespace DriveSystem;
using namespace FSM;

void setup()
{
  Serial.begin(9600);
  ServoSensorSetup();
  FSM::next_state = FSM::s_init;
  FSM::next();

}
int running = 0;

void loop() // Main FSM Loop
{
	FSM::FSM_step();
	Serial.print("COLOR READ: ");
	Serial.println((ReadColor(st::color_threshold) == WHITE) ? "WHITE" : "PURPLE");
	Serial.print("Analog Value: ");
	Serial.println(analogRead(COLORSENSE_PIN));
}




