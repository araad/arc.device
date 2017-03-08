#include "PanelInterfaceLoader.h"
#include "../utils/LogManager.h"
#include "TaskManager.h"
#include "cmsis/core_cm4.h"
#include "../interfaces/ElectricalOutletPanel.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::core;

extern "C" void __NVIC_SystemReset();

DigitalOut output1(D15);
DigitalOut output2(D12);
DigitalOut output3(D14);

arc::device::core::PanelInterfaceLoader::PanelInterfaceLoader(Callback<void(int)> onLoadedCallback)
	: pin(MBED_CONF_APP_PANEL_INTERFACE_BUS_0),
	pin0(MBED_CONF_APP_PANEL_INTERFACE_BUS_1),
	pin1(MBED_CONF_APP_PANEL_INTERFACE_BUS_2),
	pin2(MBED_CONF_APP_PANEL_INTERFACE_BUS_3),
	loadEvent(Tasks->GetQueue(), onLoadedCallback)
{
	Logger.Trace("PanelInterfaceLoader - ctor");

	output1 = 1;
	output2 = 1;
	output3 = 1;

	panelInterface = NULL;

	if (pin == 1)
	{
		piId = readPins();
		Logger.Info("pi is connected, piId: %d", piId);
		load();
	}
	else
	{
		piId = 0;
		Logger.Info("pi is disconnected");
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
	Tasks->AddOneTimeTask(callback(this, &PanelInterfaceLoader::onPanelInterfaceAttach));
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceDetach_ISR()
{
	Tasks->AddOneTimeTask(callback(this, &PanelInterfaceLoader::onPanelInterfaceDetach));
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceAttach()
{
	Logger.Trace("PanelInterfaceLoader - onPanelInterfaceAttach");
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
		loadEvent.post(piId);
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

	loadEvent.post(piId);
}

IPanelInterface * arc::device::core::PanelInterfaceLoader::getInterfaceInstance()
{
	return new arc::device::interfaces::ElectricalOutletPanel();
}

int arc::device::core::PanelInterfaceLoader::readPins()
{
	int id0 = pin0.read();
	int id1 = pin1.read();
	int id2 = pin2.read();

	int id = id0 | id1 << 1 | id2 << 2;

	return id;
}
