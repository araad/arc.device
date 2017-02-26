#include "ConnectionManager.h"
#include "ESP8266Interface.h"
#include "WiFiAccessPoint.h"
#include "../utils/LogManager.h"
#include "../core/TaskManager.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::net;

ClientConnection* Client;
ESP8266Interface esp(MBED_CONF_APP_ESP8266_TX, MBED_CONF_APP_ESP8266_RX);

Mutex credMutex;

arc::device::net::ConnectionManager::ConnectionManager() {}

void arc::device::net::ConnectionManager::Start()
{
	Logger.Trace("ConnectionManager - Start()");


	Client = new ClientConnection();

	startDiscovery();
	//startConnection();
}

DigitalOut resetPin(MBED_CONF_APP_ESP8266_RESET);

void arc::device::net::ConnectionManager::resetWifi()
{
	Logger.Trace("ConnectionManager - resetWifi()");
	resetPin = 0;
	wait(0.1);
	resetPin = 1;
	wait(0.1);
}

void arc::device::net::ConnectionManager::startConnection()
{
	Logger.Trace("starting connection...");
	connectionThread = new Thread(osPriorityNormal, 4096);
	connectionThread->start(this, &ConnectionManager::connect);
}

void arc::device::net::ConnectionManager::connect()
{
	Logger.mapThreadName("Connect");
	Logger.Trace("ConnectionManager - connect()");

	resetWifi();

	credMutex.lock();
	Logger.Trace("ConnectionManager - connect(): Connecting to Network \"%s\"", ssid.c_str());
	int connError = esp.connect(ssid.c_str(), pswd.c_str());
	credMutex.unlock();
	if (connError == 0)
	{
		Logger.Trace("ConnectionManager - connect(): Connected to Network successfully");
		network = (NetworkInterface*)&esp;
		Client->Start(network);
	}
	else
	{
		Logger.Error("ConnectionManager - connect(): Connection to Network Failed %d", connError);
	}
}

void arc::device::net::ConnectionManager::startDiscovery()
{
	discoveryThread = new Thread(osPriorityNormal, 4096);
	discoveryThread->start(this, &ConnectionManager::discover);
	discoveryTimeout.attach(callback(this, &ConnectionManager::discoverTimeout_ISR), 120);
}

void arc::device::net::ConnectionManager::discover()
{
	Logger.mapThreadName("Discover");

	resetWifi();

	netDiscovery = new NetworkDiscovery(&esp, callback(this, &ConnectionManager::onDiscoverComplete));

	Logger.Trace("waiting...");
	Thread::signal_wait(0x1);

	Logger.Trace("deleting discovery");
	delete netDiscovery;
	Logger.Trace("Done");

	wait(1);
	Tasks->AddOneTimeTask(callback(this, &ConnectionManager::startConnection));
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
