#include "Device.h"
#include "../utils/LogManager.h"

using namespace arc::device::core;
using namespace arc::device::net;

arc::device::core::TaskManager* Tasks;
arc::device::net::ConnectionManager* Connection;

arc::device::core::Device::Device()
	: service("dev", &queue)
{
	Logger.Trace("Device - ctor()");

	Tasks = new TaskManager(&queue);
	Connection = new ConnectionManager();

	// Start the connection manager
	Connection->Start();

	// Start the panelInterface loader
	piLoader = new PanelInterfaceLoader(callback(this, &Device::onPanelInterfaceLoaded));

	Callback<void(int)> cb = callback(this, &Device::onTimeUpdated);
	service.AddResource("time", "info", ResourceService::INTEGER, NULL, &cb);
	bool loaded = piLoader->isLoaded();
	service.AddResource("pi_loaded", "info", ResourceService::BOOLEAN, &loaded);

	Tasks->Start();
}

void arc::device::core::Device::onTimeUpdated(int value)
{
	Logger.Trace("onTimeUpdated %d", value);
	set_time(value);
	Logger.Trace("New Time");
}

void arc::device::core::Device::onPanelInterfaceLoaded(bool value)
{
	service.updateValue("pi_loaded", ResourceService::BOOLEAN, &value);
}
