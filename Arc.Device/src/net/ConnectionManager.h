#pragma once

#include "mbed.h"
#include "features\netsocket\NetworkInterface.h"
#include "rtos\Thread.h"
#include "ClientConnection.h"

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
				NetworkInterface *network;
				DigitalOut resetPin;
				rtos::Thread *connThread;
				ClientConnection* client;
				
				void resetWifi();
				void connect();
				void runTask();
			};
		}
	}
}

extern arc::device::net::ConnectionManager* Connection;