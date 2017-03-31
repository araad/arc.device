#include "LightSwitch.h"
#include "../mbed_config_include.h"
#include "../core/TaskManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::modules::LightSwitch::LightSwitch()
	: ModuleBase::ModuleBase("3311"),
	led(MBED_CONF_APP_LIGHT_SWITCH_LED_RED, MBED_CONF_APP_LIGHT_SWITCH_LED_GREEN, MBED_CONF_APP_NIGHT_LIGHT_LED_BLUE),
	btn(MBED_CONF_APP_LIGHT_SWITCH_BUTTON, (PinMode)MBED_CONF_APP_NIGHT_LIGHT_BUTTON_MODE)
{
	auxState = 0;

	RgbLed::copyColor(led.red, solidColor);

	btn.addTapHandler(callback(this, &LightSwitch::btn_onTap));
}

arc::device::modules::LightSwitch::~LightSwitch()
{
	delete updateAuxEv;
}

void arc::device::modules::LightSwitch::Initialize()
{
	led.Initialize();
	btn.Initialize();

	led.SetFadeDuration(300);
	led.ShowSolid(solidColor);

	ModuleBase::Initialize();
}

void arc::device::modules::LightSwitch::registerResources()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "LightSwitch - registerResources() begin");

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "LightSwitch - registerResources() end");
}

void arc::device::modules::LightSwitch::onAuxUpdated(bool state)
{
	if (auxState != state)
	{
		do_switchState(state);
	}
}

void arc::device::modules::LightSwitch::btn_onTap(uint8_t tpcnt)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "tpcnt: %d", tpcnt);
	if (tpcnt == 1) {
		do_switchState(!auxState);
		updateAuxEv->post(auxState);
	}
}

void arc::device::modules::LightSwitch::do_switchState(bool newState)
{
	auxState = newState;
	if (auxState == 0) {
		RgbLed::copyColor(led.red, solidColor);
		led.ShowSolid(solidColor);
	}
	else {
		solidColor[0] = 0;
		solidColor[1] = 100;
		solidColor[2] = 0;
		led.ShowSolid(solidColor);
	}
}
