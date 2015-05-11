#ifndef ActionQueue_h
#define ActionQueue_h

#include <stdint.h>

template <class type, int Tsize>
class ActionQueue {
public:
	static const int SIZE = Tsize;

	/*
	 * Constructor. Initializes the deque.
	 */
	ActionQueue() : head(0), tail(0), isFull(false), isEmpty(true) {}

	/*
	 * Returns true if the deque is full.
	 */
	bool is_full() {
		return isFull && head == tail;
	}

	/*
	 * Returns true if the deque is empty.
	 */
	bool is_empty() {
		return (!isFull) && head == tail;
	}

	/*
	 * Returns the size of the deque
	 */
	int size() {
		if (is_full())
			return SIZE;
		else
			return (head - tail + SIZE) % SIZE;
	}

	/* Get the top timer value
	 */
	inline long get_timer_head()
	{
		return timerBuf[head];
	}

	/*
	 * Adds a frame at the head of the deque.
	 */
	void enqueue_head(const type f) {
		if (!is_full()) {
			buf[head++] = f;

			// Wrap Head
			if (head >= SIZE) {
				head = 0;
			}

			//Check whether full
			if (head == tail) {
				isFull = true;
			}
		}
		else { // If full, we overwrite the last added message.
			buf[(head + SIZE - 1) % SIZE] = f;
		}
	}


	/*
	 * Returns a frame from the head of the deque
	 */
	type pull() {
		if (!is_empty()) {
			//Wrap head
			if (head == 0) {
				head = SIZE;
			}

			uint8_t readloc = --head;

			//Check whether queue has been emptied
			if (tail == head) {
				isFull = false;
			}

			return buf[readloc];
		}
		return type();
	}

	/*
	 * Adds a frame at the tail of the deque.
	 */
	void push(const type f) {
		if (!is_full()) {
			if (tail == 0) {
				tail = SIZE;
			}

			buf[--tail] = f;

			//Check whether full
			if (head == tail) {
				isFull = true;
			}
		}
		else { // If full, we overwrite the last added message.
			buf[tail] = f;
		}
	}

	/*
	 * Returns a frame from the tail of the deque
	 */
	type dequeue_tail() {
		if (!is_empty()) {
			uint8_t readloc = tail++;

			//Wrap tail
			if (tail >= SIZE) {
				tail = 0;
			}

			//Check whether queue has been emptied
			if (tail == head) {
				isFull = false;
			}

			return buf[readloc];
		}
		return type();
	}

private:
	type buf[SIZE];
	long timerBuf[SIZE];
	uint8_t head;
	uint8_t tail;
	bool	isFull;
};

#endif


