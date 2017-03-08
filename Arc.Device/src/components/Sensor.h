#pragma once

#include "mbed.h"

#define SENSOR_SAMPLE_SIZE 100

namespace arc
{
	namespace device
	{
		namespace components
		{
			class Sensor
			{
			public:
				Sensor(PinName pin, int min, int max, int period, bool extARef = false);
				void Initialize();
				int GetValue();
			private:
				Mutex lock;
				unsigned int _min;
				unsigned int _max;
				unsigned int _period;
				unsigned int _readings[SENSOR_SAMPLE_SIZE];
				unsigned int _index;
				unsigned long _total;
				AnalogIn _pin;
				unsigned long _lastMeasureTime;
			protected:
				void readInput();
			};
		}
	}
}