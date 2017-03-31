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
				CurrentSensor(PinName pin);
				~CurrentSensor();

				float GetCurrent();
			private:
				float current;

				virtual void readInput();
			};
		}
	}
}