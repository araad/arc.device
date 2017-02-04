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
				net::ResourceService service;
				int time;
				EventQueue queue;

				void onTimeUpdated(int value);
				void onPanelInterfaceLoaded(bool value);
			};
		}
	}
}