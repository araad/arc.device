#include "TaskManager.h"
#include "utils/Logger.h"

using namespace arc::device::utils;
using namespace arc::device::core::system;

TaskManager::TaskManager()
    : queue(40 * EVENTS_EVENT_SIZE)
{
}

int TaskManager::AddRecurringTask(const char *name, Callback<void()> cb, int period)
{
    Task<Callback<void()>> *task = new Task<Callback<void()>>(name, cb, false);
    int taskId = queue.call_every(period, callback(&TaskBase::taskStarter<Task<Callback<void()>>>, task));
    tasks[taskId] = task;
    return taskId;
}

void TaskManager::AddDelayedTask(Callback<void()> cb, int delay)
{
    queue.call_in(delay, cb);
}

void TaskManager::AddDelayedTask(Callback<void(bool)> cb, bool val, int delay)
{
    queue.call_in(delay, cb, val);
}

void TaskManager::AddOneTimeTask(Callback<void()> cb)
{
    queue.call(cb);
}

void TaskManager::AddOneTimeTask(Callback<void(bool)> cb, bool val)
{
    queue.call(cb, val);
}

void TaskManager::AddOneTimeTask(Callback<void(char *)> cb, char *val)
{
    queue.call(cb, val);
}

void TaskManager::AddOneTimeTask(Callback<void(char *, char *)> cb, char *val1, char *val2)
{
    queue.call(cb, val1, val2);
}

void TaskManager::AddTask()
{
}

void TaskManager::CancelTask(int taskId)
{
    queue.cancel(taskId);
    TaskBase *task = tasks[taskId];
    if (task)
    {
        delete task;
    }
    tasks.erase(taskId);
}

void TaskManager::Start()
{
    Logger::Log(LogLevel::TRACE, "TaskManager - Starting tasks");
    queue.dispatch_forever();
}

EventQueue *TaskManager::GetQueue()
{
    return &queue;
}