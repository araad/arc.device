#include "Led.h"
#include "rtos.h"
#include "utils/Logger.h"

using namespace arc::device::components;
using namespace arc::device::utils;

Led::Led(PinName pin)
	: led(pin), th(osPriorityNormal, 1024),
	queue(9*EVENTS_EVENT_SIZE),
	pulseEv(&queue, callback(this, &Led::do_pulse)),
	blinkOnEv(&queue, callback(this, &Led::do_blinkOn)),
	blinkOffEv(&queue, callback(this, &Led::do_blinkOff))
{
	Logger::Log(LogLevel::TRACE, "Led - ctor()");

	pulseId = 0;
	blinkOnId = 0;
	blinkOffId = 0;

	th.start(callback(&queue, &EventQueue::dispatch_forever));
}

void Led::solid()
{
	cancelTasks();
	led = 1;
}

void Led::pulse(uint8_t pulseCount)
{
	cancelTasks();
	led = 0;
	pulseEv.period((10 + (int)pulseCount) * (int)pulseDuration);
	pulseId = pulseEv.post(pulseCount);
}

void Led::blink(bool fast)
{
	cancelTasks();
	led = 0;
	blinkDuration = fast ? 100 : 600;
	blinkOnId = blinkOnEv.post();
}

void Led::off()
{
	cancelTasks();
	led = 0;
}

void Led::do_pulse(uint8_t pulseCount)
{
	for (uint8_t i = 0; i < pulseCount; i++)
	{
		led = 1;
		thread_sleep_for(pulseDuration);
		led = 0;
		thread_sleep_for(pulseDuration);
	}
}

void Led::do_blinkOn()
{
	led = 1;
	blinkOffEv.delay(blinkDuration);
	blinkOffId = blinkOffEv.post();
}

void Led::do_blinkOff()
{
	led = 0;
	blinkOnEv.delay(blinkDuration);
	blinkOnId = blinkOnEv.post();
}

void Led::cancelTasks()
{
	pulseEv.cancel();
	pulseId = 0;

	blinkOnEv.cancel();
	blinkOnId = 0;

	blinkOffEv.cancel();
	blinkOffId = 0;
}