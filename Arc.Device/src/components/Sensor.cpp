#include "Sensor.h"
#include "math.h"

using namespace arc::device::components;

Sensor::Sensor(PinName pin, int min, int max, int period, bool extARef)
	: _pin(pin)
{
	_min = min;
	_max = max;
	_period = period;
}

void Sensor::Initialize()
{
	_index = 0;
	_total = 0;
	_lastMeasureTime = 0;

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

void Sensor::readInput()
{
	// subtract the last reading:
	_total -= _readings[_index];

	// read from the sensor:
	_readings[_index] = constrain(_pin.read_u16(), _min, _max);

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
	int ret = _total / SENSOR_SAMPLE_SIZE;
	lock.unlock();
	return ret;
}