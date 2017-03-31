#pragma once

#include "mbed.h"
#include "PanelInterfaceLoader.h"
#include "Task.h"
#include "../net/ResourceService.h"
#include "../components/Button.h"
#include "../components/Led.h"
#include "ModuleBase.h"
#include "IAuxController.h"

namespace arc
{
	namespace device
	{
		using namespace components;
		using namespace net;

		namespace core
		{
			class Device : public ModuleBase
			{
			public:
				Device();

				virtual void Initialize();
			private:
				PanelInterfaceLoader piLoader;
				DigitalOut aux;
				Button btn;
				Led statusLed;
				IAuxController* auxCtrl;

				int time;
				Task<Callback<void()>> initTask;

				virtual void registerResources();

				Event<void(int)> setCurrentTimeEv;
				void onSetCurrentTime(int value);

				Event<void(bool)> auxUpdatedEv;
				void onAuxUpdated(bool value);
				void setAux(bool state);

				void onPanelInterfaceChanged(uint8_t piId);
				void onBtnTap(uint8_t tapCount);
				void onBtnHold();
			};
		}
	}
}