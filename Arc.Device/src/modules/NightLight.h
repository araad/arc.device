#pragma once

#include "mbed.h"
#include "../core/ModuleBase.h"
#include "../net/ResourceService.h"
#include "../components/RgbLed.h"
#include "../components/Button.h"
#include "../components/LightSensor.h"
#include "rtos/rtx/TARGET_CORTEX_M/cmsis_os.h"

namespace arc
{
	namespace device
	{
		using namespace core;
		using namespace net;
		using namespace components;

		namespace modules
		{
			class NightLight : public ModuleBase
			{
			public:
				NightLight(PinName rPin, PinName gPin, PinName bPin, PinName sensPin, PinName btnPin, PinMode btnMode);
				~NightLight();

			private:
				enum PowerMode { Manual, Auto };
				enum LedState { Off, On };

				RgbLed led;
				Button powerBtn;
				LightSensor sensor;

				LedState ledState;
				PowerMode powerMode;
				uint8_t singleColor[3];

				void init();
				virtual void registerResources();

				// Event Handlers
				void sensor_onAmbienceChange();
				void powerBtn_onTap(uint8_t tpcnt);
				void powerBtn_onSingleHold();

				void setLedState(LedState state);
				void setPowerMode(PowerMode mode);

				Event<void(bool)> ledStateUpdatedEv;
				void onLedStateUpdated(bool state);

				Event<void(int)> powerModeUpdatedEv;
				void onPowerModeUpdated(int mode);

				Event<void(const char*)> colorUpdatedEv;
				void onColorUpdated(const char* color);

				void do_setLedState(LedState state);
				void do_setPowerMode(PowerMode mode);
			};
		}
	}
}