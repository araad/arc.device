#pragma once

#include "mbed.h"
#include "rtos\Thread.h"
#include "ESP8266Interface.h"
#include <string>

namespace arc
{
	namespace device
	{
		namespace net
		{
			class NetworkDiscovery
			{
			public:
				NetworkDiscovery(ESP8266Interface* esp, Callback<void(char*, char*)> discoverComplete);
				~NetworkDiscovery();
			private:
				ESP8266Interface* esp;
				Callback<void(char*, char*)>* discoverComplete;
				Thread* espThread;

				void listen();
				void sendAPList();
				void receiveAPCredentials(string request);
			};
		}
	}
}