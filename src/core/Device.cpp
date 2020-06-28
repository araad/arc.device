#include "Device.h"
#include "system/TaskManager.h"
#include "utils/Logger.h"

using namespace arc::device::core;
using namespace arc::device::utils;

TaskManager Tasks;

void heartbeat()
{
    Logger::Log(LogLevel::TRACE, "Heartbeat");
}

arc::device::core::Device::Device()
    : ModuleBase::ModuleBase("dev"),
      initTask("DeviceInit", callback(this, &Device::Initialize), false)
{
    Logger::Log(LogLevel::TRACE, "Device - ctor()");

    initTask.run();
}

void arc::device::core::Device::Initialize()
{
    if (Logger::GetLogLevel() > LogLevel::TRACE)
    {
        Tasks.AddRecurringTask("Heartbeat", callback(&heartbeat), 60000);
    }

    ModuleBase::Initialize();

    initTask.state = TaskState::ASLEEP;
    Tasks.Start();
}
