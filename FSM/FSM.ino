#include "DriveSystem.h"
//#include "ActionQueue.h" // Was planning on implementing queues, but that might not work easily in the time we have.

using namespace DriveSystem;

#define GAME_END_TIME (3 * 60 * 1000)
#define GAME_ALMOST_OVER_TIME	(GAME_END_TIME - 30 * 1000)

namespace FSM {
		void s_init();
		void s_run();
		void s_action1();
		void s_shake_fb();
		void s_gameEnd();
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
			// Do some stuff here
			next_state = s_run;
			next();
		}

		void s_run()
		{
			moveForward(sin(state_time()/(0.01)));
			if (state_time() > 50000)
			{
				next_state = s_action1; //Actions.push(s_action1);
				next();
			}
		}

		void s_action1()
		{
			moveTurnLeft(255);
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
			while (true) ;
		}

		/********************* FSM Events *********************/
		void check_events()
		{
			/* Game Timers */
			if (game_time() > GAME_END_TIME)
			{
				next_state = s_gameEnd; //Stop moving and end the game.
				next();
			}
			else if (game_time() > GAME_ALMOST_OVER_TIME)
			{
				//Game is almost over. We should find a spot to dump any payload we have.
			}
		}
}


/************************* Main Code ****************************/

using namespace DriveSystem;
using namespace FSM;

void setup()
{
	FSM::next_state = FSM::s_init;
	FSM::next();
}

void loop() // Main FSM Loop
{
	FSM::FSM_step();
}


