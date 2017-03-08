#pragma once

#include "mbed.h"
#include "../core/IPanelInterface.h"
#include "../net/ResourceService.h"
#include "../components/CurrentSensor.h"

using namespace arc::device::core;
using namespace arc::device::components;

namespace arc
{
	namespace device
	{
		namespace interfaces
		{
			class ElectricalOutletPanel : public IPanelInterface
			{
			public:
				ElectricalOutletPanel();
				~ElectricalOutletPanel();

				virtual void Start();
				virtual void Stop();
			private:
				net::ResourceService service;
				DigitalOut pwrState;
				CurrentSensor sensor;

				void onPowerStateUpdated(bool value);
				Callback<void(bool)> powerStateUpdatedCallback;

				void onCurrentChange();
			};
		}
	}
}