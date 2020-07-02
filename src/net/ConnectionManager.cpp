#include "ConnectionManager.h"
#include "NTPClient.h"
#include "utils/Logger.h"
#include "core/system/TaskManager.h"

using namespace arc::device::net;
using namespace arc::device::core;
using namespace arc::device::core::system;
using namespace arc::device::components;
using namespace arc::device::utils;

ConnectionManager::ConnectionManager()
    : connectionThread(osPriorityNormal, 256 * 30, nullptr, "connect")
{
    Logger::Log(LogLevel::TRACE, "ConnectionManager - ctor()");

    networkInterface = NULL;
}

void ConnectionManager::StartConnection()
{
    Logger::Log(LogLevel::TRACE, "ConnectionManager - StartConnection()");

    statusLed->blink();

    connectionTask = new Task<Callback<void()>>("Connection", callback(this, &ConnectionManager::connect), false);
    connectionThread.start(callback(&TaskBase::taskStarter<Task<Callback<void()>>>, connectionTask));
}

void ConnectionManager::SetStatusLed(components::Led *statusLed)
{
    this->statusLed = statusLed;
}

void arc::device::net::ConnectionManager::connect()
{
    Logger::Log(LogLevel::TRACE, "ConnectionManager - connect() - begin");

    connectionTask->state = TaskState::ASLEEP;
    networkInterface = NetworkInterface::get_default_instance();
    connectionTask->state = TaskState::ALIVE;

    if (!networkInterface)
    {
        Logger::Log(LogLevel::ERROR, "ConnectionManager - connect() - No network interface found");
        ThisThread::sleep_for(UINT32_MAX);
        return;
    }

    connectionTask->state = TaskState::ASLEEP;
    credMutex.lock();
    connectionTask->state = TaskState::ALIVE;
    nsapi_error_t connError;

#if defined(MBED_CONF_NSAPI_DEFAULT_WIFI_SSID) && defined(MBED_CONF_NSAPI_DEFAULT_WIFI_PASSWORD)
    ssid = MBED_CONF_NSAPI_DEFAULT_WIFI_SSID;
    pswd = MBED_CONF_NSAPI_DEFAULT_WIFI_PASSWORD;
#endif

    if (!ssid.empty() && !pswd.empty())
    {
        Logger::Log(LogLevel::INFO, "ConnectionManager - connect(): Connecting to Network \"%s\"", ssid.c_str());

        connectionTask->state = TaskState::ASLEEP;
        connError = networkInterface->connect();
        connectionTask->state = TaskState::ALIVE;
    }

    credMutex.unlock();

    if (connError == 0)
    {
        Logger::Log(LogLevel::INFO, "ConnectionManager - connect(): Connected");
        statusLed->pulse();

        NTPClient ntp(networkInterface);
        ntp.set_server(MBED_CONF_APP_NTP_SERVER, MBED_CONF_APP_NTP_PORT);
        time_t now = ntp.get_timestamp();
        set_time(now);
    }
    else
    {
        Logger::Log(LogLevel::ERROR, "ConnectionManager - connect(): Connection to Network Failed %d", connError);
        statusLed->blink(true);
    }

    Logger::Log(LogLevel::TRACE, "ConnectionManager - connect() - end");

    thread_sleep_for(50);

    delete connectionTask;
}
