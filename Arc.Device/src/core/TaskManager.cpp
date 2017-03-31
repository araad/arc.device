#include "TaskManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::core::TaskManager::TaskManager()
	: queue(40 * EVENTS_EVENT_SIZE)
{}

int arc::device::core::TaskManager::AddRecurringTask(const char* name, Callback<void()> cb, int period)
{
	Task<Callback<void()>>* task = new Task<Callback<void()>>(name, cb, false);
	int taskId = queue.call_every(period, callback(&core::TaskBase::taskStarter<Task<Callback<void()>>>, task));
	tasks[taskId] = task;
	return taskId;
}

void arc::device::core::TaskManager::AddDelayedTask(Callback<void()> cb, int delay)
{
	queue.call_in(delay, cb);
}

void arc::device::core::TaskManager::AddDelayedTask(Callback<void(bool)> cb, bool val, int delay)
{
	queue.call_in(delay, cb, val);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void()> cb)
{
	queue.call(cb);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(bool)> cb, bool val)
{
	queue.call(cb, val);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(char*)> cb, char* val)
{
	queue.call(cb, val);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(char*, char*)> cb, char * val1, char * val2)
{
	queue.call(cb, val1, val2);
}

void arc::device::core::TaskManager::AddTask()
{

}

void arc::device::core::TaskManager::CancelTask(int taskId)
{
	queue.cancel(taskId);
	TaskBase* task = tasks[taskId];
	if (task)
	{
		delete task;
	}
	tasks.erase(taskId);
}

void arc::device::core::TaskManager::Start()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "TaskManager - Starting tasks");
	queue.dispatch_forever();
}

EventQueue* arc::device::core::TaskManager::GetQueue()
{
	return &queue;
}
