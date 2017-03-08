#include "ConnectionManager.h"
#include "WiFiAccessPoint.h"
#include "../utils/LogManager.h"
#include "../core/TaskManager.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::net;
using namespace arc::device::core;

ClientConnection* Client;

Mutex credMutex;

arc::device::net::ConnectionManager::ConnectionManager()
{
	network = 0;

	connectionTask = 0;
	connectionThread = 0;
	discoveryTask = 0;
	discoveryThread = 0;

	Client = new ClientConnection();
}

void arc::device::net::ConnectionManager::StartConnection()
{
	Logger.Trace("starting connection...");
	if (discoveryThread)
	{
		delete discoveryThread;
		discoveryThread = 0;
	}

	if (network)
	{
		delete network;
		network = 0;
	}

	connectionTask = new Task<Callback<void()>>("Connection", callback(this, &ConnectionManager::connect), false);
	connectionThread = new Thread(osPriorityNormal, 512 * 3);
	connectionThread->start(callback(&TaskBase::taskStarter<Task<Callback<void()>>>, connectionTask));
}

void arc::device::net::ConnectionManager::StopConnection()
{
}

void arc::device::net::ConnectionManager::StartDiscovery()
{
	Logger.Trace("starting discovery...");
	int delay = 0;
	if (connectionThread)
	{
		Client->Unregister();
		Tasks->AddDelayedTask(callback(Client, &ClientConnection::Stop), 1000);
		delete connectionThread;
		connectionThread = 0;
		delay = 3000;
	}

	Tasks->AddDelayedTask(callback(this, &ConnectionManager::do_startDiscovery), delay);
}

DigitalOut resetPin(MBED_CONF_APP_ESP8266_RESET);

void arc::device::net::ConnectionManager::do_startDiscovery()
{
	if (network)
	{
		network->disconnect();
		delete network;
		network = 0;
	}

	discoveryTask = new core::Task<Callback<void()>>("Discover", callback(this, &ConnectionManager::discover), false);
	discoveryThread = new Thread(osPriorityNormal, 256 * 7);
	discoveryTimeout.attach(callback(this, &ConnectionManager::discoverTimeout_ISR), 180);
	discoveryThread->start(callback(&core::TaskBase::taskStarter<core::Task<Callback<void()>>>, discoveryTask));
}

void arc::device::net::ConnectionManager::resetWifi()
{
	Logger.Trace("ConnectionManager - resetWifi()");
	resetPin = 0;
	wait(0.1);
	resetPin = 1;
	wait(0.1);
}

void arc::device::net::ConnectionManager::connect()
{
	Logger.mapThreadName("Connect");
	Logger.Trace("ConnectionManager - connect()");

	resetWifi();

	int connError = 1;
	connectionTask->state = TaskState::ASLEEP;
	credMutex.lock();
	connectionTask->state = TaskState::ALIVE;

	if (!ssid.empty() && !pswd.empty())
	{
		Logger.Trace("ConnectionManager - connect(): Connecting to Network \"%s\"", ssid.c_str());

		connectionTask->state = TaskState::ASLEEP;
		network = new ESP8266Interface(MBED_CONF_APP_ESP8266_TX, MBED_CONF_APP_ESP8266_RX);
		connError = network->connect(ssid.c_str(), pswd.c_str());
		connectionTask->state = TaskState::ALIVE;
	}

	credMutex.unlock();

	if (connError == 0)
	{
		Logger.Trace("ConnectionManager - connect(): Connected");
		connectionTask->state = TaskState::ASLEEP;
		Client->Start(network);
		connectionTask->state = TaskState::ALIVE;
	}
	else
	{
		Logger.Error("ConnectionManager - connect(): Connection to Network Failed %d", connError);
	}

	delete connectionTask;
}

void arc::device::net::ConnectionManager::discover()
{
	Logger.mapThreadName("Discover");

	discoveryTask->state = TaskState::ASLEEP;
	resetWifi();
	network = new ESP8266Interface(MBED_CONF_APP_ESP8266_TX, MBED_CONF_APP_ESP8266_RX);
	netDiscovery = new NetworkDiscovery(network, callback(this, &ConnectionManager::onDiscoverComplete));

	Logger.Trace("waiting...");
	Thread::signal_wait(0x1);

	Logger.Trace("deleting discovery");
	delete netDiscovery;
	discoveryTask->state = TaskState::ALIVE;
	Logger.Trace("Done");

	//wait(1);
	Tasks->AddOneTimeTask(callback(this, &ConnectionManager::StartConnection));

	delete discoveryTask;
}

void arc::device::net::ConnectionManager::discoverTimeout_ISR()
{
	Tasks->AddOneTimeTask(callback(this, &ConnectionManager::discoverTimeout));
}

void arc::device::net::ConnectionManager::discoverTimeout()
{
	Logger.Trace("ConnectionManager - discoverTimeout()");
	discoveryThread->signal_set(0x1);
}

void arc::device::net::ConnectionManager::onDiscoverComplete(char * ssidVal, char* pswdVal)
{
	Logger.Trace("ssid: %s pswd: %s", ssidVal, pswdVal);

	ssid = ssidVal;
	pswd = pswdVal;

	Logger.Trace("after __ssid: %s __pswd: %s", ssid.c_str(), pswd.c_str());

	discoveryTimeout.detach();
	discoveryThread->signal_set(0x1);
}
