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
	queueMutex.lock();
	queue->call(cb);
	queueMutex.unlock();
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(bool)> cb, bool val)
{
	queueMutex.lock();
	queue->call(cb, val);
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
	queueMutex.lock();
	queue->dispatch_forever();
	queueMutex.unlock();
}

EventQueue* arc::device::core::TaskManager::GetQueue()
{
	return queue;
}
