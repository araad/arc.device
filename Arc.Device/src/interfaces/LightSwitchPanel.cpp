#include "LightSwitchPanel.h"

arc::device::interfaces::LightSwitchPanel::LightSwitchPanel()
{
}

arc::device::interfaces::LightSwitchPanel::~LightSwitchPanel()
{
}

void arc::device::interfaces::LightSwitchPanel::Start()
{
	lightSwitch.Initialize();
}

void arc::device::interfaces::LightSwitchPanel::Stop()
{
}

void arc::device::interfaces::LightSwitchPanel::setUpdateAuxEv(Event<void(bool)>* ev)
{
	lightSwitch.updateAuxEv = ev;
}

void arc::device::interfaces::LightSwitchPanel::onAuxUpdated(bool state)
{
	lightSwitch.onAuxUpdated(state);
}
