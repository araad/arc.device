#pragma once

#include "mbed.h"
#include "../core/ModuleBase.h"
#include "../components/Button.h"
#include "../components/RgbLed.h"

namespace arc
{
	namespace device
	{
		using namespace components;
		using namespace core;
		using namespace net;

		namespace modules
		{
			class LightSwitch : public ModuleBase
			{
			public:
				LightSwitch();
				~LightSwitch();

				virtual void Initialize();

				Event<void(bool)>* updateAuxEv;
				void onAuxUpdated(bool state);
			private:
				RgbLed led;
				Button btn;
				uint8_t solidColor[3];
				bool auxState;

				virtual void registerResources();

				void btn_onTap(uint8_t tpcnt);
				void do_switchState(bool newState);
			};
		}
	}
}