#pragma once

#include "mbed.h"
#include "features\netsocket\NetworkInterface.h"
#include "rtos\Thread.h"
#include "ClientConnection.h"
#include "NetworkDiscovery.h"

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

				void Start();
			private:
				Mutex networkMutex;
				NetworkInterface *network;

				rtos::Thread *connectionThread;
				ClientConnection* client;

				rtos::Thread *discoveryThread;
				NetworkDiscovery* netDiscovery;
				Timeout discoveryTimeout;


				string ssid;
				string pswd;
				
				void resetWifi();
				void startConnection();
				void connect();
				void startDiscovery();
				void discover();
				void discoverTimeout_ISR();
				void discoverTimeout();
				void onDiscoverComplete(char* ssid, char* pswd);
			};
		}
	}
}

extern arc::device::net::ConnectionManager* Connection;