#include "Logger.h"
#include "FreeMem.h"
#include "platform/mbed_stats.h"
#include <stdarg.h>
#include <stdio.h>

using namespace arc::device::utils;

mbed_stats_heap_t heap_stats;
mbed_stats_stack_t stack_info[MBED_CONF_APP_MAX_THREAD_INFO];
mbed_stats_thread_t thread_stats[MBED_CONF_APP_MAX_THREAD_STATS];

bool Logger::initialized = false;
EventQueue Logger::queue(16 * EVENTS_EVENT_SIZE);
Thread Logger::logThread(osPriorityLow, 1380, nullptr, "log");
Mutex Logger::levelMutex;

void Logger::init()
{
	if (!initialized)
	{
		if (logThread.get_state() == Thread::Deleted && level > OFF)
		{
			printf("\r\n\r\n");
			logThread.start(callback(&queue, &EventQueue::dispatch_forever));
			initialized = true;
		}
	}
}

void Logger::Log(LogLevel level, char *format, ...)
{
	if (initialized && Logger::level >= level)
	{
		LogArgs logArgs = LogArgs{level, ThisThread::get_id()};
		char *logStr = new char[256];
		va_list args;
		va_start(args, format);
		vsnprintf(logStr, 256, format, args);
		va_end(args);
		queue.call(Logger::logToOutput, logArgs, logStr);
	}
}

void Logger::logToOutput(LogArgs logArgs, char *logStr)
{
	osThreadId threadId = logArgs.threadId;
	const char *levelStr = getLogLevelString(logArgs.level);
	time_t seconds = time(NULL);
	char dateTimeStr[20];

	strftime(dateTimeStr, 20, "%d-%m-%Y %H:%M:%S", localtime(&seconds));
	printf("%s %s: [", dateTimeStr, levelStr);

	if (displayThreadStats)
	{
		int count = mbed_stats_thread_get_each(thread_stats, MBED_CONF_APP_MAX_THREAD_STATS);
		for (int i = 0; i < count; i++)
		{
			if (thread_stats[i].id == (uint32_t)threadId)
			{
				printf("T:\"%s\"(0x%08x)", thread_stats[i].name, thread_stats[i].id);
				printf(" S:%lu/%lu", thread_stats[i].stack_space, thread_stats[i].stack_size);
			}
		}
	}

	if (displayMemoryHeap)
	{
		mbed_stats_heap_get(&heap_stats);
		printf(" H:%lu/%lu", heap_stats.current_size, heap_stats.max_size);
	}

	if (displayMemoryStack)
	{
		mbed_stats_stack_get(&stack_info[0]);
		printf(" S(%d):%lu/%lu", stack_info[0].stack_cnt, stack_info[0].max_size, stack_info[0].reserved_size);
	}

	printf(" F:%d]\t%s\r\n", FreeMem(), logStr);
	delete[] logStr;
}

const char *arc::device::utils::Logger::getLogLevelString(LogLevel level)
{
	switch (level)
	{
	case TRACE:
		return "TRACE";
	case INFO:
		return " INFO";
	case WARN:
		return " WARN";
	case ERROR:
		return "ERROR";
	default:
		return "     ";
	}
}
