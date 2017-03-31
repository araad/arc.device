#pragma once

#include "mbed.h"

namespace arc
{
	namespace device
	{
		namespace components
		{
			class RgbLed
			{
			public:
				// Color arrays
				const static uint8_t black[];
				const static uint8_t white[];
				const static uint8_t red[];
				const static uint8_t blue[];

				RgbLed(PinName rPin, PinName gPin, PinName bPin);

				void Initialize();
				void ShowSolid(uint8_t color[]);
				void SetFadeDuration(int fadeDuration);
				void Blink(uint8_t led);
				void FadeOut();

				static void copyColor(const uint8_t src[], uint8_t *dest);
			private:
				/*struct showMixedColors {
					uint8_t color1[3];
					uint8_t color2[3];
					uint8_t color3[3];
					uint8_t color4[3];
				};*/

				PwmOut rPin;
				PwmOut gPin;
				PwmOut bPin;
				uint8_t curVal[3];
				uint8_t prevVal[3];
				int solidFadeDuration;

				EventQueue queue;
				Thread thread;

				void showSolidTask(uint8_t color[], int fadeDuration);

				/*showMixedColors *mixedColors;
				void showMixedTask(int fadeDuration, int showDuration, bool firstPass);*/

				void blinkTask(uint8_t led);

				int calculateStep(int prevValue, int endValue);
				int calculateVal(int step, int val, int idx);
				void crossFade(uint8_t color[], int fadeDuration, int showDuration);
				void setColor(uint8_t curVal[]);
				void resetQueue();
				void doShowSolid(uint8_t color[], int fadeDuration);
			};
		}

	}
}