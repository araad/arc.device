#pragma once

#include "mbed.h"
#include "../core/IPanelInterface.h"
#include "PinDetect.h"
#include "./../net/ResourceService.h"

using namespace arc::device::core;

namespace arc
{
	namespace device
	{
		namespace interfaces
		{
			class TestPanel : public IPanelInterface
			{
			public:
				TestPanel();
				~TestPanel();

				virtual void Start();
				virtual void Stop();
			private:
				DigitalOut lightSwitch;
				PinDetect btn;
				net::ResourceService service;

				int readBtnTaskId;
				void onBtnPress_ISR();
				void onBtnPress();
				void onLightSwitchUpdated(bool value);
				Callback<void(bool)> cb;
			};
		}
	}
}