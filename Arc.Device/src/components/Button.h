#pragma once
// Button.h


#include "mbed.h"
#include "events\Event.h"
#include "PinDetect.h"

#define TAP_DELAY 30
#define TAP_INTERVAL 0.5f
#define HOLD_DELAY 1.0f
#define HOLD_TICK 0.5f

namespace arc
{
	namespace device
	{
		namespace components
		{
			class Button
			{
			public:
				Event<void(int)> *Tap;
				Event<void(void)> *SingleHold;
				Event<void(void)> *ContinuousHold;

				Button(PinName pin);
				~Button();

				void Initialize();

				void addTapHandler(Callback<void(int)> cb);   //change (void) to int maybe
				void addSingleHoldHandler(Callback<void(void)> cb);
				void addContinuousHoldHandler(Callback<void(void)> cb);

				int tapCount;
				int tickCount;
			private:
				Ticker ticker;
				Timeout timeout;
				PinDetect _pin;

				void onInterruptAssert_ISR(); //transfers callback to the main thread
				void onInterruptAssert();
				void onInterruptDeassert_ISR();
				void onInterruptDeassert();
				void onInterruptAssertAndHold_ISR();
				void onInterruptAssertAndHold();
				void onInterruptDeassertAndHold_ISR();
				void onInterruptDeassertAndHold();
				void countTicks_ISR();
				void countTaps_ISR();
				void postContinuousHoldEvent();
				void postTapEvent();
			};
		}
	}

}