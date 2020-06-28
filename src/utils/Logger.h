#pragma once

#include "mbed.h"
#include "rtos.h"

using namespace rtos;

namespace arc
{
	namespace device
	{
		namespace utils
		{
			enum LogLevel
			{
				OFF,
				ERROR,
				WARN,
				INFO,
				TRACE,
				DEBUG
			};

			struct LogArgs
			{
				LogLevel level;
				osThreadId threadId;
			};

			class Logger
			{
			public:
				static void Log(LogLevel level, char *format, ...);
				static void init();

			private:
				Logger() {}

				static const LogLevel level = MBED_CONF_APP_LOG_LEVEL;
				static const bool displayMemoryStack = MBED_STACK_STATS_ENABLED;
				static const bool displayMemoryHeap = MBED_HEAP_STATS_ENABLED;
				static const bool displayThreadStats = MBED_THREAD_STATS_ENABLED;

				static bool initialized;
				static Mutex levelMutex;
				static Thread logThread;
				static EventQueue queue;

				static void logToOutput(LogArgs logArgs, char *logStr);
				static const char *getLogLevelString(LogLevel level);
			};
		} // namespace utils
	}	  // namespace device
} // namespace arc