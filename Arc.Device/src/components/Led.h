#pragma once

#include "mbed.h"

namespace arc
{
	namespace device
	{
		namespace components
		{
			class Led
			{
			public:
				Led(PinName pin);

				void solid();
				void pulse(uint8_t pulseCount = 1);
				void blink(bool fast = false);
				void off();
			private:
				DigitalOut led;
				Mutex queueLock;
				EventQueue queue;
				Thread th;

				static const uint8_t pulseDuration = 150;
				Event<void(uint8_t)> pulseEv;
				int pulseId;

				int blinkDuration;
				Event<void()> blinkOnEv;
				Event<void()> blinkOffEv;
				int blinkOnId;
				int blinkOffId;

				void do_pulse(uint8_t pulseCount);
				void do_blinkOn();
				void do_blinkOff();
				void cancelTasks();
			};
		}
	}
}