#include "Device.h"
#include "system/TaskManager.h"
#include "utils/Logger.h"

using namespace arc::device::core;
using namespace arc::device::utils;

TaskManager Tasks;

extern "C" void __NVIC_SystemReset();

void reset()
{
    __NVIC_SystemReset();
}

void heartbeat()
{
    Logger::Log(LogLevel::TRACE, "Heartbeat");
}

Device::Device()
    : ModuleBase::ModuleBase("dev"),
      btn(MBED_CONF_APP_DEVICE_BUTTON, MBED_CONF_APP_DEVICE_BUTTON_MODE),
      statusLed(MBED_CONF_APP_DEVICE_STATUS_LED),
      initTask("DeviceInit", callback(this, &Device::Initialize), false)
{
    Logger::Log(LogLevel::TRACE, "Device - ctor()");

    initTask.run();
}

void Device::Initialize()
{
    if (Logger::GetLogLevel() > LogLevel::TRACE)
    {
        Tasks.AddRecurringTask("Heartbeat", callback(&heartbeat), 60000);
    }

    btn.Initialize();
    btn.addTapHandler(callback(this, &Device::onBtnTap));
    btn.addSingleHoldHandler(callback(this, &Device::onBtnHold));

    ModuleBase::Initialize();

    initTask.state = TaskState::ASLEEP;
    Tasks.Start();
}

void Device::onBtnTap(uint8_t tapCount)
{
    if (tapCount == 1)
    {
        reset();
    }
    else if (tapCount >= 2)
    {
        Logger::Log(LogLevel::TRACE, "Button tap - count (%d)", tapCount);
        if (tapCount == 2)
        {
            statusLed.blink();
        }
        else if (tapCount == 3)
        {
            statusLed.pulse();
        }
        else if (tapCount == 4)
        {
            statusLed.solid();
        }
        else if (tapCount == 5)
        {
            statusLed.off();
        }
    }
}

void Device::onBtnHold()
{
    Logger::Log(LogLevel::TRACE, "Button hold");
}