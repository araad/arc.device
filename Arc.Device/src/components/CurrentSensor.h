#pragma once
#include "Sensor.h"
#include "mbed.h"


namespace arc
{
	namespace device
	{
		namespace components
		{
			class CurrentSensor : protected Sensor
			{
			public:

				CurrentSensor(PinName pin, int min, int max, int period, bool extARef = false);
				~CurrentSensor();

				void Initialize();
				void addCurrentChangeHandler(Callback<void()> cb);
				float GetCurrent();

			private:
				float current;

				Event<void(void)> *CurrentChange;

				Thread readInputThread;

				void readInputThreadStarter();
				void readInput_Task();

				void readCurrent();
			};
		}
	}
}