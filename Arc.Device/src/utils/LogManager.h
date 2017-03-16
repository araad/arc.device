#pragma once

#include "mbed.h"
#include "rtos\Mutex.h"
#include "rtos\rtx\TARGET_CORTEX_M\cmsis_os.h"
#include <map>

using namespace rtos;

namespace arc
{
	namespace device
	{
		namespace utils
		{
			class LogManager
			{
			public:
				enum LogLevel { OFF, ERROR, WARN, INFO, TRACE };

				LogManager();
				void SetLevel(LogLevel level);
				void DisplayStackStats(bool val);
				void DisplayHeapStats(bool val);

				void Error(const char *format, ...);
				void Warn(const char *format, ...);
				void Info(const char *format, ...);
				void Trace(const char *format, ...);
				void mapThreadName(char *name);

			private:
				LogLevel level;
				Serial logPort;
				Mutex logMutex;
				Mutex levelMutex;
				map<osThreadId, char *> namesMap;
				Mutex mapMutex;
				bool displayStack;
				bool displayHeap;

				void log(const char* level, const char *format, va_list args);
				int getThreadInfo(osThreadInfo info);
				char* getThreadName();
			};
		}
	}
}

extern arc::device::utils::LogManager Logger;