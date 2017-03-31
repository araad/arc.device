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

				void initWatchdog();
				bool getAndClearWatchdogReset();
				void kickWatchdogThreadStarter();
				void kickWatchdog();
			};
		}
	}
}