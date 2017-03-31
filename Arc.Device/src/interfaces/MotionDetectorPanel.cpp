#include "MotionDetectorPanel.h"

arc::device::interfaces::MotionDetectorPanel::MotionDetectorPanel()
{
}

arc::device::interfaces::MotionDetectorPanel::~MotionDetectorPanel()
{
}

void arc::device::interfaces::MotionDetectorPanel::Start()
{
	motionSensor.Initialize();
}

void arc::device::interfaces::MotionDetectorPanel::Stop()
{
}
