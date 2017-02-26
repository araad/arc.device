#include "ResourceService.h"
#include "ClientConnection.h"
#include "../utils/LogManager.h"

using namespace arc::device::net;

arc::device::net::ResourceService::ResourceService(char * name, EventQueue* queue)
{
	if (Client)
	{
		object = Client->GetRegisteredObject(name);
		if (!object)
		{
			object = M2MInterfaceFactory::create_object(name);
		}
	}
	else
	{
		object = M2MInterfaceFactory::create_object(name);
	}

	instance = object->object_instance();

	if (!instance)
	{
		instance = object->create_object_instance();
	}

	this->queue = queue;
}

arc::device::net::ResourceService::~ResourceService()
{
	Logger.Trace("ResourceService - dtor()");
}

void arc::device::net::ResourceService::AddResource(const char* name, char* category, ResourceType type, void* value, void* cb)
{
	Logger.Trace("ResourceService - AddResource(): %s", name);
	if (instance)
	{
		char buffer[20];
		int size;
		switch (type)
		{
		case INTEGER:
			size = sprintf(buffer, "%d", *(int*)value);
			break;
		case BOOLEAN:
			size = sprintf(buffer, "%d", *(bool*)value);
			break;
		case FLOAT:
			size = sprintf(buffer, "%f", *(float*)value);
			break;
		case STRING:
			size = sprintf(buffer, "%s", (*(string*)value).c_str());
			break;
		default:
			Logger.Error("ResourceService - AddResource(): Unknown Type");
			return;
		}

		M2MResource* res = instance->resource(name);
		if (!res) {
			res = instance->create_dynamic_resource(name, category, (M2MResource::ResourceType)type, true);
		}

		res->set_value((const uint8_t*)buffer, size);
		if (cb)
		{
			res->set_operation(M2MBase::GET_PUT_ALLOWED);
			res->set_value_updated_function(value_updated_callback(this, &ResourceService::onValueUpdated));
			cbMap[string(name)] = cb;
		}
		else
		{
			res->set_operation(M2MBase::GET_ALLOWED);
		}
		Client->UpdateRegistration(object);
	}
}

void arc::device::net::ResourceService::AddMethod(const char * name, char * category, ResourceType type, void* cb)
{
	Logger.Trace("ResourceService - AddMethod(): %s", name);
	M2MResource* res = instance->create_dynamic_resource(name, category, (M2MResource::ResourceType)type, true);
	res->set_operation(M2MBase::POST_ALLOWED);
	res->set_execute_function(execute_callback(this, &ResourceService::onMethodExecute));
	cbMap[string(name)] = cb;
	Client->UpdateRegistration(object);
}

void arc::device::net::ResourceService::updateValue(const char * name, void * value)
{
	Logger.Trace("ResourceService - updateValue(): %s", name);
	M2MResource* res = instance->resource(name);
	ResourceType type = (ResourceType)res->resource_instance_type();
	if (res)
	{
		char buffer[20];
		int size;
		switch (type)
		{
		case INTEGER:
			size = sprintf(buffer, "%d", *(int*)value);
			break;
		case BOOLEAN:
			size = sprintf(buffer, "%d", *(bool*)value);
			break;
		case FLOAT:
			size = sprintf(buffer, "%f", *(float*)value);
			break;
		case STRING:
			size = sprintf(buffer, "%s", (*(string*)value).c_str());
			break;
		default:
			Logger.Error("ResourceService - AddResource(): Unknown Type");
			return;
		}

		res->set_value((const uint8_t*)buffer, size);
	}
	else
	{
		Logger.Warn("ResourceService - updateValue(): resource not under observation");
	}
}

void arc::device::net::ResourceService::onValueUpdated(const char * name)
{
	Logger.Trace("ResourceService - onValueUpdated(): %s", name);
	M2MResource* res = instance->resource(name);
	ResourceType type = (ResourceType)res->resource_instance_type();
	switch (type)
	{
	case INTEGER:
	{
		Callback<void(int)> *cb = (Callback<void(int)>*)cbMap[string(name)];
		queue->call(*cb, res->get_value_int());
		break;
	}
	case BOOLEAN:
	{
		Callback<void(bool)> *cb = (Callback<void(bool)>*)cbMap[string(name)];
		queue->call(*cb, (bool)res->get_value_int());
		break;
	}
	case FLOAT:
	{
		Callback<void(float)> *cb = (Callback<void(float)>*)cbMap[string(name)];
		queue->call(*cb, atof(res->get_value_string().c_str()));
		break;
	}
	case STRING:
	{
		Callback<void(string)> *cb = (Callback<void(string)>*)cbMap[string(name)];
		string newVal(res->get_value_string().c_str());
		queue->call(*cb, newVal);
		break;
	}
	default:
		break;
	}
}

void arc::device::net::ResourceService::onMethodExecute(void* argument)
{
	Logger.Trace("ResourceService - onMethodExecute()");
	if (argument) {
		M2MResource::M2MExecuteParameter* param = (M2MResource::M2MExecuteParameter*)argument;
		String resource_name = param->get_argument_resource_name();
		uint8_t* payload = param->get_argument_value();
		int payload_length = param->get_argument_value_length();

		M2MResource* res = instance->resource(resource_name);
		ResourceType type = (ResourceType)res->resource_instance_type();
		string name(resource_name.c_str());
		switch (type)
		{
		case INTEGER:
		{
			Callback<void(int)> *cb = (Callback<void(int)>*)cbMap[name];
			res->set_value(payload, payload_length);
			queue->call(*cb, res->get_value_int());
			break;
		}
		case BOOLEAN:
		{
			Callback<void(bool)> *cb = (Callback<void(bool)>*)cbMap[name];
			res->set_value(payload, payload_length);
			queue->call(*cb, (bool)res->get_value_int());
			break;
		}
		default:
			break;
		}
	}
}
