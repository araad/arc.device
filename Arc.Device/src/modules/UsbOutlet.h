#pragma once

#include "mbed.h"
#include "../core/ModuleBase.h"

namespace arc
{
	namespace device
	{
		using namespace core;
		using namespace net;

		namespace modules
		{
			class UsbOutlet : public ModuleBase
			{
			public:
				UsbOutlet(PinName enablePin);
				~UsbOutlet();

				virtual void Initialize();
			private:
				DigitalOut enable;

				virtual void registerResources();

				Event<void(bool)> enableUpdatedEv;
				void onEnableUpdated(bool value);
			};
		}
	}
}