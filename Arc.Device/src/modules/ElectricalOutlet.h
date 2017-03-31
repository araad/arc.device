#pragma once

#include "mbed.h"
#include "../core/ModuleBase.h"
#include "../components/CurrentSensor.h"

namespace arc
{
	namespace device
	{
		using namespace core;
		using namespace net;
		using namespace components;

		namespace modules
		{
			class ElectricalOutlet : public ModuleBase
			{
			public:
				ElectricalOutlet(PinName pwrStatePin, PinName sensorPin, uint8_t instance_id);
				~ElectricalOutlet();

				virtual void Initialize();
				virtual void registerResources();
			private:
				DigitalOut pwrState;
				CurrentSensor sensor;
				uint8_t instance_id;

				void init();

				Event<void(bool)> powerStateUpdatedEv;
				void onPowerStateUpdated(bool value);

				void onCurrentChange();
			};

			class ElectricalOutletCollection
			{
			public:
				ElectricalOutletCollection(int outletCount, ...)
				{
					va_list args;
					va_start(args, outletCount);
					for (int i = 0; i < outletCount; i++)
					{
						outlets.push_back(new ElectricalOutlet((PinName)va_arg(args, int), (PinName)va_arg(args, int), i));
					}
					va_end(args);
				}

				~ElectricalOutletCollection()
				{
					vector<ElectricalOutlet*>::iterator outlet;
					for (outlet = outlets.begin(); outlet < outlets.end(); outlet++)
					{
						delete (*outlet);
					}
				}

				void Initialize()
				{
					vector<ElectricalOutlet*>::iterator outlet;
					for (outlet = outlets.begin(); outlet < outlets.end(); outlet++)
					{
						(*outlet)->Initialize();
					}
				}
			private:
				vector<ElectricalOutlet*> outlets;
			};
		}
	}
}