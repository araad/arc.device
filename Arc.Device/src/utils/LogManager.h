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

				void Error(const char *format, ...);
				void Warn(const char *format, ...);
				void Info(const char *format, ...);
				void Trace(const char *format, ...);
				void mapThreadName(char *name);

			private:
				LogLevel level;
				Mutex logMutex;
				map<osThreadId, char *> namesMap;
				Mutex mapMutex;

				void log(const char *format, ...);
				int getThreadInfo(osThreadInfo info);
				char* getThreadName();
			};
		}
	}
}

extern arc::device::utils::LogManager Logger;