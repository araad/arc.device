#include "SystemController.h"
#include "../utils/LogManager.h"
#include "Task.h"

using namespace arc::device::utils;

vector<arc::device::core::TaskBase*> arc::device::core::TaskBase::instances;

arc::device::core::SystemController::SystemController()
	: th(osPriorityRealtime, 640)
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "SystemController - ctor()");
	initWatchdog();
}

arc::device::core::SystemController::~SystemController()
{
	th.terminate();
}

void arc::device::core::SystemController::initWatchdog()
{
	bool result = getAndClearWatchdogReset();
	if (result)
	{
		// TODO: read recorded failure message and notify listeners
		Logger.queue.call(LogManager::Log, LogManager::WarnArgs(), "SystemController - system was reset by watchdog");
	}

	IWDG->KR = 0x5555; //Disable write protection of IWDG registers
	IWDG->KR = 0xAAAA;    //Reload IWDG
	IWDG->KR = 0xCCCC;    //Start IWDG

	th.start(callback(this, &SystemController::kickWatchdogThreadStarter));
}

void arc::device::core::SystemController::kickWatchdogThreadStarter()
{
	Logger.mapThreadName("SysCtrl");
	while (1)
	{
		wait_ms(500);
		kickWatchdog();
	}
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
			Logger.queue.call(LogManager::Log, LogManager::WarnArgs(), "SystemController - kickWatchdog() task failed: %s", task->getName());
		}
	}

	//Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "SystemController - kickWatchdog() %d", TaskBase::instances.size());
	for (it = TaskBase::instances.begin(); it < TaskBase::instances.end(); it++)
	{
		task = *it;
		//Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "SystemController - kickWatchdog() %s", task->getName());
		if (task->state == TaskState::ALIVE)
		{
			task->state = TaskState::UNKNOWN;
		}
	}
}




