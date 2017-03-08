#include "CurrentSensor.h"
#include "../core/TaskManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::components;

CurrentSensor::CurrentSensor(PinName pin, int min, int max, int period, bool extARef)
	: Sensor(pin, min, max, period, extARef),
	readInputThread(osPriorityNormal, 512)
{
	CurrentChange = 0;
	current = 0;
}

arc::device::components::CurrentSensor::~CurrentSensor()
{
	if (CurrentChange)
	{
		delete CurrentChange;
	}
}

void CurrentSensor::Initialize()
{
	Sensor::Initialize();

	readInputThread.start(callback(this, &CurrentSensor::readInputThreadStarter));

	Tasks->AddRecurringTask("CurrentSensor", callback(this, &CurrentSensor::readCurrent), 5000);
}

void arc::device::components::CurrentSensor::addCurrentChangeHandler(Callback<void()> cb)
{
	CurrentChange = new Event<void()>(Tasks->GetQueue(), cb);
}

void CurrentSensor::readInputThreadStarter() {
	this->readInput_Task();
	while (1) {}
}

//collects samples from the sensor 
void CurrentSensor::readInput_Task() {
	while (1) {
		this->readInput();
		wait_ms(1);
	}
}

//checks those samples. Reads the data collected by readInput
void CurrentSensor::readCurrent()
{
	int value = GetValue();
	if (current != value)
	{
		current = value;
		if (CurrentChange)
		{
			CurrentChange->post();
		}
	}
}

float CurrentSensor::GetCurrent()
{
	// do the formula here
	/*int temp = GetValue();
	current = temp / sqrt(2);*/

	return current;
}