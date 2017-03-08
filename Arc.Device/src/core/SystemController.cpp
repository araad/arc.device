#include "SystemController.h"
#include "../utils/LogManager.h"
#include "Task.h"

vector<arc::device::core::TaskBase*> arc::device::core::TaskBase::instances;

arc::device::core::SystemController::SystemController()
	: th(osPriorityRealtime, 1280)
{
	Logger.Trace("SystemController - ctor()");

	initWatchdog();

	th.start(callback(&queue, &EventQueue::dispatch_forever));
}

arc::device::core::SystemController::~SystemController()
{
	queue.break_dispatch();
	th.terminate();
}

void arc::device::core::SystemController::initWatchdog()
{
	bool result = getAndClearWatchdogReset();
	if (result)
	{
		// TODO: read recorded failure message and notify listeners
		Logger.Trace("SystemController - system was reset by watchdog");
	}

	IWDG->KR = 0x5555; //Disable write protection of IWDG registers
	IWDG->KR = 0xAAAA;    //Reload IWDG
	IWDG->KR = 0xCCCC;    //Start IWDG

	queue.call_every(500, callback(this, &SystemController::kickWatchdog));
}

bool arc::device::core::SystemController::getAndClearWatchdogReset()
{
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
	{
		__HAL_RCC_CLEAR_RESET_FLAGS();
		return true;
	}

	return false;
}

void arc::device::core::SystemController::kickWatchdog()
{
	vector<TaskBase*>::iterator it;
	bool flagsOk = true;
	TaskBase* task;
	for (it = TaskBase::instances.begin(); it < TaskBase::instances.end(); it++)
	{
		task = *it;
		if (task->state == TaskState::UNKNOWN)
		{
			flagsOk = false;
			break;
		}
	}

	if (flagsOk)
	{
		IWDG->KR = 0xAAAA;
	}
	else
	{
		if (task)
		{
			// TODO: record the failure message identifiying the failing task
			//IWDG->KR = 0xAAAA;
			Logger.Trace("bad: %s", task->getName());
		}
	}

	//Logger.Trace("%d", TaskBase::instances.size());
	for (it = TaskBase::instances.begin(); it < TaskBase::instances.end(); it++)
	{
		task = *it;
		//Logger.Trace("%s", task->getName());
		if (task->state == TaskState::ALIVE)
		{
			task->state = TaskState::UNKNOWN;
		}
	}
}




