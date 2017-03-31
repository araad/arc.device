#include "Sensor.h"
#include "math.h"
#include "../core/TaskManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::components;
using namespace arc::device::utils;

Sensor::Sensor(PinName pin, int samplePeriod, int readPeriod, bool readMax, uint8_t maxAvgSamples)
	: _pin(pin),
	//queue(20480),
	sampleInputThread(osPriorityBelowNormal, 1024)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Sensor - ctor()");

	this->samplePeriod = samplePeriod;
	this->readPeriod = readPeriod;
	this->readMax = readMax;
	this->maxAvgSamples = maxAvgSamples;

	readInputTaskId = 0;
}

arc::device::components::Sensor::~Sensor()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Sensor - dtor()");

	if (valueChangeEvent)
	{
		delete valueChangeEvent;
	}

	sampleInputThread.terminate();

	if (readInputTaskId != 0)
	{
		Tasks.CancelTask(readInputTaskId);
	}
}

void Sensor::Initialize()
{
	_index = 0;
	_total = 0;
	_currentReading = 0;

	// Initialize array of readings with every element set to zero
	for (int i = 0; i < SENSOR_SAMPLE_COUNT; i++)
	{
		_readings[i] = 0;
	}

	for (int i = 0; i < SENSOR_MAX_COUNT; i++)
	{
		_maxReadings[i] = 0;
	}

	//queueThread.start(callback(&queue, &EventQueue::dispatch_forever));
	sampleInputThread.start(callback(this, &Sensor::sampleInput_Task));

	if (readPeriod >= 100)
	{
		readInputTaskId = Tasks.AddRecurringTask("Sensor", callback(this, &Sensor::readInput), readPeriod);
	}
}

void arc::device::components::Sensor::addValueChangeHandler(Callback<void()> cb)
{
	valueChangeEvent = new Event<void()>(Tasks.GetQueue(), cb);
}

void Sensor::sampleInput()
{
	if (!readMax)
	{
		_totalMutex.lock();
		if (_total >= _readings[_index])
		{
			// subtract the last reading:
			_total -= _readings[_index];
		}
		_totalMutex.unlock();
	}

	// read from the sensor:
	_currentReading = _pin.read_u16();
	//queue.call(printf, "%d,\r\n", _currentReading);
	_readings[_index++] = _currentReading;

	if (!readMax)
	{
		_totalMutex.lock();
		_total += _currentReading;
		_totalMutex.unlock();
	}

	// if we're at the end of the array...
	if (_index >= SENSOR_SAMPLE_COUNT)
	{
		// ...wrap around to the beginning:
		_index = 0;
	}
}

//collects samples from the sensor 
void Sensor::sampleInput_Task() {
	//int readinputcount = 0;
	int samples = 0;
	while (1) {
		/*if (samplePeriod >= 1000 || readinputcount++ > (1000 / (samplePeriod || 1)))
		{
			readinputcount = 0;
			Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "reading...");
		}*/
		this->sampleInput();

		if (readMax && ++samples >= maxAvgSamples)
		{
			samples = 0;
			_maxReadings[_maxIndex++] = *max_element(_readings, _readings + SENSOR_SAMPLE_COUNT);
			if (_maxIndex >= SENSOR_MAX_COUNT)
			{
				// ...wrap around to the beginning:
				_maxIndex = 0;
			}
		}

		if (samplePeriod > 0)
		{
			wait_ms(samplePeriod);
		}
		else
		{
			wait_us(500);
		}
	}
}

int Sensor::getValue() {
	int ret;

	if (readMax)
	{
		unsigned long total = 0;
		for (int i = 0; i < SENSOR_MAX_COUNT; i++)
		{
			total += _maxReadings[i];
		}
		ret = total / SENSOR_MAX_COUNT;
	}
	else
	{
		_totalMutex.lock();
		ret = _total / SENSOR_SAMPLE_COUNT;
		_totalMutex.unlock();
	}

	return ret;
}