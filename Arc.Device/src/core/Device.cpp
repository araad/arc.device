#include "Device.h"
#include "../utils/LogManager.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::core;
using namespace arc::device::net;

arc::device::core::TaskManager* Tasks;
arc::device::net::ConnectionManager* Connection;

arc::device::core::Device::Device()
	: aux0(MBED_CONF_APP_DEVICE_AUX_0), aux1(MBED_CONF_APP_DEVICE_AUX_1), service("dev", &queue)
{
	Logger.Trace("Device - ctor()");

	Tasks = new TaskManager(&queue);
	Connection = new ConnectionManager();

	// Start the connection manager
	Connection->Start();

	// Start the panelInterface loader
	piLoader = new PanelInterfaceLoader(callback(this, &Device::onPanelInterfaceLoaded));

	Callback<void(int)> time_cb = callback(this, &Device::onSetCurrentTime);
	service.AddMethod("time", "info", ResourceService::INTEGER, &time_cb);

	int panelInterfaceId = piLoader->getPanelInterfaceId();
	Logger.Trace("panelInterfaceId: %d", panelInterfaceId);
	service.AddResource("pi_id", "info", ResourceService::INTEGER, &panelInterfaceId);

	bool aux0_val = aux0.read();
	Callback<void(bool)> aux0_cb = callback(this, &Device::onSetAux0);
	service.AddResource("aux0", "aux", ResourceService::BOOLEAN, &aux0_val, &aux0_cb);

	bool aux1_val = aux1.read();
	Callback<void(bool)> aux1_cb = callback(this, &Device::onSetAux1);
	service.AddResource("aux1", "aux", ResourceService::BOOLEAN, &aux1_val, &aux1_cb);

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

void arc::device::core::Device::onSetAux0(bool value)
{
}

void arc::device::core::Device::onSetAux1(bool value)
{
}
