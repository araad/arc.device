#include "ElectricalOutletPanel.h"
#include "../utils/LogManager.h"
#include "../core/TaskManager.h"

#include "../mbed_config_include.h";

using namespace arc::device::utils;


arc::device::interfaces::ElectricalOutletPanel::ElectricalOutletPanel()
	:
	outlets(
		2,
		MBED_CONF_APP_ELECTRICAL_OUTLET_A_SWITCH,
		MBED_CONF_APP_ELECTRICAL_OUTLET_A_SENSOR,
		MBED_CONF_APP_ELECTRICAL_OUTLET_B_SWITCH,
		MBED_CONF_APP_ELECTRICAL_OUTLET_B_SENSOR),
	usbEnable(MBED_CONF_APP_USB_OUTLET_ENABLE),
	nightLight(
		MBED_CONF_APP_NIGHT_LIGHT_LED_RED,
		MBED_CONF_APP_NIGHT_LIGHT_LED_GREEN,
		MBED_CONF_APP_NIGHT_LIGHT_LED_BLUE,
		MBED_CONF_APP_NIGHT_LIGHT_SENSOR,
		MBED_CONF_APP_NIGHT_LIGHT_BUTTON,
		MBED_CONF_APP_NIGHT_LIGHT_BUTTON_MODE)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutletPanel - ctor()");
	usbEnable = 1;
}

arc::device::interfaces::ElectricalOutletPanel::~ElectricalOutletPanel()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutletPanel - dtor()");
	usbEnable = 0;
}

void arc::device::interfaces::ElectricalOutletPanel::Start()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutletPanel - Start() begin");

	outlets.Initialize();
	nightLight.Initialize();

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutletPanel - Start() end");
}

void arc::device::interfaces::ElectricalOutletPanel::Stop()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutletPanel - Stop()");
}
