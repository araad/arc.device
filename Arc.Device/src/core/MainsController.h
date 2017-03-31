#pragma once

#include "mbed.h"
#include "../components/VoltageSensor.h"
#include "../components/CurrentSensor.h"
#include <vector>

using namespace arc::device::components;
using namespace std;

namespace arc
{
	namespace device
	{
		namespace core
		{
			class MainsController
			{
			public:
				MainsController();
				void enableMainPower();
				void disableMainPower();
				void startCurrentWatch(CurrentSensor* sensor, uint8_t id);
				void stopCurrentWatch(CurrentSensor* sensor, uint8_t id);
				float getVoltage();
			private:
				DigitalOut mainSwitch;
				VoltageSensor sensor;
				bool powerEnabled;
				int watchTaskId;
				CurrentSensor* sensor1;
				CurrentSensor* sensor2;

				void checkCurrent();
				void checkSensor(CurrentSensor* sensor);
			};
		}
	}
}

extern arc::device::core::MainsController MainsCtrl;