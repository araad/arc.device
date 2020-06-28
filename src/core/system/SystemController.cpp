#include "SystemController.h"
#include "utils/Logger.h"
#include "Task.h"

using namespace arc::device::utils;
using namespace arc::device::core::system;

vector<TaskBase*> TaskBase::instances;

SystemController::SystemController()
	: th(osPriorityRealtime, 640)
{
	Logger::Log(LogLevel::TRACE, "SystemController - ctor()");
	initWatchdog();
}

SystemController::~SystemController()
{
	th.terminate();
}

void SystemController::initWatchdog()
{
	bool result = getAndClearWatchdogReset();
	if (result)
	{
		// TODO: read recorded failure message and notify listeners
		Logger::Log(LogLevel::WARN, "SystemController - system was reset by watchdog");
	}

	IWDG->KR = 0x5555; //Disable write protection of IWDG registers
	IWDG->KR = 0xAAAA;    //Reload IWDG
	IWDG->KR = 0xCCCC;    //Start IWDG

	th.start(callback(this, &SystemController::kickWatchdogThreadStarter));
}

void SystemController::kickWatchdogThreadStarter()
{
	while (1)
	{
		thread_sleep_for(500);
		kickWatchdog();
	}
}

bool SystemController::getAndClearWatchdogReset()
{
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
	{
		__HAL_RCC_CLEAR_RESET_FLAGS();
		return true;
	}

	return false;
}

void SystemController::kickWatchdog()
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
			Logger::Log(LogLevel::WARN, "SystemController - kickWatchdog() task failed: %s", task->getName());
		}
	}

	for (it = TaskBase::instances.begin(); it < TaskBase::instances.end(); it++)
	{
		task = *it;
		if (task->state == TaskState::ALIVE)
		{
			task->state = TaskState::UNKNOWN;
		}
	}
}



