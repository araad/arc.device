#pragma once
// Button.h


#include "mbed.h"
#include "PinDetect.h"

namespace arc
{
	namespace device
	{
		namespace components
		{
			class Button
			{
			public:
				Event<void(uint8_t)> *Tap;
				Event<void(void)> *SingleHold;
				Event<void(void)> *ContinuousHold;

				Button(PinName pin, PinMode mode = PullDown);
				~Button();

				void Initialize();

				void addTapHandler(Callback<void(uint8_t)> cb);
				void addSingleHoldHandler(Callback<void(void)> cb);
				void addContinuousHoldHandler(Callback<void(void)> cb);

				uint8_t tapCount;
				uint8_t tickCount;
			private:
				mbed::Ticker ticker;
				Timeout timeout;
				PinDetect _pin;

				void onInterruptAssert_ISR(); //transfers callback to the main thread
				void onInterruptAssert();
				void onInterruptAssertAndHold_ISR();
				void onInterruptAssertAndHold();
				void onInterruptDeassertAndHold_ISR();
				void onInterruptDeassertAndHold();
				void countTicks_ISR();
				void countTaps_ISR();
				void postTapEvent();
			};
		}
	}

}