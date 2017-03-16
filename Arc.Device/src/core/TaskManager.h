#pragma once

#include "mbed.h"

using namespace events;

namespace arc
{
	namespace device
	{
		namespace core
		{
			class TaskManager
			{
			public:
				TaskManager();

				int AddRecurringTask(const char* name, Callback<void()> cb, int period);

				void AddDelayedTask(Callback<void()> cb, int delay);
				void AddDelayedTask(Callback<void(bool)> cb, bool val, int delay);
				void AddDelayedTask(Callback<void(int)> cb, int delay);
				void AddDelayedTask(Callback<void(char*)> cb, int delay);

				void AddOneTimeTask(Callback<void()> cb);
				void AddOneTimeTask(Callback<void(bool)> cb, bool val);
				void AddOneTimeTask(Callback<void(int)> cb);
				void AddOneTimeTask(Callback<void(char*)> cb, char* val);
				void AddOneTimeTask(Callback<void(char*, char*)> cb, char* val1, char* val2);

				void AddTask();

				void CancelTask(int taskId);
				void Start();
				EventQueue* GetQueue();
			private:
				EventQueue queue;
			};
		}
	}
}

extern arc::device::core::TaskManager Tasks;