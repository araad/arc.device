#include "VoltageSensor.h"
#include "../core/TaskManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::components::VoltageSensor::VoltageSensor(PinName pin)
	: Sensor(pin, 1, 0, true, 16)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "VoltageSensor - ctor()");
	voltage = 0.0f;
}

arc::device::components::VoltageSensor::~VoltageSensor()
{
}

float arc::device::components::VoltageSensor::GetVoltage()
{
	readInput();
	return voltage;
}

void arc::device::components::VoltageSensor::readInput()
{
	int value = getValue();

	float newVoltage = (0.023f * (float)value) - 0;

	if (newVoltage > 80.0f)
	{
		newVoltage = 120.0f;
	}
	else
	{
		newVoltage = 0.0f;
	}

	if (voltage != newVoltage)
	{
		voltage = newVoltage;
		if (valueChangeEvent)
		{
			valueChangeEvent->post();
		}
	}
}
