#include "Device.h"
#include "TaskManager.h"
#include "MainsController.h"
#include "../utils/LogManager.h"
#include "../net/ConnectionManager.h"
#include "../mbed_config_include.h"

using namespace arc::device::core;
using namespace arc::device::net;
using namespace arc::device::utils;

TaskManager Tasks;
MainsController MainsCtrl;
ConnectionManager Connection;

static const char* infoKey = "info";
static const char* timeKey = "time";
static const char* sys_hangKey = "sys_hang";
static const char* simKey = "sim";
static const char* pi_idKey = "pi_id";

extern "C" void __NVIC_SystemReset();

void reset()
{
	__NVIC_SystemReset();
}

void heartbeat() {
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "Heartbeat");
}

arc::device::core::Device::Device()
	: ModuleBase::ModuleBase("dev"),
	aux(MBED_CONF_APP_DEVICE_AUX),
	btn(MBED_CONF_APP_DEVICE_BUTTON, MBED_CONF_APP_DEVICE_BUTTON_MODE),
	statusLed(MBED_CONF_APP_DEVICE_STATUS_LED),
	setCurrentTimeEv(Tasks.GetQueue(), callback(this, &Device::onSetCurrentTime)),
	auxUpdatedEv(Tasks.GetQueue(), callback(this, &Device::onAuxUpdated)),
	initTask("DeviceInit", callback(this, &Device::Initialize), false),
	piLoader(callback(this, &Device::onPanelInterfaceChanged))
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Device - ctor()");

	aux = 0;
	auxCtrl = NULL;

	initTask.run();
}

void arc::device::core::Device::Initialize()
{
	Tasks.AddRecurringTask("Heartbeat", callback(&heartbeat), 2000);

	Connection.SetStatusLed(&statusLed);

	btn.Initialize();
	btn.addTapHandler(callback(this, &Device::onBtnTap));
	btn.addSingleHoldHandler(callback(this, &Device::onBtnHold));

	// Start the connection
	initTask.state = core::TaskState::ASLEEP;
	Tasks.AddOneTimeTask(callback(&Connection, &ConnectionManager::StartConnection));
	initTask.state = core::TaskState::ALIVE;

	ModuleBase::Initialize();

	initTask.state = TaskState::ASLEEP;
	Tasks.Start();
}

void arc::device::core::Device::registerResources()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Device - registerResources() begin");

	int panelInterfaceId = (int)piLoader.getPanelInterfaceId();
	service.AddResource(pi_idKey, infoKey, ResourceService::INTEGER, &panelInterfaceId);

	bool aux_val = aux.read();
	service.AddResource("aux", "aux", ResourceService::BOOLEAN, &aux_val, &auxUpdatedEv);

	service.AddMethod(timeKey, infoKey, ResourceService::INTEGER, &setCurrentTimeEv);

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Device - registerResources() end");
}

void arc::device::core::Device::onSetCurrentTime(int value)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Device - onTimeUpdated()");
	set_time(value);
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "Device - onTimeUpdated() new time: %d", value);
}

void arc::device::core::Device::onPanelInterfaceChanged(uint8_t piId)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Device - onPanelInterfaceChanged() id:%d", piId);
	switch (piId)
	{
	case 1:
		MainsCtrl.enableMainPower();
		break;
	case 3:
		auxCtrl = (IAuxController*)piLoader.getPanelInterface();
		Event<void(bool)>* ev = new Event<void(bool)>(Tasks.GetQueue(), callback(this, &Device::setAux));
		auxCtrl->setUpdateAuxEv(ev);
	default:
		MainsCtrl.disableMainPower();
		break;
	}

	int val = (int)piId;
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Device - onPanelInterfaceChanged() calling service", piId);
	service.updateValue(pi_idKey, &val);
	if (val == 0)
	{
		Tasks.AddDelayedTask(callback(reset), 1000);
	}
}

void arc::device::core::Device::onAuxUpdated(bool value)
{
	aux = value;
	if (auxCtrl)
	{
		auxCtrl->onAuxUpdated(value);
	}
}

void arc::device::core::Device::setAux(bool state)
{
	aux = state;
	bool val = aux.read();
	service.updateValue("aux", &val);
}

void arc::device::core::Device::onBtnTap(uint8_t tapCount)
{
	if (tapCount == 1)
	{
		reset();
	}
}

void arc::device::core::Device::onBtnHold()
{
	Connection.StartDiscovery();
}
