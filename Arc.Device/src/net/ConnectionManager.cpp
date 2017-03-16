#include "ConnectionManager.h"
#include "WiFiAccessPoint.h"
#include "../utils/LogManager.h"
#include "../core/TaskManager.h"

#include "../mbed_config.h"

#include "../mbed-os/features/FEATURE_COMMON_PAL/mbed-trace/mbed-trace/mbed_trace.h"

using namespace arc::device::net;
using namespace arc::device::core;
using namespace arc::device::components;

ClientConnection Client;

arc::device::net::ConnectionManager::ConnectionManager()
	: connectionThread(osPriorityNormal, 512 * 3),
	discoveryThread(osPriorityNormal, 256 * 7),
	clientRegisteredEv(Tasks.GetQueue(), callback(this, &ConnectionManager::onClientRegistered))
{
	Logger.Trace("ConnectionManager - ctor() - begin");

	networkInterface = 0;
	connectionTask = 0;
	discoveryTask = 0;

	Logger.Trace("ConnectionManager - ctor() - end");
}

void arc::device::net::ConnectionManager::StartConnection()
{
	Logger.Trace("ConnectionManager - StartConnection() - begin");

	// TODO: check if connection already started

	statusLed->blink();

	connectionTask = new Task<Callback<void()>>("Connection", callback(this, &ConnectionManager::connect), false);
	connectionThread.start(callback(&TaskBase::taskStarter<Task<Callback<void()>>>, connectionTask));

	Logger.Trace("ConnectionManager - StartConnection() - end");
}

void arc::device::net::ConnectionManager::StartDiscovery()
{
	Logger.Trace("ConnectionManager - StartDiscovery() - begin");

	Thread::State discState = discoveryThread.get_state();
	if (discState != Thread::Inactive && discState != Thread::Deleted)
	{
		Logger.Trace("ConnectionManager - StartDiscovery() discovery already started, cancelling...");
		return;
	}

	int delay = 0;
	Thread::State connState = connectionThread.get_state();
	if (connState == Thread::Inactive || connState == Thread::Deleted)
	{
		/*Client.Unregister();
		Tasks.AddDelayedTask(callback(&Client, &ClientConnection::Stop), 1000);
		delay = 3000;*/
		Logger.Warn("ConnectionManager - StartDiscovery() Mbed Client was already started, must reset for now until we have a fix for restarting");
		return;
	}
	else
	{
		Logger.Trace("ConnectionManager - StartDiscovery() terminating connectionThread");
		connectionThread.terminate();
	}

	statusLed->blink(true);

	Tasks.AddDelayedTask(callback(this, &ConnectionManager::do_startDiscovery), delay);

	Logger.Trace("ConnectionManager - StartDiscovery() - end");
}

void arc::device::net::ConnectionManager::SetStatusLed(components::Led * statusLed)
{
	this->statusLed = statusLed;
}

void arc::device::net::ConnectionManager::do_startDiscovery()
{
	Logger.Trace("ConnectionManager - do_startDiscovery() - begin");

	networkInterface->disconnect();

	discoveryTask = new core::Task<Callback<void()>>("Discover", callback(this, &ConnectionManager::discover), false);
	discoveryTimeout.attach(callback(this, &ConnectionManager::discoverTimeout_ISR), 180);
	discoveryThread.start(callback(&core::TaskBase::taskStarter<core::Task<Callback<void()>>>, discoveryTask));

	Logger.Trace("ConnectionManager - do_startDiscovery() - end");
}

void arc::device::net::ConnectionManager::resetNetworkInterface()
{
	Logger.Trace("ConnectionManager - resetNetworkInterface()");

	if (networkInterface)
	{
		delete networkInterface;
		networkInterface = 0;
	}
	networkInterface = new ESP8266Interface(MBED_CONF_APP_ESP8266_TX, MBED_CONF_APP_ESP8266_RX, MBED_CONF_APP_ESP8266_RESET);
	networkInterface->hardReset();
}

void arc::device::net::ConnectionManager::connect()
{
	Logger.mapThreadName("Connect");
	Logger.Trace("ConnectionManager - connect() - begin");

	resetNetworkInterface();

	Logger.Trace("ConnectionManager - connect() configuring station...");

	connectionTask->state = TaskState::ASLEEP;
	int connError = networkInterface->configureStation();
	connectionTask->state = TaskState::ALIVE;

	Logger.Trace("ConnectionManager - connect() configuring station done, connError: %d", connError);

	connectionTask->state = TaskState::ASLEEP;
	credMutex.lock();
	connectionTask->state = TaskState::ALIVE;

	if (!ssid.empty() && !pswd.empty())
	{
		Logger.Trace("ConnectionManager - connect(): Connecting to Network \"%s\"", ssid.c_str());

		connectionTask->state = TaskState::ASLEEP;
		connError = networkInterface->connect(ssid.c_str(), pswd.c_str());
		connectionTask->state = TaskState::ALIVE;
	}

	credMutex.unlock();

	if (connError == 0)
	{
		Logger.Trace("ConnectionManager - connect(): Connected");

		statusLed->blink(true);

		connectionTask->state = TaskState::ASLEEP;
		Client.Start(networkInterface, &clientRegisteredEv);
		connectionTask->state = TaskState::ALIVE;
	}
	else
	{
		Logger.Error("ConnectionManager - connect(): Connection to Network Failed %d", connError);
		statusLed->solid();
	}
	delete connectionTask;

	Logger.Trace("ConnectionManager - connect() - end");
}

void arc::device::net::ConnectionManager::discover()
{
	Logger.mapThreadName("Discover");
	Logger.Trace("ConnectionManager - discover() - begin");

	resetNetworkInterface();

	discoveryTask->state = TaskState::ASLEEP;
	netDiscovery = new NetworkDiscovery(networkInterface, callback(this, &ConnectionManager::onDiscoverComplete));
	discoveryTask->state = TaskState::ALIVE;

	statusLed->pulse(3);

	Logger.Trace("waiting...");
	discoveryTask->state = TaskState::ASLEEP;
	Thread::signal_wait(0x1);
	discoveryTask->state = TaskState::ALIVE;

	statusLed->blink(true);

	discoveryTask->state = TaskState::ASLEEP;
	delete netDiscovery;
	discoveryTask->state = TaskState::ALIVE;

	statusLed->solid();

	Tasks.AddOneTimeTask(callback(this, &ConnectionManager::StartConnection));

	delete discoveryTask;

	Logger.Trace("ConnectionManager - discover() - end");
}

void arc::device::net::ConnectionManager::discoverTimeout_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &ConnectionManager::discoverTimeout));
}

void arc::device::net::ConnectionManager::discoverTimeout()
{
	Logger.Trace("ConnectionManager - discoverTimeout()");
	discoveryThread.signal_set(0x1);
}

void arc::device::net::ConnectionManager::onDiscoverComplete(char * ssidVal, char* pswdVal)
{
	Logger.Trace("ssid: %s pswd: %s", ssidVal, pswdVal);

	ssid = ssidVal;
	pswd = pswdVal;

	Logger.Trace("after __ssid: %s __pswd: %s", ssid.c_str(), pswd.c_str());

	discoveryTimeout.detach();
	discoveryThread.signal_set(0x1);
}

void arc::device::net::ConnectionManager::onClientRegistered()
{
	Logger.Trace("ConnectionManager - onClientRegistered()");
	statusLed->pulse();
}
