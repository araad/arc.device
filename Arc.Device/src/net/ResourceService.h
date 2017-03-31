#pragma once

#include "mbed.h"
#include "mbed-client\m2mobject.h"
#include <map>
#include <vector>

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
					BOOLEAN
				}ResourceType;

				ResourceService(const char* name, uint8_t instance_id);
				~ResourceService();

				void AddResource(const char* name, const char* category, ResourceType type, void* value, void* ev = NULL);
				void AddMethod(const char* name, const char* category, ResourceType type, void* ev);
				void updateValue(const char* name, void* value);
			private:
				const static uint8_t maxObjectCount = 5;
				static M2MObject* objects[maxObjectCount];
				static uint8_t objectCount;

				const char* name;
				uint8_t instance_id;
				M2MObject *object;
				M2MObjectInstance *instance;
				map<string, void*> eventMap;
				vector<const char*> resNames;

				void init();
				void onValueUpdated(const char* name);
				void onMethodExecute(void* args);
			};
		}
	}
}