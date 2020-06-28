#include "mbed.h"
#include "utils/Logger.h"

using arc::device::utils::Logger;
using arc::device::utils::LogLevel;

int main()
{
	set_time(0);
	Logger::init();

#ifdef MBED_MAJOR_VERSION
	Logger::Log(LogLevel::TRACE, "ARC main() - Mbed OS version %d.%d.%d", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

	Logger::Log(LogLevel::INFO, "ARC main() - Arc.Device v6.0");
	Logger::Log(LogLevel::DEBUG, "ARC main() - System Clock: %d RTC: %d", SystemCoreClock, rtc_isenabled());

	thread_sleep_for(UINT32_MAX);
}