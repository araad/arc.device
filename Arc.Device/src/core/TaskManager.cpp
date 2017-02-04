#include "TaskManager.h"
#include "../utils/LogManager.h"

arc::device::core::TaskManager::TaskManager(EventQueue * queue)
{
	this->queue = queue;
}

void arc::device::core::TaskManager::AddRecurringTask(Callback<void()> cb, int period)
{
	Logger.Info("Queueing recurring task");
	queue->call_every(period, cb);
}

void arc::device::core::TaskManager::AddDelayedTask(Callback<void()> cb, int delay)
{
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void()> cb)
{
	queue->call(cb);
}

void arc::device::core::TaskManager::AddOneTimeTask(Callback<void(bool)> cb, bool val)
{
	queue->call(cb, val);
}

void arc::device::core::TaskManager::AddTask()
{
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
