#pragma once

#include "mbed.h"
#include "PinDetect.h"
#include "IPanelInterface.h"

namespace arc
{
	namespace device
	{
		namespace core
		{
			class PanelInterfaceLoader
			{
			public:
				PanelInterfaceLoader(Callback<void(uint8_t)> onLoadedCallback);
				uint8_t getPanelInterfaceId();
				IPanelInterface* getPanelInterface();
			private:
				PinDetect pin;
				DigitalIn pin0;
				DigitalIn pin1;
				DigitalIn pin2;
				uint8_t piId;
				Event<void(uint8_t)> loadEvent;

				IPanelInterface *panelInterface;

				void onPanelInterfaceAttach_ISR();
				void onPanelInterfaceDetach_ISR();
				void onPanelInterfaceAttach();
				void onPanelInterfaceDetach();

				void load();
				void unload();
				IPanelInterface* createInterfaceInstance();
				uint8_t readPins();
			};
		}
	}
}