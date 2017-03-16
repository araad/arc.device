#include "LogManager.h"
#include <stdarg.h>
#include "rtos\Thread.h"
#include "platform\mbed_stats.h"

mbed_stats_heap_t heap_stats;

using namespace arc::device::utils;

LogManager::LogManager()
	: logPort(STDIO_UART_TX, STDIO_UART_RX, 115200)
{
	level = OFF;
	displayHeap = false;
	displayStack = false;
}

void LogManager::SetLevel(LogLevel level)
{
	levelMutex.lock();
	this->level = level;
	levelMutex.unlock();
}

void arc::device::utils::LogManager::DisplayStackStats(bool val)
{
	displayStack = val;
}

void arc::device::utils::LogManager::DisplayHeapStats(bool val)
{
	displayHeap = val;
}

void LogManager::Error(const char* format, ...)
{
	levelMutex.lock();
	LogLevel level = this->level;
	levelMutex.unlock();

	if (level >= ERROR)
	{
		va_list args;
		va_start(args, format);
		log("ERROR", format, args);
		va_end(args);
	}
}

void LogManager::Warn(const char* format, ...)
{
	levelMutex.lock();
	LogLevel level = this->level;
	levelMutex.unlock();

	if (level >= WARN)
	{
		va_list args;
		va_start(args, format);
		log("WARN", format, args);
		va_end(args);
	}
}

void LogManager::Info(const char* format, ...)
{
	levelMutex.lock();
	LogLevel level = this->level;
	levelMutex.unlock();

	if (level >= INFO)
	{
		va_list args;
		va_start(args, format);
		log("INFO", format, args);
		va_end(args);
	}
}

void LogManager::Trace(const char* format, ...)
{
	levelMutex.lock();
	LogLevel level = this->level;
	levelMutex.unlock();

	if (level >= TRACE)
	{
		va_list args;
		va_start(args, format);
		log("TRACE", format, args);
		va_end(args);
	}
}

void arc::device::utils::LogManager::mapThreadName(char *name)
{
	mapMutex.lock();
	namesMap[Thread::gettid()] = name;
	mapMutex.unlock();
}

void LogManager::log(const char* level, const char* format, va_list args)
{
	logMutex.lock();
	time_t seconds = time(NULL);
	char buffer[20];
	strftime(buffer, 20, "%d-%m-%Y %H:%M:%S", localtime(&seconds));
	logPort.printf("%s ", buffer);

	logPort.printf("%s:\t[T:\"%s\"", level, getThreadName());

	if (displayStack)
	{
		logPort.printf(" S:%lu/%lu", getThreadInfo(osThreadInfoStackMax), getThreadInfo(osThreadInfoStackSize));
	}

	if (displayHeap)
	{
		mbed_stats_heap_get(&heap_stats);
		logPort.printf(" H:%lu,%lu", heap_stats.current_size, heap_stats.max_size);
	}

	logPort.printf("] ");
	logPort.vprintf(format, args);
	logPort.printf("\r\n");
	logMutex.unlock();
}

int LogManager::getThreadInfo(osThreadInfo info)
{
	int val = -1;
	osEvent ev = _osThreadGetInfo(Thread::gettid(), info);
	if (ev.status == osOK)
	{
		val = ev.value.v;
	}
	return val;
}

char* arc::device::utils::LogManager::getThreadName()
{
	char *name;

	mapMutex.lock();
	name = namesMap[Thread::gettid()];
	mapMutex.unlock();

	return name;
}

arc::device::utils::LogManager Logger;