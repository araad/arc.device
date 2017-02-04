#pragma once

#include "../security.h"
#include "mbed.h"
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2msecurity.h"
#include "mbed-client/m2mdevice.h"
#include "features\netsocket\NetworkInterface.h"
#include "rtos\Mutex.h"

#define MBED_SERVER_ADDRESS "coap://api.connector.mbed.com:5684"

namespace arc
{
	namespace device
	{
		namespace net
		{
			class ClientConnection : M2MInterfaceObserver
			{
			public:
				ClientConnection();

				void Start(NetworkInterface* connHandler);
				void UpdateRegistration(M2MObject *object);
			private:
				M2MInterface* interfaceObject;
				M2MSecurity* securityObject;
				M2MDevice* deviceObject;
				M2MObjectList objectList;
				bool registered;
				rtos::Mutex regMutex;

				// Inherited via M2MInterfaceObserver
				virtual void bootstrap_done(M2MSecurity * server_object) override;
				virtual void object_registered(M2MSecurity * security_object, const M2MServer & server_object) override;
				virtual void object_unregistered(M2MSecurity * server_object) override;
				virtual void registration_updated(M2MSecurity * security_object, const M2MServer & server_object) override;
				virtual void error(M2MInterface::Error error) override;
				virtual void value_updated(M2MBase * base, M2MBase::BaseType type) override;
			};
		}
	}
}

extern arc::device::net::ClientConnection* Client;