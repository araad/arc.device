#include "mbed.h"
#include "utils/Logger.h"
#include "core/system/TaskManager.h"
#include "core/system/SystemController.h"

using namespace arc::device::utils;
using namespace arc::device::core::system;

TaskManager Tasks;

void heartbeat()
{
	Logger::Log(LogLevel::TRACE, "Heartbeat");
}

int main()
{
	set_time(0);
	Logger::Initialize();

#ifdef MBED_MAJOR_VERSION
	Logger::Log(LogLevel::TRACE, "ARC main() - Mbed OS version %d.%d.%d", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

	Logger::Log(LogLevel::INFO, "ARC main() - Arc.Device v6.0");
	Logger::Log(LogLevel::DEBUG, "ARC main() - System Clock: %d RTC: %d", SystemCoreClock, rtc_isenabled());

	SystemController sysCtrl;

	if (Logger::GetLogLevel() >= LogLevel::TRACE)
	{
		Tasks.AddRecurringTask("Heartbeat", callback(&heartbeat), 60000);
	}

	Tasks.Start();
}