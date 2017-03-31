#pragma once

#include "mbed.h"
#include "ESP8266Interface.h"
#include "rtos\Thread.h"
#include "ClientConnection.h"
#include "NetworkDiscovery.h"
#include "../core/Task.h"
#include "../components/Led.h"

#ifdef MBED_CONFIG_INTELLISENSE
#include "../BUILD/NUCLEO_F401RE/GCC_ARM/mbed_config.h"
#endif

namespace arc
{
	namespace device
	{
		namespace net
		{
			class ConnectionManager
			{
			public:
				ConnectionManager();

				void StartConnection();
				void StartDiscovery();
				void SetStatusLed(components::Led* statusLed);
				void AddClientRegisteredEventHandler(Callback<void()>* cb);
			private:
				ESP8266Interface* networkInterface;
				components::Led* statusLed;

				Thread connectionThread;
				core::Task<Callback<void()>>* connectionTask;

				Timeout clientTimeout;

				NetworkDiscovery* netDiscovery;
				Thread discoveryThread;
				core::Task<Callback<void()>>* discoveryTask;
				Timeout discoveryTimeout;

				Event<void()> clientRegisteredEv;
				static const uint8_t maxHandlers = 5;
				uint8_t currentHandlerIndex;
				Callback<void()>* onClientRegisteredEventHandler[maxHandlers];
				bool clientStarted;
				bool clientRegistered;
				Event<void()> clientErrorEv;

				string ssid;
				string pswd;
				Mutex credMutex;
				
				void resetNetworkInterface();
				void do_startDiscovery();
				void connect();
				void discover();
				void discoverTimeout_ISR();
				void discoverTimeout();
				void onDiscoverComplete(char* ssid, char* pswd);
				void onClientRegistered();
				void onClientError();
			};
		}
	}
}

extern arc::device::net::ConnectionManager Connection;