#include "MotionDetector.h"
#include "../core/TaskManager.h"
#include "../mbed_config_include.h"
#include "../utils/LogManager.h"

using namespace arc::device::utils;

arc::device::modules::MotionDetector::MotionDetector()
	: ModuleBase::ModuleBase("3302"),
	detector(MBED_CONF_APP_MOTION_DETECTOR_STATE),
	detectionDelayUpdatedEv(Tasks.GetQueue(), callback(this, &MotionDetector::onDetectionDelayUpdated))
{
	detector.attach_asserted(this, &MotionDetector::onMotion_ISR);
	detector.attach_deasserted(this, &MotionDetector::onIdle_ISR);
	detector.setSampleFrequency();

	detectionDelay = 10;
}

arc::device::modules::MotionDetector::~MotionDetector()
{
}

void arc::device::modules::MotionDetector::Initialize()
{
	state = detector;

	ModuleBase::Initialize();
}

void arc::device::modules::MotionDetector::registerResources()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "MotionDetector - registerResources() begin");

	service.AddResource("5500", "motion", net::ResourceService::BOOLEAN, &state);

	int delay = (int)detectionDelay;
	service.AddResource("5903", "delay", net::ResourceService::INTEGER, &delay);

	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "MotionDetector - registerResources() end");
}

void arc::device::modules::MotionDetector::onDetectionDelayUpdated(int delay)
{
	if (delay >= 10 && delay <= 255)
	{
		detectionDelay = delay;
	}
	else
	{
		int delay = (int)detectionDelay;
		service.updateValue("5903", &delay);
	}
}

void arc::device::modules::MotionDetector::onMotion_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &MotionDetector::onMotion));
}

void arc::device::modules::MotionDetector::onIdle_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &MotionDetector::onIdle));
}

void arc::device::modules::MotionDetector::onMotion()
{
	if (!state)
	{
		Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "MotionDetector - onMotion() setting state to true");
		state = true;
		service.updateValue("5500", &state);
	}
	else
	{
		Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "MotionDetector - onMotion() state is already true, cancelling timeout");
		idleTimeout.detach();
	}
}

void arc::device::modules::MotionDetector::onIdle()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "MotionDetector - onIdle() starting timeout");
	idleTimeout.attach(callback(this, &MotionDetector::onIdleTimeout_ISR), detectionDelay);
}

void arc::device::modules::MotionDetector::onIdleTimeout_ISR()
{
	Tasks.AddOneTimeTask(callback(this, &MotionDetector::onIdleTimeout));
}

void arc::device::modules::MotionDetector::onIdleTimeout()
{
	Logger.queue.call(LogManager::Log, LogManager::TraceArgs(), "MotionDetector - onIdleTimeout()");
	state = false;
	service.updateValue("5500", &state);
}
