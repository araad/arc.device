#pragma once
#include "mbed.h"

namespace arc
{
    namespace device
    {
        namespace core
        {
            class ModuleBase
            {
            public:
                virtual ~ModuleBase() {}

                virtual void Initialize() {
                    // TODO: Add initialization logic...
                }

            protected:
                ModuleBase(const char *resName, uint8_t instance_id = 0) {}
            };
        } // namespace core
    }     // namespace device
} // namespace arc