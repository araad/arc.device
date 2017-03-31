#include "PanelInterfaceLoader.h"
#include "../utils/LogManager.h"
#include "TaskManager.h"
#include "cmsis/core_cm4.h"
#include "../interfaces/ElectricalOutletPanel.h"
#include "../interfaces/LightSwitchPanel.h"
#include "../interfaces/MotionDetectorPanel.h"

#include "../mbed_config_include.h"

using namespace arc::device::core;
using namespace arc::device::utils;

extern "C" void __NVIC_SystemReset();

arc::device::core::PanelInterfaceLoader::PanelInterfaceLoader(Callback<void(uint8_t)> onLoadedCallback)
	: pin(MBED_CONF_APP_PANEL_INTERFACE_LOADER_PIN_0),
	pin0(MBED_CONF_APP_PANEL_INTERFACE_LOADER_PIN_1),
	pin1(MBED_CONF_APP_PANEL_INTERFACE_LOADER_PIN_2),
	pin2(MBED_CONF_APP_PANEL_INTERFACE_LOADER_PIN_3),
	loadEvent(Tasks.GetQueue(), onLoadedCallback)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "PanelInterfaceLoader - ctor() begin");

	panelInterface = NULL;

	if (pin == 1)
	{
		piId = readPins();
		Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "PanelInterfaceLoader - ctor() pi is connected, piId: %d", piId);
		load();
	}
	else
	{
		piId = 0;
		Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "PanelInterfaceLoader - ctor() pi is disconnected");
	}

	pin.attach_asserted(this, &PanelInterfaceLoader::onPanelInterfaceAttach_ISR);
	pin.attach_deasserted(this, &PanelInterfaceLoader::onPanelInterfaceDetach_ISR);
	pin.setSampleFrequency();

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "PanelInterfaceLoader - ctor() - end");
}

uint8_t arc::device::core::PanelInterfaceLoader::getPanelInterfaceId()
{
	return piId;
}

IPanelInterface * arc::device::core::PanelInterfaceLoader::getPanelInterface()
{
	return panelInterface;
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceAttach_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &PanelInterfaceLoader::onPanelInterfaceAttach));
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceDetach_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &PanelInterfaceLoader::onPanelInterfaceDetach));
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceAttach()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "PanelInterfaceLoader - onPanelInterfaceAttach");
	piId = readPins();
	load();
}

void arc::device::core::PanelInterfaceLoader::onPanelInterfaceDetach()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "PanelInterfaceLoader - onPanelInterfaceDetach");
	piId = 0;
	unload();
}

void arc::device::core::PanelInterfaceLoader::load()
{
	panelInterface = createInterfaceInstance();
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

	/*if (panelInterface)
	{
		panelInterface->Stop();
		delete panelInterface;
		panelInterface = NULL;
	}
	else
	{

	}*/

	loadEvent.post(0);
}

IPanelInterface * arc::device::core::PanelInterfaceLoader::createInterfaceInstance()
{
	switch (readPins()) {
	case 1:
		Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "PanelInterfaceLoader - createInterfaceInstance() *** ElectricalOutletPanel ***");
		return new arc::device::interfaces::ElectricalOutletPanel();
	case 2:
		Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "PanelInterfaceLoader - createInterfaceInstance() *** MotionDetectorPanel ***");
		return new arc::device::interfaces::MotionDetectorPanel();
	case 3:
		Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "PanelInterfaceLoader - createInterfaceInstance() *** LightSwitchPanel ***");
		return new arc::device::interfaces::LightSwitchPanel();
	default:
		return NULL;
	}
}

uint8_t arc::device::core::PanelInterfaceLoader::readPins()
{
	uint8_t id0 = (uint8_t)pin0.read();
	uint8_t id1 = (uint8_t)pin1.read();
	uint8_t id2 = (uint8_t)pin2.read();

	uint8_t id = id0 | id1 << 1 | id2 << 2;

	return id + 1;
}
