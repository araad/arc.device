#include "Button.h"
#include "..\core\TaskManager.h"
#include "..\utils\LogManager.h"
#include "PinDetect.h"

using namespace arc::device::components;
using namespace arc::device::utils;

Button::Button(PinName pin, PinMode mode)
	: _pin(pin, mode)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Button - ctor()");
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

	_pin.attach_asserted(this, &Button::onInterruptAssert_ISR);
	_pin.attach_asserted_held(this, &Button::onInterruptAssertAndHold_ISR);
	_pin.attach_deasserted_held(this, &Button::onInterruptDeassertAndHold_ISR);
	_pin.setSamplesTillHeld(24); // a hold is recognized after 480ms (24 * 20ms)
	_pin.setSampleFrequency();

}

void Button::onInterruptAssert_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &Button::onInterruptAssert));
}

void Button::onInterruptAssert()
{
	if (tapCount > 0)
	{
		timeout.detach();
	}

	tapCount++;
	timeout.attach(callback(this, &Button::countTaps_ISR), TAP_INTERVAL);
}

void Button::onInterruptAssertAndHold_ISR()
{
	if (tapCount > 0) // cancel the tap event if holding
	{
		tapCount = 0;
		timeout.detach();
	}
	Tasks.AddOneTimeTask(callback(this, &Button::onInterruptAssertAndHold));
}

void Button::onInterruptAssertAndHold()
{
	ticker.attach(callback(this, &Button::countTicks_ISR), HOLD_TICK);
}

void Button::onInterruptDeassertAndHold_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &Button::onInterruptDeassertAndHold));
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
		if (ContinuousHold)
		{
			ContinuousHold->post();
		}
	}
}

void arc::device::components::Button::countTaps_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &Button::postTapEvent));
}

void arc::device::components::Button::postTapEvent()
{
	if (Tap)
	{
		Tap->post(tapCount);
	}
	tapCount = 0;
	timeout.detach();
}

void arc::device::components::Button::addTapHandler(Callback<void(uint8_t)> cb)
{
	Tap = new Event<void(uint8_t)>(Tasks.GetQueue(), cb);  //calls cb on the Tasks queue (main thread), returns a void accepts an int.
}

void arc::device::components::Button::addSingleHoldHandler(Callback<void(void)> cb)
{
	SingleHold = new Event<void(void)>(Tasks.GetQueue(), cb);
}

void arc::device::components::Button::addContinuousHoldHandler(Callback<void(void)> cb)
{
	ContinuousHold = new Event<void(void)>(Tasks.GetQueue(), cb);
}