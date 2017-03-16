#include "Device.h"
#include "../utils/LogManager.h"

#include "../mbed_config.h"

using namespace arc::device::core;
using namespace arc::device::net;

TaskManager Tasks;
ConnectionManager Connection;
DigitalOut mainsSwitch(MBED_CONF_APP_DEVICE_MAINS_SWITCH);

static const char* infoKey = "info";
static const char* timeKey = "time";
static const char* sys_hangKey = "sys_hang";
static const char* simKey = "sim";
static const char* pi_idKey = "pi_id";

extern "C" void __NVIC_SystemReset();

void heartbeat() {
	Logger.Trace("Heartbeat");
}

arc::device::core::Device::Device()
	: aux(MBED_CONF_APP_DEVICE_AUX),
	service("dev", Tasks.GetQueue()),
	btn(MBED_CONF_APP_DEVICE_BUTTON, (PinMode)MBED_CONF_APP_DEVICE_BUTTON_MODE),
	piLoader(callback(this, &Device::onPanelInterfaceLoaded)),
	statusLed(MBED_CONF_APP_DEVICE_STATUS_LED),
	initTask("DeviceInit", callback(this, &Device::initialize), false)
{
	initTask.run();
}

void arc::device::core::Device::initialize()
{
	Tasks.AddRecurringTask("Heartbeat", callback(&heartbeat), 2000);

	Connection.SetStatusLed(&statusLed);

	btn.Initialize();
	btn.addTapHandler(callback(this, &Device::onBtnTap));
	btn.addSingleHoldHandler(callback(this, &Device::onBtnHold));

	mainsSwitch = 1;

	// Start the connection
	initTask.state = core::TaskState::ASLEEP;
	Tasks.AddOneTimeTask(callback(&Connection, &ConnectionManager::StartConnection));
	initTask.state = core::TaskState::ALIVE;

	int panelInterfaceId = piLoader.getPanelInterfaceId();
	service.AddResource(pi_idKey, infoKey, ResourceService::INTEGER, &panelInterfaceId);

	Callback<void(int)> time_cb = callback(this, &Device::onSetCurrentTime);
	service.AddMethod(timeKey, infoKey, ResourceService::INTEGER, &time_cb);

	Callback<void(bool)> sysHangSim_cb = callback(this, &Device::onSysHangSim);
	service.AddMethod(sys_hangKey, simKey, ResourceService::BOOLEAN, &sysHangSim_cb);

	bool aux_val = aux.read();
	Callback<void(bool)> aux_cb = callback(this, &Device::onSetAux);
	service.AddResource("aux", "aux", ResourceService::BOOLEAN, &aux_val, &aux_cb);

	initTask.state = TaskState::ASLEEP;
	Tasks.Start();
}

void arc::device::core::Device::onSetCurrentTime(int value)
{
	Logger.Trace("onTimeUpdated %d", value);
	set_time(value);
	Logger.Trace("New Time");
}

void arc::device::core::Device::onPanelInterfaceLoaded(int piId)
{
	service.updateValue(pi_idKey, &piId);
}

void arc::device::core::Device::onSetAux(bool value)
{
}

void arc::device::core::Device::onBtnTap(int tapCount)
{
	if (tapCount == 1)
	{
		__NVIC_SystemReset();
	}
}

void arc::device::core::Device::onBtnHold()
{
	Connection.StartDiscovery();
}

void do_SysHangSim()
{
	while (1) {}
}

void arc::device::core::Device::onSysHangSim(bool value)
{
	Task<Callback<void()>> task("SysHangSim", callback(do_SysHangSim));
	task.run();
}
