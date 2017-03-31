#include "ResourceService.h"
#include "ClientConnection.h"
#include "../utils/LogManager.h"

using namespace arc::device::net;
using namespace arc::device::utils;

uint8_t ResourceService::objectCount = 0;
M2MObject* ResourceService::objects[];

arc::device::net::ResourceService::ResourceService(const char * name, uint8_t instance_id)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - ctor()");

	object = NULL;
	instance = NULL;
	this->name = name;
	this->instance_id = instance_id;
}

arc::device::net::ResourceService::~ResourceService()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - dtor()");

	object->remove_object_instance(instance->instance_id());
}

void arc::device::net::ResourceService::AddResource(const char* name, const char* category, ResourceType type, void* value, void* ev)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - AddResource(): %s - begin", name);

	if (!instance)
	{
		init();
	}

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
			Logger.queue.call(LogManager::Log, LogManager::ErrorArgs(), "ResourceService - AddResource(): Unknown Type");
			return;
		}

		M2MResource* res = instance->create_dynamic_resource(name, category, (M2MResource::ResourceType)type, true);
		res->set_value((const uint8_t*)buffer, size);
		if (ev)
		{
			res->set_operation(M2MBase::GET_PUT_ALLOWED);
			res->set_value_updated_function(value_updated_callback(this, &ResourceService::onValueUpdated));
			eventMap[string(name)] = ev;
		}
		else
		{
			res->set_operation(M2MBase::GET_ALLOWED);
		}
		resNames.push_back(name);
		Client.UpdateRegistration(object);
	}
	else
	{
		Logger.queue.call(LogManager::Log, LogManager::WarnArgs(), "ResourceService - AddResource() instance not found");
	}
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - AddResource(): %s - end", name);
}

void arc::device::net::ResourceService::AddMethod(const char * name, const char * category, ResourceType type, void* ev)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - AddMethod(): %s - begin", name);

	if (!instance)
	{
		init();
	}

	if (instance)
	{
		M2MResource* res = instance->create_dynamic_resource(name, category, (M2MResource::ResourceType)type, true);
		res->set_operation(M2MBase::POST_ALLOWED);
		res->set_execute_function(execute_callback(this, &ResourceService::onMethodExecute));
		eventMap[string(name)] = ev;
		Client.UpdateRegistration(object);
	}
	else
	{
		Logger.queue.call(LogManager::Log, LogManager::WarnArgs(), "ResourceService - AddMethod() instance not found");
	}
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - AddMethod(): %s - end", name);
}

void arc::device::net::ResourceService::updateValue(const char * name, void * value)
{
	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - updateValue() %s/%d/%s - begin", object->name(), instance->instance_id(), name);
	if (instance)
	{
		M2MResource* res = instance->resource(name);
		if (res)
		{
			ResourceType type = (ResourceType)res->resource_instance_type();
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
				Logger.queue.call(LogManager::Log, LogManager::ErrorArgs(), "ResourceService - AddResource(): Unknown Type");
				return;
			}

			res->set_value((const uint8_t*)buffer, size);
		}
		else
		{
			//Logger.queue.call(LogManager::Log, LogManager::WarnArgs(), "ResourceService - updateValue() %s/%d/%s resource not under observation", object->name(), instance->instance_id(), name);
		}
	}

	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - updateValue() %s/%d/%s - end", object->name(), instance->instance_id(), name);
}

void arc::device::net::ResourceService::init()
{
	for (uint8_t i = 0; i < objectCount; i++)
	{
		M2MObject* storedObj = objects[i];
		if (storedObj && strcmp(storedObj->name(), name) == 0)
		{
			object = storedObj;
			break;
		}
	}

	if (!object && objectCount < maxObjectCount)
	{
		object = M2MInterfaceFactory::create_object(name);
		objects[objectCount++] = object;
	}

	instance = object->create_object_instance(instance_id);
}

void arc::device::net::ResourceService::onValueUpdated(const char * name)
{
	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - onValueUpdated(): %s - begin", name);
	M2MResource* res = instance->resource(name);
	ResourceType type = (ResourceType)res->resource_instance_type();
	switch (type)
	{
	case INTEGER:
	{
		Event<void(int)> *ev = (Event<void(int)>*)eventMap[string(name)];
		ev->post(res->get_value_int());
		break;
	}
	case BOOLEAN:
	{
		Event<void(bool)> *ev = (Event<void(bool)>*)eventMap[string(name)];
		ev->post((bool)res->get_value_int());
		break;
	}
	case FLOAT:
	{
		Event<void(float)> *ev = (Event<void(float)>*)eventMap[string(name)];
		ev->post(atof(res->get_value_string().c_str()));
		break;
	}
	case STRING:
	{
		Event<void(const char*)> *ev = (Event<void(const char*)>*)eventMap[string(name)];
		ev->post(res->get_value_string().c_str());
		break;
	}
	default:
		break;
	}

	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - onValueUpdated(): %s - end", name);
}

void arc::device::net::ResourceService::onMethodExecute(void* argument)
{
	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - onMethodExecute() - begin");
	if (argument) {
		M2MResource::M2MExecuteParameter* param = (M2MResource::M2MExecuteParameter*)argument;
		String resource_name = param->get_argument_resource_name();
		uint8_t* payload = param->get_argument_value();
		int payload_length = param->get_argument_value_length();

		M2MResource* res = instance->resource(resource_name);
		ResourceType type = (ResourceType)res->resource_instance_type();
		const char* name = resource_name.c_str();
		switch (type)
		{
		case INTEGER:
		{
			Event<void(int)> *ev = (Event<void(int)>*)eventMap[string(name)];
			res->set_value(payload, payload_length);
			ev->post(res->get_value_int());
			break;
		}
		case BOOLEAN:
		{
			Event<void(bool)> *ev = (Event<void(bool)>*)eventMap[string(name)];
			res->set_value(payload, payload_length);
			ev->post((bool)res->get_value_int());
			break;
		}
		default:
			break;
		}
	}

	//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "ResourceService - onMethodExecute() - end");
}
