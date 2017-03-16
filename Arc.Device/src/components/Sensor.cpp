#include "Sensor.h"
#include "math.h"
#include "../utils/LogManager.h"

using namespace arc::device::components;

Sensor::Sensor(PinName pin, int min, int max, int period, bool extARef)
	: _pin(pin)
{
	_min = min;
	_max = max;
	this->period = period;
}

void Sensor::Initialize()
{
	_index = 0;
	_total = 0;

	// Initialize array of readings with every element set to zero
	for (int i = 0; i < SENSOR_SAMPLE_SIZE; i++)
	{
		_readings[i] = 0;
	}
}

int constrain(int in, int min, int max)
{
	in = in > max ? max : in;
	in = in < min ? min : in;
	return in;
}

int mapRange(int value, int from1, int to1, int from2, int to2)
{
	return (int)((float)(value - from1) / (float)((to1 - from1) * (to2 - from2) + from2));
}

void Sensor::sampleInput()
{
	// subtract the last reading:
	lock.lock();
	if (_total >= _readings[_index])
	{
		_total -= _readings[_index];
	}
	lock.unlock();

	// read from the sensor:
	_readings[_index] = _pin.read_u16();

	// add the reading to the total and	advance to the next position in the array:
	lock.lock();
	_total += _readings[_index++];
	lock.unlock();

	// if we're at the end of the array...
	if (_index >= SENSOR_SAMPLE_SIZE)
	{
		// ...wrap around to the beginning:
		_index = 0;
	}

}

int Sensor::GetValue() {
	lock.lock();
	//Logger.Trace("Sensor - GetValue() _total pointer: 0x%08x", &_total);
	int ret = _total / SENSOR_SAMPLE_SIZE;
	/*_total = _readings[_index];
	int ret = _total;*/
	lock.unlock();
	return ret;
}