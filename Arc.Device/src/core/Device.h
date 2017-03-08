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
				~Device();
			private:
				PanelInterfaceLoader *piLoader;
				DigitalOut aux;
				Button btn;

				net::ResourceService service;
				int time;
				EventQueue queue;
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