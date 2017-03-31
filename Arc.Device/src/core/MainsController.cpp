#include "MainsController.h"
#include "../utils/LogManager.h"
#include "../mbed_config_include.h"
#include "TaskManager.h"

using namespace arc::device::utils;

#define MAINSCTRL_MAX_CURRENT 14.0f

arc::device::core::MainsController::MainsController()
	: mainSwitch(MBED_CONF_APP_MAINS_CONTROLLER_POWER_SWITCH),
	sensor(MBED_CONF_APP_MAINS_CONTROLLER_VOLTAGE_SENSOR)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "MainsController - ctor()");
	mainSwitch = 0;
	sensor1 = NULL;
	sensor2 = NULL;
	sensor.Initialize();
}

void arc::device::core::MainsController::enableMainPower()
{
	powerEnabled = true;
}

void arc::device::core::MainsController::disableMainPower()
{
	powerEnabled = false;
	mainSwitch = 0;
}

void arc::device::core::MainsController::startCurrentWatch(CurrentSensor * sensor, uint8_t id)
{
	if (powerEnabled)
	{
		if (id == 0)
		{
			sensor1 = sensor;
		}
		else if (id == 1)
		{
			sensor2 = sensor;
		}

		if (watchTaskId == 0)
		{
			watchTaskId = Tasks.AddRecurringTask("CurrentWatch", callback(this, &MainsController::checkCurrent), 1000);
		}

		mainSwitch = 1;
	}
}

void arc::device::core::MainsController::stopCurrentWatch(CurrentSensor * sensor, uint8_t id)
{
	if (id == 0)
	{
		sensor1 = NULL;
	}
	else if (id == 1)
	{
		sensor2 = NULL;
	}

	if (!sensor1 && !sensor2)
	{
		Tasks.CancelTask(watchTaskId);
		watchTaskId = 0;
	}
}

float arc::device::core::MainsController::getVoltage()
{
	return sensor.GetVoltage();
}

void arc::device::core::MainsController::checkCurrent()
{
	checkSensor(sensor1);
	checkSensor(sensor2);
}

void arc::device::core::MainsController::checkSensor(CurrentSensor * sensor)
{
	if (sensor)
	{
		if (sensor->GetCurrent() > MAINSCTRL_MAX_CURRENT)
		{
			disableMainPower();
		}
	}
}
