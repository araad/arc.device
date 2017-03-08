#pragma once

#include "mbed.h"

namespace arc
{
	namespace device
	{
		namespace core
		{
			class SystemController
			{
			public:
				SystemController();
				~SystemController();
			private:
				Thread th;
				EventQueue queue;

				void initWatchdog();
				bool getAndClearWatchdogReset();
				void kickWatchdog();
			};
		}
	}
}