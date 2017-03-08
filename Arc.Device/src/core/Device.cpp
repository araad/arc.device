#include "Device.h"
#include "../utils/LogManager.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::core;
using namespace arc::device::net;

TaskManager* Tasks;
ConnectionManager* Connection;

extern "C" void __NVIC_SystemReset();

void heartbeat() {
	Logger.Trace("heartbeat");
}

arc::device::core::Device::Device()
	: aux(MBED_CONF_APP_DEVICE_AUX),
	service("dev", &queue),
	btn(MBED_CONF_APP_DEVICE_BUTTON),
	initTask("DeviceInit", callback(this, &Device::initialize), false)
{
	Logger.Trace("Device - ctor() 1");

	initTask.run();
}

arc::device::core::Device::~Device()
{
	delete piLoader;
}

void arc::device::core::Device::initialize()
{
	Tasks = new TaskManager(&queue);
	Tasks->AddRecurringTask("Heartbeat", callback(&heartbeat), 2000);
	Connection = new ConnectionManager();

	btn.Initialize();
	btn.addTapHandler(callback(this, &Device::onBtnTap));
	btn.addSingleHoldHandler(callback(this, &Device::onBtnHold));

	// Start the connection
	initTask.state = core::TaskState::ASLEEP;
	Tasks->AddOneTimeTask(callback(Connection, &ConnectionManager::StartConnection));
	initTask.state = core::TaskState::ALIVE;

	// Start the panelInterface loader
	initTask.state = core::TaskState::ASLEEP;
	piLoader = new PanelInterfaceLoader(callback(this, &Device::onPanelInterfaceLoaded));
	initTask.state = core::TaskState::ALIVE;

	Callback<void(int)> time_cb = callback(this, &Device::onSetCurrentTime);
	service.AddMethod("time", "info", ResourceService::INTEGER, &time_cb);

	Callback<void(bool)> sysHangSim_cb = callback(this, &Device::onSysHangSim);
	service.AddMethod("sys_hang", "sim", ResourceService::BOOLEAN, &sysHangSim_cb);

	int panelInterfaceId = piLoader->getPanelInterfaceId();
	service.AddResource("pi_id", "info", ResourceService::INTEGER, &panelInterfaceId);

	/*bool aux_val = aux.read();
	Callback<void(bool)> aux_cb = callback(this, &Device::onSetAux);
	service.AddResource("aux", "aux", ResourceService::BOOLEAN, &aux_val, &aux_cb);*/

	initTask.state = TaskState::ASLEEP;
	Tasks->Start();
}

void arc::device::core::Device::onSetCurrentTime(int value)
{
	Logger.Trace("onTimeUpdated %d", value);
	set_time(value);
	Logger.Trace("New Time");
}

void arc::device::core::Device::onPanelInterfaceLoaded(int piId)
{
	service.updateValue("pi_id", &piId);
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
	Connection->StartDiscovery();
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
