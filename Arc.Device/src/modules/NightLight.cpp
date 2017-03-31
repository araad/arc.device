#include "NightLight.h"
#include "../core/TaskManager.h"
#include "../net/ConnectionManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::modules::NightLight::NightLight(PinName rPin, PinName gPin, PinName bPin, PinName sensPin, PinName btnPin, PinMode btnMode)
	: ModuleBase::ModuleBase("4343"),
	led(rPin, gPin, bPin),
	sensor(sensPin, 10, 1000),
	powerBtn(btnPin, btnMode),
	ledStateUpdatedEv(Tasks.GetQueue(), callback(this, &NightLight::onLedStateUpdated)),
	powerModeUpdatedEv(Tasks.GetQueue(), callback(this, &NightLight::onPowerModeUpdated)),
	colorUpdatedEv(Tasks.GetQueue(), callback(this, &NightLight::onColorUpdated))
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "NightLight - ctor()");
}

arc::device::modules::NightLight::~NightLight()
{
}

void arc::device::modules::NightLight::init()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "NightLight - init()");

	ledState = Off;
	powerMode = Manual;

	led.Initialize();

	powerBtn.Initialize();
	powerBtn.addTapHandler(callback(this, &NightLight::powerBtn_onTap));
	powerBtn.addSingleHoldHandler(callback(this, &NightLight::powerBtn_onSingleHold));

	sensor.Initialize();
	sensor.addValueChangeHandler(callback(this, &NightLight::sensor_onAmbienceChange));

	RgbLed::copyColor(led.white, singleColor);

	// Initialize led state based on initial ambience state
	Tasks.AddDelayedTask(callback(this, &NightLight::sensor_onAmbienceChange), 1000);
}

void arc::device::modules::NightLight::registerResources()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "NightLight - registerResources() begin");

	init();

	bool state = (bool)ledState;
	service.AddResource("5850", "nl_switch", net::ResourceService::BOOLEAN, &state, &ledStateUpdatedEv);

	int mode = (int)powerMode;
	service.AddResource("mode", "nl_mode", net::ResourceService::INTEGER, &mode, &powerModeUpdatedEv);

	string color;
	char buffer[4];
	itoa(singleColor[0], buffer, 10);
	color.append(buffer);
	color.append(":");
	itoa(singleColor[1], buffer, 10);
	color.append(buffer);
	color.append(":");
	itoa(singleColor[2], buffer, 10);
	color.append(buffer);
	service.AddResource("5706", "nl_color", net::ResourceService::STRING, &color, &colorUpdatedEv);

	int ambience = (int)sensor.GetAmbience();
	service.AddResource("5700", "nl_sensor", net::ResourceService::INTEGER, &ambience);

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "NightLight - registerResources() end");
}

void arc::device::modules::NightLight::sensor_onAmbienceChange()
{
	int amb = (int)sensor.GetAmbience();
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "NightLight - sensor_onAmbienceChange() new value: %d", amb);
	service.updateValue("5700", &amb);

	if (powerMode == Manual) return;
	setLedState(amb == LightSensor::Dark ? On : Off);
}

void arc::device::modules::NightLight::powerBtn_onTap(uint8_t tpcnt)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "NightLight - powerBtn_onTap() tapcount is %d", tpcnt);
	if (tpcnt == 1)
	{
		if (powerMode == Auto) return;
		setLedState(ledState == On ? Off : On);
	}
}

void arc::device::modules::NightLight::powerBtn_onSingleHold()
{
	setPowerMode(powerMode == Auto ? Manual : Auto);
}

void arc::device::modules::NightLight::setLedState(LedState state)
{
	do_setLedState(state);
	bool val = (bool)ledState;
	service.updateValue("5850", &val);
}

void arc::device::modules::NightLight::setPowerMode(PowerMode mode)
{
	do_setPowerMode(mode);
	int val = (int)powerMode;
	service.updateValue("mode", &val);
}

void arc::device::modules::NightLight::onLedStateUpdated(bool state)
{
	if (powerMode != Auto && ledState != state)
	{
		do_setLedState((LedState)state);
	}
	else if (ledState != state)
	{
		bool val = (bool)ledState;
		service.updateValue("5850", &val);
	}
}

void arc::device::modules::NightLight::onPowerModeUpdated(int mode)
{
	if (powerMode != mode)
	{
		do_setPowerMode((PowerMode)mode);
	}
}

void arc::device::modules::NightLight::onColorUpdated(const char * color)
{
	char* tokens;
	tokens = strtok((char*)color, ":");
	int i = 0;
	while (tokens != NULL)
	{
		singleColor[i++] = (uint8_t)atoi(tokens);
		tokens = strtok(NULL, ":");
	}

	if (ledState == On)
	{
		led.ShowSolid(singleColor);
	}
}

void arc::device::modules::NightLight::do_setLedState(LedState state)
{
	ledState = state;
	if (ledState == On)
	{
		led.ShowSolid(singleColor);
	}
	else
	{
		led.FadeOut();
	}
}

void arc::device::modules::NightLight::do_setPowerMode(PowerMode mode)
{
	powerMode = mode;
	led.Blink(powerMode == Auto ? 2 : 0);
	if (powerMode == Auto)
	{
		int amb = (int)sensor.GetAmbience();
		setLedState(amb == LightSensor::Dark ? On : Off);
	}
}
