#include "ClientConnection.h"
#include "../utils/LogManager.h"

arc::device::net::ClientConnection::ClientConnection()
{
	Logger.Trace("ClientConnection - ctor()");
	registered = false;
	interfaceObject = M2MInterfaceFactory::create_interface(*this, MBED_ENDPOINT_NAME, "test", 60, 5684, MBED_DOMAIN, M2MInterface::TCP);

	securityObject = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
	if (securityObject)
	{
		securityObject->set_resource_value(M2MSecurity::M2MServerUri, "coap://api.connector.mbed.com:5684");
		securityObject->set_resource_value(M2MSecurity::BootstrapServer, 0);
		securityObject->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Certificate);
		securityObject->set_resource_value(M2MSecurity::ServerPublicKey, SERVER_CERT, sizeof(SERVER_CERT));
		securityObject->set_resource_value(M2MSecurity::PublicKey, CERT, sizeof(CERT));
		securityObject->set_resource_value(M2MSecurity::Secretkey, KEY, sizeof(KEY));
	}

	deviceObject = M2MInterfaceFactory::create_device();
	if (deviceObject)
	{
		deviceObject->create_resource(M2MDevice::Manufacturer, "Team22");
		deviceObject->create_resource(M2MDevice::DeviceType, "Arc.Device");
		deviceObject->create_resource(M2MDevice::ModelNumber, "Model_Number");
		deviceObject->create_resource(M2MDevice::SerialNumber, "Serial_Number");
	}

	objectList.push_back(deviceObject);
}

void arc::device::net::ClientConnection::Start(NetworkInterface * connHandler)
{
	Logger.Trace("ClientConnection - Start() begin");
	interfaceObject->set_platform_network_handler((void*)connHandler);
	interfaceObject->register_object(securityObject, objectList);
	objectList.clear();
	Logger.Trace("ClientConnection - Start() end");
}

void arc::device::net::ClientConnection::UpdateRegistration(M2MObject *object)
{
	Logger.Trace("ClientConnection - UpdateRegistration()");

	regObjectsMutex.lock();
	int index = -1;
	for (int i = 0; i < regObjects.size(); i++)
	{
		M2MObject* obj = regObjects[i];
		if (obj->name() == object->name())
		{
			index = i;
		}
	}

	if (index == -1)
	{
		regObjects.push_back(object);
	}
	regObjectsMutex.unlock();

	regMutex.lock();
	if (registered)
	{
		M2MObjectList list;
		list.push_back(object);
		interfaceObject->update_registration(securityObject, list);
	}
	else
	{
		objectList.push_back(object);
	}
	regMutex.unlock();
}

M2MObject * arc::device::net::ClientConnection::GetRegisteredObject(char * name)
{
	Logger.Trace("ClientConnection - GetRegisteredObject() %s", name);

	regObjectsMutex.lock();
	for (int i = 0; i < regObjects.size(); i++)
	{
		M2MObject* obj = regObjects[i];
		if (obj->name() == name)
		{
			Logger.Trace("ClientConnection - GetRegisteredObject() found object");
			regObjectsMutex.unlock();
			return obj;
		}
	}
	regObjectsMutex.unlock();

	return NULL;
}

void arc::device::net::ClientConnection::bootstrap_done(M2MSecurity * server_object)
{
	Logger.Trace("Bootstrap Done");
}

void arc::device::net::ClientConnection::object_registered(M2MSecurity * security_object, const M2MServer & server_object)
{
	Logger.mapThreadName("MbedClient");
	Logger.Trace("Object Registered");
	regMutex.lock();
	registered = true;
	regMutex.unlock();

	if (objectList.size() > 0)
	{
		interfaceObject->update_registration(securityObject, objectList);
		objectList.clear();
	}
}

void arc::device::net::ClientConnection::object_unregistered(M2MSecurity * server_object)
{
	Logger.Trace("Object Unregistered");
}

void arc::device::net::ClientConnection::registration_updated(M2MSecurity * security_object, const M2MServer & server_object)
{
	//Logger.Trace("Registration Updated");
}

void arc::device::net::ClientConnection::error(M2MInterface::Error error)
{
	Logger.mapThreadName("MbedClient");

	String error_code;
	switch (error)
	{
	case M2MInterface::ErrorNone:
		error_code += "M2MInterface::ErrorNone";
		break;
	case M2MInterface::AlreadyExists:
		error_code += "M2MInterface::AlreadyExists";
		break;
	case M2MInterface::BootstrapFailed:
		error_code += "M2MInterface::BootstrapFailed";
		break;
	case M2MInterface::InvalidParameters:
		error_code += "M2MInterface::InvalidParameters";
		break;
	case M2MInterface::NotRegistered:
		error_code += "M2MInterface::NotRegistered";
		break;
	case M2MInterface::Timeout:
		error_code += "M2MInterface::Timeout";
		break;
	case M2MInterface::NetworkError:
		error_code += "M2MInterface::NetworkError";
		break;
	case M2MInterface::ResponseParseFailed:
		error_code += "M2MInterface::ResponseParseFailed";
		break;
	case M2MInterface::UnknownError:
		error_code += "M2MInterface::UnknownError";
		break;
	case M2MInterface::MemoryFail:
		error_code += "M2MInterface::MemoryFail";
		break;
	case M2MInterface::NotAllowed:
		error_code += "M2MInterface::NotAllowed";
		break;
	case M2MInterface::SecureConnectionFailed:
		error_code += "M2MInterface::SecureConnectionFailed";
		break;
	case M2MInterface::DnsResolvingFailed:
		error_code += "M2MInterface::DnsResolvingFailed";
		break;
	}

	Logger.Error("ClientConnection - error(): %s", error_code.c_str());
}

void arc::device::net::ClientConnection::value_updated(M2MBase * base, M2MBase::BaseType type)
{
	Logger.Trace("Value Updated: %s", base->uri_path().c_str());
	M2MResource* resource = NULL;
	M2MResourceInstance* res_instance = NULL;
	M2MObjectInstance* obj_instance = NULL;
	M2MObject* obj = NULL;
	String object_name = "";
	String resource_name = "";
	uint16_t object_instance_id = 0;
	uint16_t resource_instance_id = 0;
	if (base) {
		switch (base->base_type()) {
		case M2MBase::Object:
			obj = (M2MObject *)base;
			object_name = obj->name();
			break;
		case M2MBase::ObjectInstance:
			obj_instance = (M2MObjectInstance *)base;
			object_name = obj_instance->name();
			object_instance_id = obj_instance->instance_id();
			break;
		case M2MBase::Resource: {
			resource = (M2MResource*)base;
			object_name = resource->object_name();
			object_instance_id = resource->object_instance_id();
			resource_name = resource->name();
			Logger.Trace("Value updated, object name %s, object instance id %d, resource name %s",
				resource->object_name().c_str(), resource->object_instance_id(), resource->name().c_str());
		}
								break;
		case M2MBase::ResourceInstance: {
			res_instance = (M2MResourceInstance*)base;
			object_name = res_instance->object_name();
			object_instance_id = res_instance->object_instance_id();
			resource_name = res_instance->name();
			resource_instance_id = res_instance->instance_id();
		}
										break;
		default:
			break;
		}
	}
}
