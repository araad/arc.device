#include "ElectricalOutletPanel.h"
#include "../utils/LogManager.h"
#include "./../core/TaskManager.h"

#include "../mbed_config.h";


arc::device::interfaces::ElectricalOutletPanel::ElectricalOutletPanel()
	: service("3312", Tasks.GetQueue()),
	pwrState(MBED_CONF_APP_ELECTRICAL_OUTLET_PLUG_B_SWITCH),
	sensor(MBED_CONF_APP_ELECTRICAL_OUTLET_PLUG_B_SENSOR, 0, 4000, 10, 3000)
{
	pwrState = 0;
	sensor.Initialize();
	sensor.addCurrentChangeHandler(callback(this, &ElectricalOutletPanel::onCurrentChange));
}

arc::device::interfaces::ElectricalOutletPanel::~ElectricalOutletPanel()
{
	Logger.Trace("ElectricalOutletPanel - dtor()");
}

void arc::device::interfaces::ElectricalOutletPanel::Start()
{
	Logger.Trace("ElectricalOutletPanel - Start() begin");

	bool state = pwrState.read();
	powerStateUpdatedCallback = callback(this, &ElectricalOutletPanel::onPowerStateUpdated);
	service.AddResource("5850", "plugs", net::ResourceService::BOOLEAN, &state, &powerStateUpdatedCallback);

	float reading = sensor.GetCurrent();
	service.AddResource("5700", "sensors", net::ResourceService::FLOAT, &reading);

	Tasks.AddDelayedTask(callback(this, &ElectricalOutletPanel::onPowerStateUpdated), true, 20000);

	Logger.Trace("ElectricalOutletPanel - Start() end");
}

void arc::device::interfaces::ElectricalOutletPanel::Stop()
{
	Logger.Trace("ElectricalOutletPanel - Stop()");
}

void arc::device::interfaces::ElectricalOutletPanel::onPowerStateUpdated(bool value)
{
	Logger.Trace("ElectricalOutletPanel - onPowerStateUpdated() new value = %d", value);
	pwrState = value;
}

void arc::device::interfaces::ElectricalOutletPanel::onCurrentChange()
{
	float reading = sensor.GetCurrent();
	Logger.Trace("Current: %f", reading);
	service.updateValue("5700", &reading);
}
