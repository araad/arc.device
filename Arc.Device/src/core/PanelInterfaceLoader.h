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
				PanelInterfaceLoader(Callback<void(bool)> onLoadedCallback);
				bool isLoaded();
			private:
				PinDetect pin;
				bool loaded;
				Event<void(bool)>* loadEvent;

				IPanelInterface *panelInterface;

				void onPanelInterfaceAttach_ISR();
				void onPanelInterfaceDetach_ISR();
				void onPanelInterfaceAttach();
				void onPanelInterfaceDetach();

				void load();
				void unload();
			};
		}
	}
}