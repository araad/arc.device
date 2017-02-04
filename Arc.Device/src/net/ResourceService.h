#pragma once

#include "mbed.h"
#include "mbed-client\m2mobject.h"
#include <map>
#include <string>

namespace arc
{
	namespace device
	{
		namespace net
		{
			class ResourceService
			{
			public:
				typedef enum
				{
					STRING,
					INTEGER,
					FLOAT,
					BOOLEAN,
					OPAQUE,
					TIME,
					OBJLINK
				}ResourceType;

				ResourceService(char* name, EventQueue* queue);

				void AddResource(const char* name, char* category, ResourceType type, void* value, void* cb = NULL);
				void updateValue(const char* name, ResourceType type, void* value);
			private:
				M2MObject *object;
				M2MObjectInstance *instance;
				EventQueue* queue;
				map<string, void*> cbMap;

				void onValueUpdated(const char* name);
			};
		}
	}
}