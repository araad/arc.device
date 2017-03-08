#pragma once

#include "mbed.h"
#include "ESP8266Interface.h"
#include "rtos\Thread.h"
#include "ClientConnection.h"
#include "NetworkDiscovery.h"
#include "../core/Task.h"

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
				void StopConnection();
				void StartDiscovery();
				void StopDiscovery();
			private:
				Mutex networkMutex;
				ESP8266Interface *network;

				ClientConnection* client;
				rtos::Thread *connectionThread;
				core::Task<Callback<void()>>* connectionTask;

				NetworkDiscovery* netDiscovery;
				rtos::Thread *discoveryThread;
				core::Task<Callback<void()>>* discoveryTask;
				Timeout discoveryTimeout;


				string ssid;
				string pswd;
				
				void do_startDiscovery();
				void resetWifi();
				void connect();
				void discover();
				void discoverTimeout_ISR();
				void discoverTimeout();
				void onDiscoverComplete(char* ssid, char* pswd);
			};
		}
	}
}

extern arc::device::net::ConnectionManager* Connection;