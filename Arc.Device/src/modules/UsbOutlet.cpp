#include "UsbOutlet.h"
#include "../core/TaskManager.h"
#include "../net/ConnectionManager.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::modules::UsbOutlet::UsbOutlet(PinName enablePin)
	: ModuleBase::ModuleBase("3201"),
	enable(enablePin),
	enableUpdatedEv(Tasks.GetQueue(), callback(this, &UsbOutlet::onEnableUpdated))
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "UsbOutlet - ctor()");

	enable = 1;
}

arc::device::modules::UsbOutlet::~UsbOutlet()
{
}

void arc::device::modules::UsbOutlet::Initialize()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "UsbOutlet - Initialize() begin");

	ModuleBase::Initialize();

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "UsbOutlet - Initialize() end");
}

void arc::device::modules::UsbOutlet::registerResources()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "UsbOutlet - registerResources() begin");

	bool state = enable;
	service.AddResource("5550", "plugs", net::ResourceService::BOOLEAN, &state, &enableUpdatedEv);

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "UsbOutlet - registerResources() end");
}

void arc::device::modules::UsbOutlet::onEnableUpdated(bool value)
{
	Logger.queue.call(LogManager::Log, LogManager::InfoArgs(), "UsbOutlet - onEnableUpdated() new value = %d", value);
	enable = value;
}
