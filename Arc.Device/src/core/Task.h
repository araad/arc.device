#pragma once

#include "mbed.h"
#include <vector>
#include <atomic>
#include <algorithm>

using namespace std;

namespace arc
{
	namespace device
	{
		namespace core
		{
			enum class TaskState
			{
				ALIVE,
				ASLEEP,
				UNKNOWN
			};

			class TaskBase
			{
			public:
				static vector<TaskBase*> instances;

				template <typename T>
				static void taskStarter(T* task)
				{
					task->run();
				}

				std::atomic<TaskState> state;

				const char* getName()
				{
					return name;
				}
			protected:
				const char* name;
				bool periodic;
			};

			template <class T>
			class Task : public TaskBase
			{
			public:
				Task(const char* name, T cb, bool periodic = true)
				{
					this->name = name;
					this->state = TaskState::ASLEEP;
					this->cb = cb;
					this->periodic = periodic;
					Task::instances.push_back(this);
				}

				~Task()
				{
					Task::instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
				}

				void run()
				{
					state = TaskState::ALIVE;
					cb.call();
					if (!periodic)
					{
						state = TaskState::ASLEEP;
					}
				}

				template <typename A0>
				void run(A0 a0)
				{
					state = TaskState::ALIVE;
					cb.call(a0);
					if (!periodic)
					{
						state = TaskState::ASLEEP;
					}
				}
			private:
				T cb;
			};
		}
	}
}