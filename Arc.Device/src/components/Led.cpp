#include "Led.h"
#include "rtos\rtx\TARGET_CORTEX_M\cmsis_os.h"
#include "../utils/LogManager.h"

using namespace arc::device::components;
using namespace arc::device::utils;

arc::device::components::Led::Led(PinName pin)
	: led(pin), th(osPriorityNormal, 1024),
	queue(9*EVENTS_EVENT_SIZE),
	pulseEv(&queue, callback(this, &Led::do_pulse)),
	blinkOnEv(&queue, callback(this, &Led::do_blinkOn)),
	blinkOffEv(&queue, callback(this, &Led::do_blinkOff))
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Led - ctor()");

	pulseId = 0;
	blinkOnId = 0;
	blinkOffId = 0;

	th.start(callback(&queue, &EventQueue::dispatch_forever));
}

void arc::device::components::Led::solid()
{
	cancelTasks();
	led = 1;
}

void arc::device::components::Led::pulse(uint8_t pulseCount)
{
	cancelTasks();
	led = 0;
	pulseEv.period((10 + (int)pulseCount) * (int)pulseDuration);
	pulseId = pulseEv.post(pulseCount);
}

void arc::device::components::Led::blink(bool fast)
{
	cancelTasks();
	led = 0;
	blinkDuration = fast ? 100 : 600;
	blinkOnId = blinkOnEv.post();
}

void arc::device::components::Led::off()
{
	cancelTasks();
	led = 0;
}

void arc::device::components::Led::do_pulse(uint8_t pulseCount)
{
	for (uint8_t i = 0; i < pulseCount; i++)
	{
		led = 1;
		wait_ms(pulseDuration);
		led = 0;
		wait_ms(pulseDuration);
	}
}

void arc::device::components::Led::do_blinkOn()
{
	led = 1;
	blinkOffEv.delay(blinkDuration);
	blinkOffId = blinkOffEv.post();
}

void arc::device::components::Led::do_blinkOff()
{
	led = 0;
	blinkOnEv.delay(blinkDuration);
	blinkOnId = blinkOnEv.post();
}

void arc::device::components::Led::cancelTasks()
{
	pulseEv.cancel();
	pulseId = 0;

	blinkOnEv.cancel();
	blinkOnId = 0;

	blinkOffEv.cancel();
	blinkOffId = 0;
}
