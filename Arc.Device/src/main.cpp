#include "utils\LogManager.h"
#include "core\Device.h"
//#include "core\SystemController.h"
#include "../mbed-os/hal/rtc_api.h"

#include "../mbed-os/features/FEATURE_COMMON_PAL/mbed-trace/mbed-trace/mbed_trace.h"

Thread rtc_thread;

static Mutex MyMutex;
static void my_mutex_wait()
{
	MyMutex.lock();
}
static void my_mutex_release()
{
	MyMutex.unlock();
}

int main() {
	set_time(0);

	mbed_trace_mutex_wait_function_set(my_mutex_wait); // only if thread safety is needed
	mbed_trace_mutex_release_function_set(my_mutex_release);
	//mbed_trace_init();

	Logger.SetLevel(Logger.TRACE);
	Logger.DisplayStackStats(true);
	Logger.DisplayHeapStats(true);
	Logger.mapThreadName("Main");

	int enabled = rtc_isenabled();

	Logger.Trace("ARC - main() System Clock: %d RTC: %d", SystemCoreClock, enabled);
	//arc::device::core::SystemController sysCtrl;

	arc::device::core::Device arcDevice;
}