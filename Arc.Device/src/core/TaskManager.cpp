#include "TaskManager.h"
#include "../utils/LogManager.h"

arc::device::core::TaskManager::TaskManager(EventQueue * queue)
{
	this->queue = queue;
}

int arc::device::core::TaskManager::AddRecurringTask(Callback<void()> cb, int period)
{
	Logger.Info("Queueing recurring task");
	int id = 0;
	queueMutex.lock();
	id = queue->call_every(period, cb);
	queueMutex.unlock();
	return id;
}

void arc::device::core::TaskManager::AddDelayedTask(Callback<void()> cb, int delay)
{
	queueMutex.lock();
	queue->call_in(delay, cb);
	queueMutex.unlock();
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void()> cb)
{
	Logger.Trace("TaskManager - AddOneTimeTask() begin");
	queueMutex.lock();
	Logger.Trace("TaskManager - AddOneTimeTask() calling task");
	queue->call(cb);
	Logger.Trace("TaskManager - AddOneTimeTask() task was called");
	queueMutex.unlock();
	Logger.Trace("TaskManager - AddOneTimeTask() end");
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(bool)> cb, bool val)
{
	queueMutex.lock();
	queue->call(cb, val);
	queueMutex.unlock();
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(char*)> cb, char* val)
{
	queueMutex.lock();
	queue->call(cb, val);
	queueMutex.unlock();
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(char*, char*)> cb, char * val1, char * val2)
{
	Logger.Trace("TaskManager - AddOneTimeTask() val1: %s val2: %s", val1, val2);
	queueMutex.lock();
	queue->call(cb, val1, val2);
	queueMutex.unlock();
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
