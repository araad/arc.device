#include "RgbLed.h"
#include <vector>
#include "../utils/LogManager.h"

using namespace arc::device::components;
using namespace arc::device::utils;

const uint8_t RgbLed::black[] = { 0, 0, 0 };
const uint8_t RgbLed::white[] = { 100, 100, 100 };
const uint8_t RgbLed::red[] = { 100, 0, 0 };
const uint8_t RgbLed::blue[] = { 0, 0, 100 };

RgbLed::RgbLed(PinName rPin, PinName gPin, PinName bPin)
	: queue(10 * EVENTS_EVENT_SIZE),
	thread(osPriorityNormal, 1280),
	rPin(rPin), gPin(gPin), bPin(bPin)
{
	this->rPin = 1.0f;
	this->gPin = 1.0f;
	this->bPin = 1.0f;

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "RgbLed - ctor()");
}

void RgbLed::Initialize()
{
	solidFadeDuration = 1000;

	// Initialize color variables
	copyColor(black, curVal);
	copyColor(black, prevVal);

	thread.start(callback(&queue, &EventQueue::dispatch_forever));
}

void RgbLed::Blink(uint8_t led)
{
	resetQueue();
	queue.call(this, &RgbLed::blinkTask, led);
}

void RgbLed::ShowSolid(uint8_t color[]) {
	doShowSolid(color, solidFadeDuration);
}

void arc::device::components::RgbLed::SetFadeDuration(int fadeDuration)
{
	solidFadeDuration = fadeDuration;
}

void RgbLed::FadeOut()
{
	doShowSolid((uint8_t*)black, solidFadeDuration / 2);
}

void RgbLed::doShowSolid(uint8_t color[], int fadeDuration)
{
	resetQueue();
	queue.call(this, &RgbLed::showSolidTask, color, fadeDuration);
}

void RgbLed::showSolidTask(uint8_t color[], int fadeDuration)
{
	Logger.mapThreadName("RgbLed");
	crossFade(color, fadeDuration, 0);
}

void arc::device::components::RgbLed::blinkTask(uint8_t led)
{
	Logger.mapThreadName("RgbLed");
	const uint8_t *color = led == 0 ? red : blue;
	uint8_t val[3];
	for (uint8_t j = 0; j < 3; j++) {
		val[j] = color[j] * 255 / 100;
	}

	setColor((uint8_t*)black);
	wait_ms(400);

	for (uint8_t i = 0; i < 2; i++)
	{
		setColor(val);
		wait_ms(250);
		setColor((uint8_t*)black);
		wait_ms(400);
	}
}

void RgbLed::resetQueue()
{
	queue.break_dispatch();
	thread.terminate();

	thread.start(callback(&queue, &EventQueue::dispatch_forever));
}

int RgbLed::calculateStep(int prevValue, int endValue) {
	int step;

	step = (int)((endValue * 255) / 100) - (int)prevValue;
	if (step) {
		step = 1020 / step;
	}

	return step;
}

int RgbLed::calculateVal(int step, int val, int idx) {
	if ((step) && idx % step == 0) { // If step is non-zero and its time to change a value,
		if (step > 0) {              //   increment the value if step is positive...
			val += 1;
		}
		else if (step < 0) {         //   ...or decrement it if step is negative
			val -= 1;
		}
	}
	// Defensive driving: make sure val stays in the range 0-255
	if (val > 255) {
		val = 255;
	}
	else if (val < 0) {
		val = 0;
	}

	return val;
}

void RgbLed::crossFade(uint8_t color[], int fadeDuration, int showDuration) {
	// Convert to 0-255
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "==========================starting crossFade");
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "color[0]: %d color[1]: %d, color[2]: %d", color[0], color[1], color[2]);
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "fadeDuration: %d, showDuration: %d", fadeDuration, showDuration);
	int R = (color[0] * 255) / 100;
	int G = (color[1] * 255) / 100;
	int B = (color[2] * 255) / 100;

	int stepR = calculateStep(prevVal[0], R);
	int stepG = calculateStep(prevVal[1], G);
	int stepB = calculateStep(prevVal[2], B);

	float stepDuration = roundf(((float)fadeDuration / 1020.0f) * 10.0f) / 10.0f;
	int skipDelay = (int)floorf(1020 / (stepDuration * 100)) / 10;

	for (int i = 0; i <= 1020; i++)
	{
		curVal[0] = calculateVal(stepR, curVal[0], i);
		curVal[1] = calculateVal(stepG, curVal[1], i);
		curVal[2] = calculateVal(stepB, curVal[2], i);

		setColor(curVal);

		// Update current values for next loop
		copyColor(curVal, prevVal);

		if (stepDuration >= 1) {
			wait_ms((int)stepDuration);
		}
		else if (stepDuration > 0 && !(i % skipDelay)) {
			wait_ms(1);
		}
	}

	if (showDuration > 0) {
		wait_ms(showDuration);
	}
}

void RgbLed::setColor(uint8_t val[])
{
	for (uint8_t i = 0; i < 3; i++)
	{
		float v = 1.0f - (float)val[i] / 255.0f;
		if (i == 0)
		{
			rPin.write(v);
		}
		else if (i == 1)
		{
			gPin.write(v);
		}
		else if (i == 2)
		{
			bPin.write(v);
		}
	}

}

void RgbLed::copyColor(const uint8_t src[], uint8_t *dest)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		dest[i] = src[i];
	}
}