#pragma once

#include "mbed.h"
#include "IPanelInterface.h"

namespace arc
{
	namespace device
	{
		namespace core
		{
			class IAuxController : public IPanelInterface
			{
			public:
				virtual ~IAuxController() {}

				virtual void setUpdateAuxEv(Event<void(bool)>* ev) = 0;
				virtual void onAuxUpdated(bool state) = 0;
			};
		}
	}
}