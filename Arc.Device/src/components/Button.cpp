#include "Button.h"
#include "..\core\TaskManager.h"
#include "..\utils\LogManager.h"
#include "PinDetect.h"

using namespace arc::device::components;

Button::Button(PinName pin)
	: _pin(pin)
{
	_pin.setAssertValue(0);
	Tap = 0;
	SingleHold = 0;
	ContinuousHold = 0;
}

arc::device::components::Button::~Button()
{
	if (Tap)
	{
		delete Tap;
	}

	if (SingleHold)
	{
		delete SingleHold;
	}

	if (ContinuousHold)
	{
		delete ContinuousHold;
	}
}

void Button::Initialize()
{
	tapCount = 0;
	tickCount = 0;
	_pin.mode(PullDown);

	Logger.Trace("pin value: %d", _pin);

	_pin.attach_asserted(this, &Button::onInterruptAssert_ISR);
	_pin.attach_deasserted(this, &Button::onInterruptDeassert_ISR);
	_pin.attach_asserted_held(this, &Button::onInterruptAssertAndHold_ISR);
	_pin.attach_deasserted_held(this, &Button::onInterruptDeassertAndHold_ISR);
	_pin.setSamplesTillHeld(24); // a hold is recognized after 480ms (24 * 20ms)
	_pin.setSampleFrequency();

}

void Button::onInterruptAssert_ISR()
{
	Tasks->AddOneTimeTask(callback(this, &Button::onInterruptAssert));
}

void Button::onInterruptAssert()
{
	if (tapCount > 0)
	{
		timeout.detach();
	}

	tapCount++;
	//Logger.Trace("number of taps: %d", tapCount);
	timeout.attach(callback(this, &Button::countTaps_ISR), TAP_INTERVAL);
}

void Button::onInterruptDeassert_ISR()
{
	Tasks->AddOneTimeTask(callback(this, &Button::onInterruptDeassert));
}

void Button::onInterruptDeassert()
{
	//Logger.Trace("Button was let go");
}

void Button::onInterruptAssertAndHold_ISR()
{
	if (tapCount > 0) // cancel the tap event if holding
	{
		tapCount = 0;
		timeout.detach();
	}
	Tasks->AddOneTimeTask(callback(this, &Button::onInterruptAssertAndHold));
}

void Button::onInterruptAssertAndHold()
{
	//Logger.Trace("Button is being held");
	ticker.attach(callback(this, &Button::countTicks_ISR), HOLD_TICK);
}

void Button::onInterruptDeassertAndHold_ISR()
{
	Tasks->AddOneTimeTask(callback(this, &Button::onInterruptDeassertAndHold));
}

void Button::onInterruptDeassertAndHold()
{
	ticker.detach();
	tickCount = 0;
}

void arc::device::components::Button::countTicks_ISR()
{
	tickCount++;
	if (tickCount == 1)
	{
		if (SingleHold)
		{
			SingleHold->post();
		}
	}
	else if (tickCount >= 2)
	{
		//Logger.Trace("ContinuousHold post Start");
		if (ContinuousHold)
		{
			ContinuousHold->post();
		}
		//Logger.Trace("ContinuousHold post Finish");
	}
}

void arc::device::components::Button::countTaps_ISR()
{
	Tasks->AddOneTimeTask(callback(this, &Button::postTapEvent));
	//tapCount = 0;
}

void arc::device::components::Button::postTapEvent()
{
	//Logger.Trace("tap post Start");
	Logger.Trace("number of taps: %d", tapCount);
	if (Tap)
	{
		Tap->post(tapCount);
	}
	//Logger.Trace("tap post Finish");
	tapCount = 0;
	timeout.detach();
}

void arc::device::components::Button::addTapHandler(Callback<void(int)> cb)
{
	Tap = new Event<void(int)>(Tasks->GetQueue(), cb);  //calls cb on the Tasks queue (main thread), returns a void accepts an int.
}

void arc::device::components::Button::addSingleHoldHandler(Callback<void(void)> cb)
{
	SingleHold = new Event<void(void)>(Tasks->GetQueue(), cb);
}

void arc::device::components::Button::addContinuousHoldHandler(Callback<void(void)> cb)
{
	ContinuousHold = new Event<void(void)>(Tasks->GetQueue(), cb);
}