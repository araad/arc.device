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
				~ResourceService();

				void AddResource(const char* name, char* category, ResourceType type, void* value, void* cb = NULL);
				void AddMethod(const char* name, char* category, ResourceType type, void* cb);
				void updateValue(const char* name, void* value);
			private:
				M2MObject *object;
				M2MObjectInstance *instance;
				EventQueue* queue;
				map<string, void*> cbMap;

				void onValueUpdated(const char* name);
				void onMethodExecute(void* args);
			};
		}
	}
}