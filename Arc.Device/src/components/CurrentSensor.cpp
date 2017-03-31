#include "CurrentSensor.h"
#include "../utils/LogManager.h"

using namespace arc::device::components;
using namespace arc::device::utils;

CurrentSensor::CurrentSensor(PinName pin)
	: Sensor(pin, 1, 3000, true, 16)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "CurrentSensor - ctor()");
	current = 0.0f;
}

arc::device::components::CurrentSensor::~CurrentSensor()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "CurrentSensor - dtor()");
}

void CurrentSensor::readInput()
{
	int value = getValue();

	float newCurrent = ((0.362f * (float)value) - 15052.584f) / 1000.0f;
	newCurrent = fmaxf(0.0f, newCurrent);

	if ((current > 0.0f && newCurrent == 0.0f) || fabsf(current - newCurrent) >= 0.1f)
	{
		current = newCurrent;
		if (valueChangeEvent)
		{
			valueChangeEvent->post();
		}
	}
}

float CurrentSensor::GetCurrent()
{
	return current;
}