#include "mbed.h"
#include "utils/Logger.h"
#include "core/system/SystemController.h"
#include "core/Device.h"

using namespace arc::device::utils;
using namespace arc::device::core;

int main()
{
	set_time(0);
	Logger::Initialize();

#ifdef MBED_MAJOR_VERSION
	Logger::Log(LogLevel::DEBUG, "ARC main() - Mbed OS version %d.%d.%d", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

	Logger::Log(LogLevel::INFO, "ARC main() - Arc.Device v6.0");
	Logger::Log(LogLevel::DEBUG, "ARC main() - System Clock: %d RTC: %d", SystemCoreClock, rtc_isenabled());

	system::SystemController sysCtrl;
	Device device;
}