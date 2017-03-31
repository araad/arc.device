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

				/*void Error(const char *format, ...);
				void Warn(const char *format, ...);
				void Info(const char *format, ...);
				void Trace(const char *format, ...);*/
				void mapThreadName(char *name);
				//void StartMbedTrace();

				EventQueue queue;

				struct LogArgs
				{
					LogLevel level;
					osThreadId threadId;
				};

				static LogArgs ErrorArgs();
				static LogArgs WarnArgs();
				static LogArgs InfoArgs();
				static LogArgs TraceArgs();
				static LogArgs createLogArgs(LogLevel level);
				static void Log(LogArgs args, const char* format, ...);
			private:
				LogLevel level;
				Serial logPort;
				//Mutex logMutex;
				//Mutex levelMutex;
				map<osThreadId, char *> namesMap;
				Mutex mapMutex;
				bool displayStack;
				bool displayHeap;
				Thread logThread;

				void log(const char* level, osThreadId threadId, const char *format, va_list args);
				int getThreadInfo(osThreadId threadId, osThreadInfo info);
				char* getThreadName(osThreadId threadId);
			};
		}
	}
}

extern arc::device::utils::LogManager Logger;