#pragma once

#include "mbed.h"
#include "PinDetect.h"
#include "IPanelInterface.h"
#include "DebounceIn.h"

namespace arc
{
	namespace device
	{
		namespace core
		{
			class PanelInterfaceLoader
			{
			public:
				PanelInterfaceLoader(Callback<void(int)> onLoadedCallback);
				int getPanelInterfaceId();
			private:
				PinDetect pin;
				DigitalIn pin0;
				DigitalIn pin1;
				DigitalIn pin2;
				int piId;
				Event<void(int)> loadEvent;

				IPanelInterface *panelInterface;

				void onPanelInterfaceAttach_ISR();
				void onPanelInterfaceDetach_ISR();
				void onPanelInterfaceAttach();
				void onPanelInterfaceDetach();

				void load();
				void unload();
				IPanelInterface* getInterfaceInstance();
				int readPins();
			};
		}
	}
}