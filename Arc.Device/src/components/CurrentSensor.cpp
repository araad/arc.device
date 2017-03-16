#include "CurrentSensor.h"
#include "../core/TaskManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::components;

CurrentSensor::CurrentSensor(PinName pin, int min, int max, int samplePeriod, int readPeriod, bool extARef)
	: Sensor(pin, min, max, samplePeriod, extARef),
	readInputThread(osPriorityNormal, 1024)
{
	CurrentChange = 0;
	current = 0;
	this->readPeriod = readPeriod;
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

	//queue.call_every(period, callback(this, &CurrentSensor::sampleInput_Task));
	//readInputThread.start(callback(&queue, &EventQueue::dispatch_forever));
	readInputThread.start(callback(this, &CurrentSensor::sampleInput_Task));

	Tasks.AddRecurringTask("CurrentSensor", callback(this, &CurrentSensor::readCurrent), readPeriod);
}

void arc::device::components::CurrentSensor::addCurrentChangeHandler(Callback<void()> cb)
{
	CurrentChange = new Event<void()>(Tasks.GetQueue(), cb);
}

//collects samples from the sensor 
void CurrentSensor::sampleInput_Task() {
	/*Logger.mapThreadName("SensorB");
	Logger.Trace("starting readinput task");*/
	//int readinputcount = 0;
	while (1) {
		/*if (period >= 1000 || readinputcount++ > (1000/period))
		{
			readinputcount = 0;
			Logger.Trace("reading...");
		}*/
		this->sampleInput();
		//wait_us(100);
		wait_ms(period);
	}
}

//checks those samples. Reads the data collected by readInput
void CurrentSensor::readCurrent()
{
	int value = GetValue();
	Logger.Trace("Current: %d", value);
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