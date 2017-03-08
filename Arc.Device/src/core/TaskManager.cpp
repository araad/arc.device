#include "TaskManager.h"
#include "../utils/LogManager.h"
#include "Task.h"

arc::device::core::TaskManager::TaskManager(EventQueue * queue)
{
	this->queue = queue;
}

int arc::device::core::TaskManager::AddRecurringTask(const char* name, Callback<void()> cb, int period)
{
	Logger.Info("Queueing recurring task");
	int id = 0;
	queueMutex.lock();
	Task<Callback<void()>>* task = new Task<Callback<void()>>(name, cb, false);
	id = queue->call_every(period, callback(&core::TaskBase::taskStarter<Task<Callback<void()>>>, task));
	queueMutex.unlock();
	return id;
}

void arc::device::core::TaskManager::AddDelayedTask(Callback<void()> cb, int delay)
{
	queue->call_in(delay, cb);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void()> cb)
{
	queue->call(cb);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(bool)> cb, bool val)
{
	queue->call(cb, val);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(char*)> cb, char* val)
{
	queue->call(cb, val);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(char*, char*)> cb, char * val1, char * val2)
{
	queue->call(cb, val1, val2);
}

void arc::device::core::TaskManager::AddTask()
{

}

void arc::device::core::TaskManager::CancelTask(int taskId)
{
	Logger.Info("Cancelling task (%d)", taskId);
	queueMutex.lock();
	queue->cancel(taskId);
	queueMutex.unlock();
}

void arc::device::core::TaskManager::Start()
{
	Logger.Trace("TaskManager - Starting tasks");
	queue->dispatch_forever();
}

EventQueue* arc::device::core::TaskManager::GetQueue()
{
	return queue;
}
