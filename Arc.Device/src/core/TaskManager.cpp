#include "TaskManager.h"
#include "../utils/LogManager.h"
#include "Task.h"

arc::device::core::TaskManager::TaskManager() {}

int arc::device::core::TaskManager::AddRecurringTask(const char* name, Callback<void()> cb, int period)
{
	Task<Callback<void()>>* task = new Task<Callback<void()>>(name, cb, false);
	return queue.call_every(period, callback(&core::TaskBase::taskStarter<Task<Callback<void()>>>, task));
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
}

void arc::device::core::TaskManager::Start()
{
	Logger.Trace("TaskManager - Starting tasks");
	queue.dispatch_forever();
}

EventQueue* arc::device::core::TaskManager::GetQueue()
{
	return &queue;
}
