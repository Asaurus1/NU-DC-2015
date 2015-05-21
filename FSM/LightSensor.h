#ifndef BUMPSENSOR_H
#define BUMPSENSOR_H

class BumpSensor
{
private:
	bool last_state;
	bool current_state;
	int  pin;

	BumpSensor(int _pin) : pin(_pin), last_state(0), current_state(0);

public:
	bool on() { return current_state; }
	bool pressed() { return (current_state == true && last_state == false); }
	bool released() { return (current_state == false && last_state == true); }
	void poll() 
	{ 
		last_state = current_state; 
		current_state = digitalRead(pin);
	}
};

#endif


