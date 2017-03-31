#pragma once
#include "mbed.h"
#include "../net/ConnectionManager.h"
#include "../net/ResourceService.h"

namespace arc
{
	namespace device
	{
		using namespace net;

		namespace core
		{
			class ModuleBase
			{
			public:
				virtual ~ModuleBase() {}

				virtual void Initialize()
				{
					clientRegisteredCallback = callback(this, &ModuleBase::onClientRegisteredEventHandler);
					Connection.AddClientRegisteredEventHandler(&clientRegisteredCallback);
				}
				void onClientRegisteredEventHandler()
				{
					registerResources();
				}
			protected:
				ModuleBase(const char* resName, uint8_t instance_id = 0) : service(resName, instance_id) {}
				
				ResourceService service;
				Callback<void()> clientRegisteredCallback;

				virtual void registerResources() = 0;
			};
		}
	}
}