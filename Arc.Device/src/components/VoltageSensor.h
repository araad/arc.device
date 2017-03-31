#pragma once

#include "Sensor.h"

namespace arc
{
	namespace device
	{
		namespace components
		{
			class VoltageSensor : public Sensor
			{
			public:
				VoltageSensor(PinName pin);
				~VoltageSensor();

				float GetVoltage();
			private:
				float voltage;

				virtual void readInput();
			};
		}
	}
}