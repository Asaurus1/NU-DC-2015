#ifndef BUMPSENSOR_H
#define BUMPSENSOR_H

class BumpSensor
{
private:
	bool last_state;
	bool unstable_state;
	bool current_state;
	byte trigger_state;
	int  pin;
	unsigned long debounceTimer;
	static const byte PRESSED = 1;
	static const byte RELEASED = 2;
	static const byte NONE	= 0;

public:
  BumpSensor(int _pin) : pin(_pin), last_state(0), current_state(0), debounceTimer(0) {};
	bool on() { return current_state; }
	bool pressed() { return trigger_state == PRESSED; }
	bool released() { return trigger_state == RELEASED; }
	void poll() 
	{ 
		if (millis() > debounceTimer + 500)
		{
			last_state = current_state; 
			current_state = !digitalRead(pin);

			if (last_state != current_state)
			{
				if (last_state == 0)
					trigger_state = PRESSED;
				else
					trigger_state = RELEASED;

				debounceTimer = millis();
			}
			else trigger_state = NONE;
		}
	}
};

#endif


