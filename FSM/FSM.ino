#include "DriveSystem.h"
#include "SensorsAndServos.h"
#include <Servo.h>
//#include "ActionQueue.h" // Was planning on implementing queues, but that might not work easily in the time we have.

using namespace DriveSystem;

#define GAME_END_TIME (3 * 60 * 1000)
#define GAME_ALMOST_OVER_TIME	(GAME_END_TIME - 30 * 1000)

//Global Vars:
int Our_Color; //(0=White, 1=Purple; I couldn't get an enum working
int Global_Color_Thresh=650;

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
                      Our_Color = digitalRead(TEAMSWITCH_PIN);
                      int val = analogRead(COLORSENSE_PIN);
                      if (Our_Color==0) //White, thus we're on purple
                        {
                          Global_Color_Thresh=val+100;
                        }
                      else
                        {
                          Global_Color_Thresh=val-100;
                        }
                      
	              next_state = s_run;
		      next();
		}

		void s_run()
		{
                    moveForward1Block();
                    delay(10000);
                      
                     next_state=s_run;
                     next();
//			moveForward(sin(state_time()/(0.01)));
//			if (state_time() > 50000)
//			{
//				next_state = s_action1; //Actions.push(s_action1);
//				next();
//			}
		}

		void s_action1()
		{
			moveForward(100);
			if (state_time() > 5000)
			{
				next_state = s_shake_fb; //Actions.push(s_gameEnd);
				next();
			}
		}

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
                    moveBrake();
                    //Check color. If it's not our team color, advance one square and check again
                    int flag=0;
                    while(ReadColor(Global_Color_Thresh)!=Our_Color)
                    {
                      moveForward(255);
                      delay(200);
                      moveBrake();
                      flag=1;
                    }
                    if(flag){
                    moveForward(255);
                    delay(500);
                    moveBrake();
                    }
 
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
                  int color=ReadColor(Global_Color_Thresh);
                  digitalWrite(LED_PIN,!color);
         
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
        ServoSensorSetup();
  	FSM::next_state = FSM::s_init;
	FSM::next();

}

void loop() // Main FSM Loop
{
	FSM::FSM_step();
}


