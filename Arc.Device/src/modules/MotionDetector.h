#pragma once

#include "mbed.h"
#include "../core/ModuleBase.h"
#include "../components/Button.h"
#include "../components/RgbLed.h"
#include "PinDetect.h"

namespace arc
{
	namespace device
	{
		using namespace components;
		using namespace core;
		using namespace net;

		namespace modules
		{
			class MotionDetector : public ModuleBase
			{
			public:
				MotionDetector();
				~MotionDetector();

				virtual void Initialize();
			private:
				PinDetect detector;
				bool state;
				uint8_t detectionDelay;
				Timeout idleTimeout;

				virtual void registerResources();

				Event<void(int)> detectionDelayUpdatedEv;
				void onDetectionDelayUpdated(int delay);

				void onMotion_ISR();
				void onMotion();
				void onIdle_ISR();
				void onIdle();
				void onIdleTimeout_ISR();
				void onIdleTimeout();
			};
		}
	}
}