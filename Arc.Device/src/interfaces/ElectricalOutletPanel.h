#pragma once

#include "mbed.h"
#include "../core/IPanelInterface.h"
#include "../modules/ElectricalOutlet.h"
#include "../modules/UsbOutlet.h"
#include "../modules/NightLight.h"
#include "../net/ConnectionManager.h"

namespace arc
{
	namespace device
	{
		using namespace core;
		using namespace modules;

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
				ElectricalOutletCollection outlets;
				NightLight nightLight;
				DigitalOut usbEnable;
			};
		}
	}
}