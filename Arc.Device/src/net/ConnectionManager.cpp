#include "ConnectionManager.h"
#include "ESP8266Interface.h"
#include "../utils/LogManager.h"
#include "WiFiAccessPoint.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

using namespace arc::device::net;

ClientConnection* Client;
ESP8266Interface esp(MBED_CONF_APP_ESP8266_TX, MBED_CONF_APP_ESP8266_RX);

arc::device::net::ConnectionManager::ConnectionManager()
	: resetPin(MBED_CONF_APP_ESP8266_RESET) {}

void arc::device::net::ConnectionManager::Start()
{
	Logger.Trace("ConnectionManager - Start()");

	Client = new ClientConnection();
	connThread = new Thread(osPriorityNormal, 4096);
	connThread->start(this, &ConnectionManager::connect);
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

	Logger.Trace("ConnectionManager - connect(): Connecting to Network \"%s\"", MBED_CONF_APP_ESP8266_SSID);
	int connError = esp.connect(MBED_CONF_APP_ESP8266_SSID, MBED_CONF_APP_ESP8266_PASSWORD);
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
