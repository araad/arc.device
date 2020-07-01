#include "Button.h"
#include "core/system/TaskManager.h"
#include "utils/Logger.h"

using namespace arc::device::components;
using namespace arc::device::utils;

#define TAP_INTERVAL 0.5f
#define HOLD_TICK 0.3f

Button::Button(PinName pin, PinMode mode)
	: _pin(pin, mode)
{
	Logger::Log(LogLevel::TRACE, "Button - ctor()");
	_pin.setAssertValue(0);
	Tap = 0;
	SingleHold = 0;
	ContinuousHold = 0;
}

Button::~Button()
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

	_pin.attach_asserted(callback(this, &Button::onInterruptAssert_ISR));
	_pin.attach_asserted_held(callback(this, &Button::onInterruptAssertAndHold_ISR));
	_pin.attach_deasserted_held(callback(this, &Button::onInterruptDeassertAndHold_ISR));
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

void Button::countTicks_ISR()
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

void Button::countTaps_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &Button::postTapEvent));
}

void Button::postTapEvent()
{
	if (Tap)
	{
		Tap->post(tapCount);
	}
	tapCount = 0;
	timeout.detach();
}

void Button::addTapHandler(Callback<void(uint8_t)> cb)
{
	Tap = new Event<void(uint8_t)>(Tasks.GetQueue(), cb);
}

void Button::addSingleHoldHandler(Callback<void(void)> cb)
{
	SingleHold = new Event<void(void)>(Tasks.GetQueue(), cb);
}

void Button::addContinuousHoldHandler(Callback<void(void)> cb)
{
	ContinuousHold = new Event<void(void)>(Tasks.GetQueue(), cb);
}