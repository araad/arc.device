#include "PanelInterfaceLoader.h"
#include "../utils/LogManager.h"
#include "TaskManager.h"
#include "cmsis/core_cm4.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::core;

extern "C" void __NVIC_SystemReset();

DigitalOut outpin(D15);
DigitalOut led(LED2);

arc::device::core::PanelInterfaceLoader::PanelInterfaceLoader(Callback<void(bool)> onLoadedCallback)
	: pin(MBED_CONF_APP_PANEL_INTERFACE_DETECT)
{
	Logger.Trace("PanelInterfaceLoader - ctor");

	panelInterface = NULL;
	loaded = false;
	outpin = 1;
	loadEvent = new Event<void(bool)>(Tasks->GetQueue(), onLoadedCallback);

	if (pin == 1)
	{
		Logger.Info("pin is ON");
		led = 1;
		load();
	}
	else
	{
		Logger.Info("pin is OFF");
		led = 0;
	}

	pin.attach_asserted(this, &PanelInterfaceLoader::onPanelInterfaceAttach_ISR);
	pin.attach_deasserted(this, &PanelInterfaceLoader::onPanelInterfaceDetach_ISR);
	pin.setSampleFrequency();
}

bool arc::device::core::PanelInterfaceLoader::isLoaded()
{
	return loaded;
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceAttach_ISR()
{
	led = 1;
	Tasks->AddOneTimeTask(callback(this, &PanelInterfaceLoader::onPanelInterfaceAttach));
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceDetach_ISR()
{
	led = 0;
	Tasks->AddOneTimeTask(callback(this, &PanelInterfaceLoader::onPanelInterfaceDetach));
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceAttach()
{
	Logger.Trace("PanelInterfaceLoader - onPanelInterfaceAttach\r\n");

	load();
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceDetach()
{
	Logger.Trace("PanelInterfaceLoader - onPanelInterfaceDetach");

	unload();
}

void arc::device::core::PanelInterfaceLoader::load()
{
	// TODO: load the proper interface instance

	//NVIC_SystemReset();
	//panelInterface->Start();

	loaded = true;
	loadEvent->post(loaded);
}

void arc::device::core::PanelInterfaceLoader::unload()
{
	// TODO: unload the interface instance

	//panelInterface->Stop();
	if (panelInterface)
	{
		delete panelInterface;
		panelInterface = NULL;
	}

	loaded = false;
	loadEvent->post(loaded);
}
