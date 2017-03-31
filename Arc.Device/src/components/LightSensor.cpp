#include "LightSensor.h"
#include "../utils/LogManager.h"

using namespace arc::device::components;
using namespace arc::device::utils;

arc::device::components::LightSensor::LightSensor(PinName pin, int samplePeriod, int readPeriod)
	: Sensor(pin, samplePeriod, readPeriod)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "LightSensor - ctor()");

	ambience = Dark;
}

arc::device::components::LightSensor::~LightSensor()
{
}

arc::device::components::LightSensor::Ambience arc::device::components::LightSensor::GetAmbience()
{
	return ambience;
}

void arc::device::components::LightSensor::readInput()
{
	int value = getValue();

	if (ambience == Dark && value < 45000)
	{
		ambience = Bright;
		if (valueChangeEvent)
		{
			valueChangeEvent->post();
		}
	}
	else if (ambience == Bright && value >= 45000)
	{
		ambience = Dark;
		if (valueChangeEvent)
		{
			valueChangeEvent->post();
		}
	}
}
