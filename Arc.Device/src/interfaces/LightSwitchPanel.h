#pragma once

#include "mbed.h"
#include "../core/IAuxController.h"
#include "../modules/LightSwitch.h"
#include "../net/ConnectionManager.h"

namespace arc
{
	namespace device
	{
		using namespace core;
		using namespace modules;

		namespace interfaces
		{
			class LightSwitchPanel : public IAuxController
			{
			public:
				LightSwitchPanel();
				~LightSwitchPanel();

				virtual void Start();
				virtual void Stop();
			private:
				LightSwitch lightSwitch;

				virtual void setUpdateAuxEv(Event<void(bool)>* ev);
				virtual void onAuxUpdated(bool state);
			};
		}
	}
}