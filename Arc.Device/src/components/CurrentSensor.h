#pragma once
#include "Sensor.h"
#include "mbed.h"


namespace arc
{
	namespace device
	{
		namespace components
		{
			class CurrentSensor : public Sensor
			{
			public:

				CurrentSensor(PinName pin, int min, int max, int samplePeriod, int readPeriod, bool extARef = false);
				~CurrentSensor();

				void Initialize();
				void addCurrentChangeHandler(Callback<void()> cb);
				float GetCurrent();

			private:
				float current;
				int readPeriod;

				Event<void(void)> *CurrentChange;

				EventQueue queue;
				Thread readInputThread;

				void sampleInput_Task();

				void readCurrent();
			};
		}
	}
}