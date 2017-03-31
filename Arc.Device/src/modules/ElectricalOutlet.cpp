#include "ElectricalOutlet.h"
#include "../core/TaskManager.h"
#include "../core/MainsController.h"
#include "../net/ConnectionManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::modules::ElectricalOutlet::ElectricalOutlet(PinName pwrStatePin, PinName sensorPin, uint8_t instance_id)
	: ModuleBase::ModuleBase("3312", instance_id),
	pwrState(pwrStatePin),
	sensor(sensorPin),
	powerStateUpdatedEv(Tasks.GetQueue(), callback(this, &ElectricalOutlet::onPowerStateUpdated))
{
	this->instance_id = instance_id;
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutlet - ctor()  [instance_id: %d]", instance_id);
	pwrState = 0;
	sensor.addValueChangeHandler(callback(this, &ElectricalOutlet::onCurrentChange));
}

arc::device::modules::ElectricalOutlet::~ElectricalOutlet()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutlet - dtor()  [instance_id: %d]", instance_id);

	MainsCtrl.stopCurrentWatch(&sensor, instance_id);
}

void arc::device::modules::ElectricalOutlet::Initialize()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutlet - Initialize() [instance_id: %d]", instance_id);

	ModuleBase::Initialize();
}

void arc::device::modules::ElectricalOutlet::registerResources()
{
	init();

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutlet - registerResources() [instance_id: %d]", instance_id);
	bool state = pwrState;
	service.AddResource("5850", "plugs", net::ResourceService::BOOLEAN, &state, &powerStateUpdatedEv);

	float reading = sensor.GetCurrent();
	service.AddResource("5800", "power", net::ResourceService::FLOAT, &reading);
}

void arc::device::modules::ElectricalOutlet::init()
{
	sensor.Initialize();
	MainsCtrl.startCurrentWatch(&sensor, instance_id);
}

void arc::device::modules::ElectricalOutlet::onPowerStateUpdated(bool value)
{
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ElectricalOutlet - onPowerStateUpdated() [instance_id: %d] new value = %d", instance_id, value);
	pwrState = value;
}

void arc::device::modules::ElectricalOutlet::onCurrentChange()
{
	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutlet - onCurrentChange() begin");
	float current = sensor.GetCurrent();
	float voltage = MainsCtrl.getVoltage();
	float power = current * voltage;
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ElectricalOutlet - onCurrentChange() [instance_id: %d] %fV", instance_id, voltage);
	service.updateValue("5800", &power);
	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ElectricalOutlet - onCurrentChange() end");
}
