#pragma once

#include "mbed.h"
#include "Sensor.h"

namespace arc
{
	namespace device
	{
		namespace components
		{
			class LightSensor : public Sensor
			{
			public:
				typedef enum { Dark, Bright } Ambience;

				LightSensor(PinName pin, int samplePeriod, int readPeriod);
				~LightSensor();

				Ambience GetAmbience();
			private:
				Ambience ambience;

				virtual void readInput();
			};
		}
	}
}