#include "LogManager.h"
#include <stdarg.h>
#include "rtos\Thread.h"

using namespace arc::device::utils;

LogManager::LogManager()
{
	level = OFF;
}

void LogManager::SetLevel(LogLevel level)
{
	this->level = level;
}

void LogManager::Error(const char* format, ...)
{
	logMutex.lock();
	if (this->level >= ERROR)
	{
		va_list args;
		va_start(args, format);
		time_t seconds = time(NULL);
		char buffer[32];
		strftime(buffer, 32, "%d-%m-%Y %H:%M:%S", localtime(&seconds));
		printf("%s ", buffer);
		printf("ERROR: [%s (%d/%d)] ", getThreadName(), getThreadInfo(osThreadInfoStackMax), getThreadInfo(osThreadInfoStackSize));
		vprintf(format, args);
		printf("\r\n");
		va_end(args);
	}
	logMutex.unlock();
}

void LogManager::Warn(const char* format, ...)
{
	logMutex.lock();
	if (this->level >= WARN)
	{
		va_list args;
		va_start(args, format);
		time_t seconds = time(NULL);
		char buffer[32];
		strftime(buffer, 32, "%d-%m-%Y %H:%M:%S", localtime(&seconds));
		printf("%s ", buffer);
		printf("WARN: [%s (%d/%d)] ", getThreadName(), getThreadInfo(osThreadInfoStackMax), getThreadInfo(osThreadInfoStackSize));
		vprintf(format, args);
		printf("\r\n");
		va_end(args);
	}
	logMutex.unlock();
}

void LogManager::Info(const char* format, ...)
{
	logMutex.lock();
	if (this->level >= INFO)
	{
		va_list args;
		va_start(args, format);
		time_t seconds = time(NULL);
		char buffer[32];
		strftime(buffer, 32, "%d-%m-%Y %H:%M:%S", localtime(&seconds));
		printf("%s ", buffer);
		printf("INFO: [%s (%d/%d)] ", getThreadName(), getThreadInfo(osThreadInfoStackMax), getThreadInfo(osThreadInfoStackSize));
		vprintf(format, args);
		printf("\r\n");
		va_end(args);
	}
	logMutex.unlock();
}

void LogManager::Trace(const char* format, ...)
{
	logMutex.lock();
	if (this->level >= TRACE)
	{
		va_list args;
		va_start(args, format);
		time_t seconds = time(NULL);
		char buffer[32];
		strftime(buffer, 32, "%d-%m-%Y %H:%M:%S", localtime(&seconds));
		printf("%s ", buffer);
		printf("TRACE: [%s (%d/%d)] ", getThreadName(), getThreadInfo(osThreadInfoStackMax), getThreadInfo(osThreadInfoStackSize));
		vprintf(format, args);
		printf("\r\n");
		va_end(args);
	}
	logMutex.unlock();
}

void arc::device::utils::LogManager::mapThreadName(char *name)
{
	mapMutex.lock();
	namesMap[Thread::gettid()] = name;
	mapMutex.unlock();
}

void LogManager::log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
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