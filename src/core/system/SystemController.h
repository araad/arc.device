#pragma once

#include "mbed.h"

namespace arc
{
    namespace device
    {
        namespace core
        {
            namespace system
            {
                class SystemController
                {
                public:
                    SystemController();
                    ~SystemController();

                private:
                    Thread th;

                    void initWatchdog();
                    bool getAndClearWatchdogReset();
                    void kickWatchdogThreadStarter();
                    void kickWatchdog();
                };
            } // namespace system
        }     // namespace core
    }         // namespace device
} // namespace arc