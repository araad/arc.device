#pragma once

#include "mbed.h"
#include "rtos\Thread.h"
#include "TaskManager.h"
#include "PanelInterfaceLoader.h"
#include "..\net\ConnectionManager.h"
#include "..\net\ResourceService.h"
#include <string>
#include "Task.h"
#include "../components/Button.h"
#include "../components/Led.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace rtos;
using namespace arc::device::components;

namespace arc
{
	namespace device
	{
		namespace core
		{
			class Device
			{
			public:
				Device();
			private:
				PanelInterfaceLoader piLoader;
				DigitalOut aux;
				Button btn;
				Led statusLed;

				net::ResourceService service;
				int time;
				Task<Callback<void()>> initTask;

				void initialize();
				void onSetCurrentTime(int value);
				void onPanelInterfaceLoaded(int piId);
				void onSetAux(bool value);
				void onBtnTap(int tapCount);
				void onBtnHold();
				void onSysHangSim(bool value);
			};
		}
	}
}