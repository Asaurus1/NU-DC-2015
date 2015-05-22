#ifndef FSM_H
#define FSM_H

// Define some colors, just for the hell of it
#define WHITE 0
#define PURPLE 1
#define BLACK 2

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

	// Debug
	int last_debug_time = 0;
};

// Functions for bump sensing
inline void startBumpCount() { st::previous_bumps = st::bumps; }
inline int  bumpCount()	{ return st::bumps - st::previous_bumps; }

// Functions for timing
inline void startTimerDelay() { st::timer_prev = millis(); }
inline unsigned long timerDelay() { return millis() - st::timer_prev; }

// Functions for color counting
void startColorCount() { st::previous_colorchange = st::colorchanges; }
int  colorCount() { return st::colorchanges - st::previous_colorchange; }

namespace FSM
{
	void s_init();
	void s_run();
	void s_action1();
	void s_shake_fb();
	void s_gameEnd();
  void s_done();
	void FSM_step();
	void next();
	void check_events();
	bool waitTime(int ms);
	bool waitBumps(int count, int timeout = 0);
	bool waitColor(int color, int timeout = 0);
	bool waitColorCount(int count);

	// State Timers
	  unsigned long previous_millis;
	  inline long state_time() { return millis() - previous_millis; };
	  inline long game_time() { return millis(); };

		// State Transition Variables
		void (*previous_state)() = 0;
		void (*current_state)() = 0;
		void (*next_state)() = 0;
		bool state_init = 0;

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
}

#endif