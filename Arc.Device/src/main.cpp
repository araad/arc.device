#include "utils/LogManager.h"
#include "core/Device.h"
#include "core/SystemController.h"
#include "../mbed-os/hal/rtc_api.h"

#include "mbed_config_include.h";

using namespace arc::device::utils;

int main() {
	set_time(0);

	Logger.SetLevel(Logger.OFF);
	Logger.DisplayStackStats(true);
	Logger.DisplayHeapStats(true);
	Logger.mapThreadName("Main");

	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "ARC - main() System Clock: %d RTC: %d", SystemCoreClock, rtc_isenabled());

	arc::device::core::SystemController sysCtrl;
	arc::device::core::Device arcDevice;
}