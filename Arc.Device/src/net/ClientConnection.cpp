#include "ClientConnection.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::net::ClientConnection::ClientConnection()
	: queueThread(osPriorityBelowNormal, 1280),
	queue(15 * EVENTS_EVENT_SIZE)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ClientConnection - ctor() - begin");
	registered = false;

	interfaceObject = M2MInterfaceFactory::create_interface(*this, MBED_ENDPOINT_NAME, "arc", 60, 5684, MBED_DOMAIN, M2MInterface::TCP);

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
}

void arc::device::net::ClientConnection::Start(NetworkInterface * connHandler, Event<void()>* clientRegisteredEvent, Event<void()>* clientErrorEvent)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ClientConnection - Start()");

	clientRegisteredEv = clientRegisteredEvent;
	clientErrorEv = clientErrorEvent;

	if (interfaceObject && securityObject && deviceObject)
	{
		interfaceObject->set_platform_network_handler((void*)connHandler);
		M2MObjectList list;
		list.push_back(deviceObject);
		interfaceObject->register_object(securityObject, list);
	}
	else
	{
		Logger.queue.call(LogManager::Log, LogManager::ErrorArgs(), "ClientConnection - Start() objects are not instantiated");
	}
}

void arc::device::net::ClientConnection::UpdateRegistration(M2MObject * object)
{
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ClientConnection - UpdateRegistration() queueing resource registration");
	queue.call(this, &ClientConnection::do_updateRegistration, object);
}

void arc::device::net::ClientConnection::queueThreadStarter()
{
	Logger.mapThreadName("ClientQueue");
	queue.dispatch_forever();
}

void arc::device::net::ClientConnection::do_updateRegistration(M2MObject * object)
{
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ClientConnection - do_updateRegistration() registering resource");
	M2MObjectList list;
	list.push_back(object);
	interfaceObject->update_registration(securityObject, list);
	Thread::signal_wait(1);
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ClientConnection - do_updateRegistration() new resource registered");
}

void arc::device::net::ClientConnection::bootstrap_done(M2MSecurity * server_object)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "Bootstrap Done");
}

void arc::device::net::ClientConnection::object_registered(M2MSecurity * security_object, const M2MServer & server_object)
{
	Logger.mapThreadName("MbedClient");
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ClientConnection - object_registered()");
	regMutex.lock();
	registered = true;
	regMutex.unlock();

	queueThread.start(callback(this, &ClientConnection::queueThreadStarter));

	if (clientRegisteredEv)
	{
		clientRegisteredEv->post();
	}
}

void arc::device::net::ClientConnection::object_unregistered(M2MSecurity * server_object)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ClientConnection - object_unregistered()");

	queue.break_dispatch();
}

void arc::device::net::ClientConnection::registration_updated(M2MSecurity * security_object, const M2MServer & server_object)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ClientConnection - registration_updated()");
	queueThread.signal_set(1);
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

	Logger.queue.call(LogManager::Log, LogManager::ErrorArgs(), "ClientConnection - error(): %s", error_code.c_str());

	if (clientErrorEv)
	{
		clientErrorEv->post();
	}
}

void arc::device::net::ClientConnection::value_updated(M2MBase * base, M2MBase::BaseType type)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ClientConnection - value_updated() path: %s", base->uri_path());
}
