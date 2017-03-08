#include "ElectricalOutletPanel.h"
#include "../utils/LogManager.h"
#include "./../core/TaskManager.h"


arc::device::interfaces::ElectricalOutletPanel::ElectricalOutletPanel()
	: service("3312", Tasks->GetQueue()),
	pwrState(A0),
	sensor(A1, 0, 65535, 10)
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
	Logger.Trace("ElectricalOutletPanel started");

	bool state = pwrState.read();
	powerStateUpdatedCallback = callback(this, &ElectricalOutletPanel::onPowerStateUpdated);
	service.AddResource("5850", "plugs", net::ResourceService::BOOLEAN, &state, &powerStateUpdatedCallback);

	float reading = sensor.GetCurrent();
	service.AddResource("5700", "sensors", net::ResourceService::FLOAT, &reading);
}

void arc::device::interfaces::ElectricalOutletPanel::Stop()
{
	Logger.Trace("ElectricalOutletPanel stopped");
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
