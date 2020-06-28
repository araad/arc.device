#include "mbed.h"

int main()
{
	set_time(0);

#ifdef MBED_MAJOR_VERSION
	printf("ARC - main() Mbed OS version %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

	printf("ARC - main() System Clock: %lu RTC: %d\r\n", SystemCoreClock, rtc_isenabled());
}