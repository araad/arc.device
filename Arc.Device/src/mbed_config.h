#pragma once

#ifdef MBED_CONFIG_ARC_STM32F401RE_RELEASE
#include "../Release/ARC_STM32F401RE/mbed_config.h"
#elif MBED_CONFIG_ARC_STM32F401RE_DEBUG
#include "../Debug/ARC_STM32F401RE/mbed_config.h"
#elif MBED_CONFIG_NUCLEO_F401RE_RELEASE
#include "../Release/NUCLEO_F401RE/mbed_config.h"
#elif MBED_CONFIG_NUCLEO_F401RE_DEBUG
#include "../Debug/NUCLEO_F401RE/mbed_config.h"
#endif