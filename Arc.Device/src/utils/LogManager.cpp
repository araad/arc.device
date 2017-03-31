#include "LogManager.h"
#include <stdarg.h>
#include "rtos\Thread.h"
#include "platform\mbed_stats.h"
#include "../mbed_config_include.h"
#include <stdio.h>
#include <stdlib.h>

//#include "../mbed-os/features/FEATURE_COMMON_PAL/mbed-trace/mbed-trace/mbed_trace.h"
//static Mutex* mbedTraceMutex;
//static Serial* mbedTracePort;

using namespace arc::device::utils;

#define FREEMEM_CELL 100

//static void my_mutex_wait()
//{
//	mbedTraceMutex->lock();
//}
//static void my_mutex_release()
//{
//	mbedTraceMutex->unlock();
//}

//static void logMbedTrace(const char* format, ...)
//{
//	va_list args;
//	va_start(args, format);
//	mbedTracePort->vprintf(format, args);
//	va_end(args);
//}

mbed_stats_heap_t heap_stats;

using namespace arc::device::utils;

LogManager::LogManager()
	: logPort(STDIO_UART_TX, NC, 115200),
	queue(16 * EVENTS_EVENT_SIZE),
	logThread(osPriorityLow, 1380)
{
	level = OFF;
	displayHeap = false;
	displayStack = false;
}

void LogManager::SetLevel(LogLevel level)
{
	//levelMutex.lock();
	this->level = level;

	if (this->level > OFF)
	{
		logPort.printf("\r\n\r\n");
		logThread.start(callback(&queue, &EventQueue::dispatch_forever));
	}
	//levelMutex.unlock();
}

void arc::device::utils::LogManager::DisplayStackStats(bool val)
{
	displayStack = val;
}

void arc::device::utils::LogManager::DisplayHeapStats(bool val)
{
	displayHeap = val;
}

//void LogManager::Error(const char* format, ...)
//{
//	//levelMutex.lock();
//	//LogLevel level = this->level;
//	//levelMutex.unlock();
//
//	//if (level >= ERROR)
//	//{
//	//	va_list args;
//	//	va_start(args, format);
//	//	queueCall("ERROR", Thread::gettid(), format, args);
//	//	va_end(args);
//	//}
//}
//
//void LogManager::Warn(const char* format, ...)
//{
//	//levelMutex.lock();
//	//LogLevel level = this->level;
//	//levelMutex.unlock();
//
//	//if (level >= WARN)
//	//{
//	//	va_list args;
//	//	va_start(args, format);
//	//	queueCall("WARN", Thread::gettid(), format, args);
//	//	va_end(args);
//	//}
//}
//
//void LogManager::Info(const char* format, ...)
//{
//	//levelMutex.lock();
//	//LogLevel level = this->level;
//	//levelMutex.unlock();
//
//	//if (level >= INFO)
//	//{
//	//	va_list args;
//	//	va_start(args, format);
//	//	queueCall("INFO", Thread::gettid(), format, args);
//	//	va_end(args);
//	//}
//}
//
//void LogManager::Trace(const char* format, ...)
//{
//	//levelMutex.lock();
//	//LogLevel level = this->level;
//	//levelMutex.unlock();
//
//	//if (level >= TRACE)
//	//{
//	//	va_list args;
//	//	va_start(args, format);
//	//	queueCall("TRACE", Thread::gettid(), format, args);
//	//	va_end(args);
//	//}
//}

void arc::device::utils::LogManager::mapThreadName(char *name)
{
	mapMutex.lock();
	namesMap[Thread::gettid()] = name;
	mapMutex.unlock();
}

//void arc::device::utils::LogManager::StartMbedTrace()
//{
//	mbedTracePort = &logPort;
//	mbedTraceMutex = &logMutex;
//
//	mbed_trace_mutex_wait_function_set(my_mutex_wait); // only if thread safety is needed
//	mbed_trace_mutex_release_function_set(my_mutex_release);
//	mbed_trace_print_function_set(logMbedTrace);
//	mbed_trace_init();
//}

int LogManager::getThreadInfo(osThreadId threadId, osThreadInfo info)
{
	int val = -1;
	osEvent ev = _osThreadGetInfo(threadId, info);
	if (ev.status == osOK)
	{
		val = ev.value.v;
	}
	return val;
}

char* arc::device::utils::LogManager::getThreadName(osThreadId threadId)
{
	char *name;

	mapMutex.lock();
	name = namesMap[threadId];
	mapMutex.unlock();

	return name;
}

LogManager::LogArgs arc::device::utils::LogManager::ErrorArgs()
{
	return createLogArgs(ERROR);
}

LogManager::LogArgs arc::device::utils::LogManager::WarnArgs()
{
	return createLogArgs(WARN);
}

LogManager::LogArgs arc::device::utils::LogManager::InfoArgs()
{
	return createLogArgs(INFO);
}

LogManager::LogArgs arc::device::utils::LogManager::TraceArgs()
{
	return createLogArgs(TRACE);
}

LogManager::LogArgs arc::device::utils::LogManager::createLogArgs(LogLevel level)
{
	return LogArgs{ level, Thread::gettid()};
}

void arc::device::utils::LogManager::Log(LogArgs logArgs, const char* format, ...)
{
	if (Logger.level >= logArgs.level)
	{
		const char* level_str;
		if (logArgs.level == TRACE)
		{
			level_str = "TRACE";
		}
		else if (logArgs.level == INFO)
		{
			level_str = "INFO";
		}
		else if (logArgs.level == WARN)
		{
			level_str = "WARN";
		}
		else if (logArgs.level == ERROR)
		{
			level_str = "ERROR";
		}

		va_list args;
		va_start(args, format);
		Logger.log(level_str, logArgs.threadId, format, args);
		va_end(args);
	}
}

struct elem { /* Definition of a structure that is FREEMEM_CELL bytes  in size.) */
	struct elem *next;
	char dummy[FREEMEM_CELL - 2];
};
int FreeMem(void) {
	int counter;
	struct elem *head, *current, *nextone;
	current = head = (struct elem*) malloc(sizeof(struct elem));
	if (head == NULL)
		return 0;      /*No memory available.*/
	counter = 0;
	// __disable_irq();
	do {
		counter++;
		current->next = (struct elem*) malloc(sizeof(struct elem));
		current = current->next;
	} while (current != NULL);
	/* Now counter holds the number of type elem
	structures we were able to allocate. We
	must free them all before returning. */
	current = head;
	do {
		nextone = current->next;
		free(current);
		current = nextone;
	} while (nextone != NULL);
	// __enable_irq();

	return counter*FREEMEM_CELL;
}

void LogManager::log(const char* level, osThreadId threadId, const char* format, va_list args)
{
	//logMutex.lock();
	time_t seconds = time(NULL);
	char buffer[20];
	strftime(buffer, 20, "%d-%m-%Y %H:%M:%S", localtime(&seconds));
	logPort.printf("%s", buffer);

	logPort.printf(" %s:\t[T:\"%s\"(0x%08x)", level, getThreadName(threadId), threadId);

	if (displayStack)
	{
		logPort.printf(" S:%lu/%lu", getThreadInfo(threadId, osThreadInfoStackMax), getThreadInfo(threadId, osThreadInfoStackSize));
	}

	if (displayHeap)
	{
		mbed_stats_heap_get(&heap_stats);
		logPort.printf(" H:%lu,%lu", heap_stats.current_size, heap_stats.max_size);
	}

	logPort.printf(" F:%d", FreeMem());

	logPort.printf("] ");
	logPort.vprintf(format, args);
	logPort.printf("\r\n");
	//logMutex.unlock();
}

arc::device::utils::LogManager Logger;