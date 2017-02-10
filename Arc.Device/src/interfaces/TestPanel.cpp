#include "TestPanel.h"
#include "../utils/LogManager.h"

arc::device::interfaces::TestPanel::TestPanel()
	:lightSwitch(D14), btn(USER_BUTTON), service("3311", Tasks->GetQueue())
{
	lightSwitch = 0;

	btn.attach_asserted(this, &TestPanel::onBtnPress_ISR);
	btn.setSampleFrequency();
}

arc::device::interfaces::TestPanel::~TestPanel()
{
	Logger.Trace("TestPanel - dtor()");
}

void arc::device::interfaces::TestPanel::Start()
{
	Logger.Trace("TestPanel started");
	bool light = lightSwitch.read();
	cb = callback(this, &TestPanel::onLightSwitchUpdated);
	service.AddResource("5850", "lights", net::ResourceService::BOOLEAN, &light, &cb);
}

void arc::device::interfaces::TestPanel::Stop()
{
	Logger.Trace("TestPanel stopped");
	Tasks->CancelTask(readBtnTaskId);
}

void arc::device::interfaces::TestPanel::onBtnPress_ISR()
{
	lightSwitch = !lightSwitch;
	Tasks->AddOneTimeTask(callback(this, &TestPanel::onBtnPress));
}

void arc::device::interfaces::TestPanel::onBtnPress()
{
	bool light = lightSwitch.read();
	Logger.Trace("TestPanel - onBtnPress() lightSwitch = %d", light);
	service.updateValue("5850", &light);
}

void arc::device::interfaces::TestPanel::onLightSwitchUpdated(bool value)
{
	Logger.Trace("TestPanel - onLightSwitchUpdated() new value = %d", value);
	lightSwitch = value;
}
