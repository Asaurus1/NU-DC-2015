#ifndef BUMPSENSOR_H
#define BUMPSENSOR_H

class BumpSensor
{
private:
	bool last_state;
	bool raw_state;
	bool unstable_state;
	bool current_state;
	byte trigger_state;
	int  pin;
	long debounceCounter;
	bool debouncing;
	static const byte PRESSED = 1;
	static const byte RELEASED = 2;
	static const byte NONE	= 0;

public:
  BumpSensor(int _pin) : pin(_pin), last_state(0), current_state(0), debounceCounter(11), debouncing(false) {};
	bool on() { return current_state; }
	bool pressed() { return current_state == true && last_state == false; }
	bool released() { return current_state == false && last_state == true; }
	void poll() 
	{ 
		raw_state = !digitalRead(pin);
		last_state = current_state;

		if (debounceCounter > 10)
		{
			//Handle last debouncing
			if (debouncing)
			{
				last_state = current_state;
				current_state = unstable_state;
				debouncing = false;
			}
			else if (raw_state != current_state)
			{
				// Start Debouncing
				unstable_state = raw_state;
				debounceCounter = 0;
				debouncing = true;
			}
		}
		else {
			if (debouncing)
			{
				if (raw_state == unstable_state)
					  debounceCounter++;
				else debounceCounter--;

				if (debounceCounter < -5)
				{
					//failed
					debounceCounter = 11;
					debouncing = false;
				}
			}
		}

		/*Serial.println("BUMP");
			Serial.print("RAW "); Serial.println(raw_state);
			Serial.print("Debounce Cnt "); Serial.println(debounceCounter);
			Serial.print("cur/last "); Serial.print( current_state); Serial.print("/"), Serial.println(last_state);*/

	}
};

#endif


