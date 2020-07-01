#pragma once

#include "mbed.h"
#include "system/Task.h"
#include "ModuleBase.h"
#include "components/Button.h"
#include "components/Led.h"

using namespace arc::device::components;

namespace arc
{
    namespace device
    {
        namespace core
        {
            using namespace system;

            class Device : public ModuleBase
            {
            public:
                Device();

                virtual void Initialize();

            private:
                Button btn;
                Led statusLed;

                Task<Callback<void()>> initTask;

                void onBtnTap(uint8_t tapCount);
				void onBtnHold();
            };
        } // namespace core
    }     // namespace device
} // namespace arc