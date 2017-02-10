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
				TaskManager(EventQueue* queue);

				int AddRecurringTask(Callback<void()> cb, int period);
				
				void AddDelayedTask(Callback<void()> cb, int delay);
				void AddDelayedTask(Callback<void(bool)> cb, int delay);
				void AddDelayedTask(Callback<void(int)> cb, int delay);
				void AddDelayedTask(Callback<void(char*)> cb, int delay);

				void AddOneTimeTask(Callback<void()> cb);
				void AddOneTimeTask(Callback<void(bool)> cb, bool val);
				void AddOneTimeTask(Callback<void(int)> cb);
				void AddOneTimeTask(Callback<void(char*)> cb);

				void AddTask();

				void CancelTask(int taskId);
				void Start();
				EventQueue* GetQueue();
			private:
				EventQueue *queue;
				Mutex queueMutex;
			};
		}
	}
}

extern arc::device::core::TaskManager* Tasks;