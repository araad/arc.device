#include "ConnectionManager.h"
#include "WiFiAccessPoint.h"
#include "../utils/LogManager.h"
#include "../core/TaskManager.h"

#include "../mbed_config_include.h"

#include "../mbed-os/features/FEATURE_COMMON_PAL/mbed-trace/mbed-trace/mbed_trace.h"

using namespace arc::device::net;
using namespace arc::device::core;
using namespace arc::device::components;
using namespace arc::device::utils;

ClientConnection Client;

extern "C" void __NVIC_SystemReset();

arc::device::net::ConnectionManager::ConnectionManager()
	: connectionThread(osPriorityNormal, 256 * 6),
	discoveryThread(osPriorityNormal, 256 * 7),
	clientRegisteredEv(Tasks.GetQueue(), callback(this, &ConnectionManager::onClientRegistered)),
	clientErrorEv(Tasks.GetQueue(), callback(this, &ConnectionManager::onClientError))
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - ctor()");

	networkInterface = 0;
	connectionTask = 0;
	discoveryTask = 0;
	currentHandlerIndex = 0;
	clientStarted = false;
	clientRegistered = false;
}

void arc::device::net::ConnectionManager::StartConnection()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - StartConnection()");

	// TODO: check if connection already started

	statusLed->blink();

	connectionTask = new Task<Callback<void()>>("Connection", callback(this, &ConnectionManager::connect), false);
	connectionThread.start(callback(&TaskBase::taskStarter<Task<Callback<void()>>>, connectionTask));
}

void arc::device::net::ConnectionManager::StartDiscovery()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - StartDiscovery() - begin");

	Thread::State discState = discoveryThread.get_state();
	if (discState != Thread::Inactive && discState != Thread::Deleted)
	{
		Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - StartDiscovery() discovery already started, cancelling...");
		return;
	}

	int delay = 0;
	Thread::State connState = connectionThread.get_state();
	if (clientStarted && (connState == Thread::Inactive || connState == Thread::Deleted))
	{
		/*Client.Unregister();
		Tasks.AddDelayedTask(callback(&Client, &ClientConnection::Stop), 1000);
		delay = 3000;*/
		Logger.queue.call(LogManager::Log, LogManager::WarnArgs(), "ConnectionManager - StartDiscovery() Mbed Client was already started, must reset for now until we have a fix for restarting");
		return;
	}
	else
	{
		Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - StartDiscovery() terminating connectionThread");
		connectionThread.terminate();
	}

	statusLed->blink(true);

	Tasks.AddDelayedTask(callback(this, &ConnectionManager::do_startDiscovery), delay);

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - StartDiscovery() - end");
}

void arc::device::net::ConnectionManager::SetStatusLed(components::Led * statusLed)
{
	this->statusLed = statusLed;
}

void arc::device::net::ConnectionManager::AddClientRegisteredEventHandler(Callback<void()>* cb)
{
	if (clientRegistered)
	{
		cb->call();
	}
	else if (currentHandlerIndex < maxHandlers)
	{
		onClientRegisteredEventHandler[currentHandlerIndex++] = cb;
	}
	else
	{
		Logger.queue.call(LogManager::Log, LogManager::WarnArgs(), "ConnectionManager - AddClientRegisteredEventHandler() Reached max of %d handlers", maxHandlers);
	}
}

void arc::device::net::ConnectionManager::do_startDiscovery()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - do_startDiscovery() - begin");

	networkInterface->disconnect();

	discoveryTask = new core::Task<Callback<void()>>("Discover", callback(this, &ConnectionManager::discover), false);
	discoveryTimeout.attach(callback(this, &ConnectionManager::discoverTimeout_ISR), 180);
	discoveryThread.start(callback(&core::TaskBase::taskStarter<core::Task<Callback<void()>>>, discoveryTask));

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - do_startDiscovery() - end");
}

void arc::device::net::ConnectionManager::resetNetworkInterface()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - resetNetworkInterface()");

	if (networkInterface)
	{
		delete networkInterface;
		networkInterface = 0;
	}
	networkInterface = new ESP8266Interface(MBED_CONF_APP_ESP8266_TX, MBED_CONF_APP_ESP8266_RX, MBED_CONF_APP_ESP8266_DEBUG);
	networkInterface->hardReset();
}

void arc::device::net::ConnectionManager::connect()
{
	Logger.mapThreadName("Connect");
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - connect() - begin");

	resetNetworkInterface();

	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ConnectionManager - connect() configuring station...");

	connectionTask->state = TaskState::ASLEEP;
	int connError = networkInterface->configureStation();
	connectionTask->state = TaskState::ALIVE;

	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ConnectionManager - connect() configuring station done, connError: %d", connError);

	connectionTask->state = TaskState::ASLEEP;
	credMutex.lock();
	connectionTask->state = TaskState::ALIVE;

	if (!ssid.empty() && !pswd.empty())
	{
		Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ConnectionManager - connect(): Connecting to Network \"%s\"", ssid.c_str());

		connectionTask->state = TaskState::ASLEEP;
		connError = networkInterface->connect(ssid.c_str(), pswd.c_str());		
		connectionTask->state = TaskState::ALIVE;

		// TODO: unable to reuse esp after discovery to connect with mbed client without resetting
		if (connError == 0) __NVIC_SystemReset();
	}

	credMutex.unlock();

	if (connError == 0)
	{
		Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ConnectionManager - connect(): Connected");

		statusLed->blink(true);

		connectionTask->state = TaskState::ASLEEP;
		Client.Start(networkInterface, &clientRegisteredEv, &clientErrorEv);
		clientStarted = true;
		connectionTask->state = TaskState::ALIVE;
	}
	else
	{
		Logger.queue.call(LogManager::Log, LogManager::ErrorArgs(), "ConnectionManager - connect(): Connection to Network Failed %d", connError);
		statusLed->pulse();
	}
	delete connectionTask;

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - connect() - end");
}

void arc::device::net::ConnectionManager::discover()
{
	Logger.mapThreadName("Discover");
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - discover() - begin");

	resetNetworkInterface();

	discoveryTask->state = TaskState::ASLEEP;
	netDiscovery = new NetworkDiscovery(networkInterface, callback(this, &ConnectionManager::onDiscoverComplete));
	discoveryTask->state = TaskState::ALIVE;

	statusLed->pulse(3);

	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ConnectionManager - discover() waiting for incoming connection...");
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

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - discover() - end");
}

void arc::device::net::ConnectionManager::discoverTimeout_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &ConnectionManager::discoverTimeout));
}

void arc::device::net::ConnectionManager::discoverTimeout()
{
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ConnectionManager - discoverTimeout() timeout before incoming connection");
	discoveryThread.signal_set(0x1);
}

void arc::device::net::ConnectionManager::onDiscoverComplete(char * ssidVal, char* pswdVal)
{
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ConnectionManager - onDiscoverComplete() received credentials for wifi network");

	ssid = ssidVal;
	pswd = pswdVal;

	discoveryTimeout.detach();
	discoveryThread.signal_set(0x1);
}

void arc::device::net::ConnectionManager::onClientRegistered()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ConnectionManager - onClientRegistered()");
	statusLed->pulse(2);
	clientRegistered = true;
	for (uint8_t i = 0; i < maxHandlers; i++)
	{
		Callback<void()>* cb = onClientRegisteredEventHandler[i];
		if (cb)
		{
			cb->call();
		}
	}
}

void arc::device::net::ConnectionManager::onClientError()
{
	__NVIC_SystemReset();
}
