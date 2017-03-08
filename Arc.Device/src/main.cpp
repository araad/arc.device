#include "utils\LogManager.h"
#include "core\Device.h"
#include "core\SystemController.h"

int main() {
	set_time(0);
	Logger.mapThreadName("Main");
	Logger.SetLevel(Logger.TRACE);
	arc::device::core::SystemController sysCtrl;

	arc::device::core::Device arcDevice;
}