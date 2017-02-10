#pragma once

#include "mbed.h"
#include "rtos\Thread.h"
#include "TaskManager.h"
#include "PanelInterfaceLoader.h"
#include "..\net\ConnectionManager.h"
#include "..\net\ResourceService.h"
#include <string>

using namespace rtos;

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
				PanelInterfaceLoader *piLoader;
				DigitalOut aux0;
				DigitalOut aux1;

				net::ResourceService service;
				int time;
				EventQueue queue;

				void onSetCurrentTime(int value);
				void onPanelInterfaceLoaded(int piId);
				void onSetAux0(bool value);
				void onSetAux1(bool value);
			};
		}
	}
}