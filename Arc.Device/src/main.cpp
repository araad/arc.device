#include "utils\LogManager.h"
#include "core\Device.h"

int main() {
	set_time(0);
	Logger.mapThreadName("Main");
	Logger.SetLevel(Logger.TRACE);

	arc::device::core::Device arcDevice;
}