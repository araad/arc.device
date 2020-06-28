#pragma once

#include "mbed.h"
#include "system/Task.h"
#include "ModuleBase.h"

namespace arc
{
	namespace device
	{
		namespace core
		{
            using namespace system;

			class Device : public ModuleBase
			{
			public:
				Device();

				virtual void Initialize();
			private:

				Task<Callback<void()>> initTask;
			};
		}
	}
}