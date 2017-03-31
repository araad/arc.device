#pragma once

#include "mbed.h"
#include <atomic>

#define SENSOR_SAMPLE_COUNT 100
#define SENSOR_MAX_COUNT 10

namespace arc
{
	namespace device
	{
		namespace components
		{
			class Sensor
			{
			public:
				Sensor(PinName pin, int samplePeriod, int readPeriod, bool readMax = false, uint8_t maxAvgSamples = 0);
				virtual ~Sensor();

				void Initialize();
				void addValueChangeHandler(Callback<void()> cb);

			private:
				AnalogIn _pin;
				unsigned int _readings[SENSOR_SAMPLE_COUNT];
				unsigned int _currentReading;
				uint8_t _index;

				unsigned int _maxReadings[SENSOR_MAX_COUNT];
				uint8_t _maxIndex;
				bool readMax;
				uint8_t maxAvgSamples;

				Mutex _totalMutex;
				unsigned long _total;
			protected:
				unsigned int samplePeriod;
				int readPeriod;
				int readInputTaskId;

				Event<void()> *valueChangeEvent;
				//EventQueue queue;
				//Thread queueThread;

				Thread sampleInputThread;
				void sampleInput_Task();
				void sampleInput();

				virtual void readInput() = 0;
				int getValue();
			};
		}
	}
}