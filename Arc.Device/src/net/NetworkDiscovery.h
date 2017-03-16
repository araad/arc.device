#pragma once

#include "mbed.h"
#include "rtos\Thread.h"
#include "ESP8266Interface.h"
#include "features\netsocket\WiFiAccessPoint.h"
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
				Thread espThread;
				static const int maxApCount = 50;
				static const int maxApPerResponse = 5;
				int apCount = maxApCount;
				int apToSend = maxApCount;
				int apResponseCount;
				int currentResponseIndex = 0;
				WiFiAccessPoint apList[maxApCount];

				void scanNetworks();
				void listen();
				void sendAPList();
				void sendDeviceInfo();
				void receiveAPCredentials(string request);
			};
		}
	}
}