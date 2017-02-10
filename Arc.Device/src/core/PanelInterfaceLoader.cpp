#include "PanelInterfaceLoader.h"
#include "../utils/LogManager.h"
#include "TaskManager.h"
#include "cmsis/core_cm4.h"
#include "../interfaces/TestPanel.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::core;

extern "C" void __NVIC_SystemReset();

DigitalOut outpin(D15);
DigitalOut outpin2(D12);
DigitalOut led(LED2);

arc::device::core::PanelInterfaceLoader::PanelInterfaceLoader(Callback<void(int)> onLoadedCallback)
	: pin(MBED_CONF_APP_PANEL_INTERFACE_BUS_0),
	/*bus(MBED_CONF_APP_PANEL_INTERFACE_BUS_1,
		MBED_CONF_APP_PANEL_INTERFACE_BUS_2,
		MBED_CONF_APP_PANEL_INTERFACE_BUS_3)*/
	pin0(MBED_CONF_APP_PANEL_INTERFACE_BUS_1),
	pin1(MBED_CONF_APP_PANEL_INTERFACE_BUS_2),
	pin2(MBED_CONF_APP_PANEL_INTERFACE_BUS_3)
{
	Logger.Trace("PanelInterfaceLoader - ctor");

	panelInterface = NULL;
	outpin = 1;
	outpin2 = 1;
	loadEvent = new Event<void(int)>(Tasks->GetQueue(), onLoadedCallback);

	if (pin == 1)
	{
		//piId = bus.read();
		piId = readPins();
		Logger.Info("pi is connected, piId: %d", piId);
		led = 1;
		load();
	}
	else
	{
		piId = 0;
		Logger.Info("pi is disconnected");
		led = 0;
	}

	pin.attach_asserted(this, &PanelInterfaceLoader::onPanelInterfaceAttach_ISR);
	pin.attach_deasserted(this, &PanelInterfaceLoader::onPanelInterfaceDetach_ISR);
	pin.setSampleFrequency();
}

int arc::device::core::PanelInterfaceLoader::getPanelInterfaceId()
{
	return piId;
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
	Logger.Trace("PanelInterfaceLoader - onPanelInterfaceAttach");
	//piId = bus.read();
	piId = readPins();
	load();
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceDetach()
{
	Logger.Trace("PanelInterfaceLoader - onPanelInterfaceDetach");
	piId = 0;
	unload();
}

void arc::device::core::PanelInterfaceLoader::load()
{
	// TODO: load the proper interface instance

	//NVIC_SystemReset();
	panelInterface = getInterfaceInstance();
	if (panelInterface)
	{
		panelInterface->Start();
		loadEvent->post(piId);
	}
	else
	{

	}
}

void arc::device::core::PanelInterfaceLoader::unload()
{
	// TODO: unload the interface instance

	if (panelInterface)
	{
		panelInterface->Stop();
		delete panelInterface;
		panelInterface = NULL;
	}
	else
	{

	}

	loadEvent->post(piId);
}

IPanelInterface * arc::device::core::PanelInterfaceLoader::getInterfaceInstance()
{
	return new arc::device::interfaces::TestPanel();
}

int arc::device::core::PanelInterfaceLoader::readPins()
{
	int id0 = pin0.read();
	int id1 = pin1.read();
	int id2 = pin2.read();

	int id = id0 | id1 << 1 | id2 << 2;

	return id;
}
