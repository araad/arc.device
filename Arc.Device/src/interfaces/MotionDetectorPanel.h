#pragma once

#include "mbed.h"
#include "../core/IPanelInterface.h"
#include "../modules/MotionDetector.h"
#include "../net/ConnectionManager.h"

namespace arc
{
	namespace device
	{
		using namespace core;
		using namespace modules;

		namespace interfaces
		{
			class MotionDetectorPanel : public IPanelInterface
			{
			public:
				MotionDetectorPanel();
				~MotionDetectorPanel();

				virtual void Start();
				virtual void Stop();
			private:
				MotionDetector motionSensor;
			};
		}
	}
}