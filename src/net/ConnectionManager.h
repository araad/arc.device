#pragma once

#include "mbed.h"
#include "core/system/Task.h"
#include "components/Led.h"

using namespace arc::device::core::system;

namespace arc
{
    namespace device
    {
        namespace net
        {
            class ConnectionManager
            {
            public:
                ConnectionManager();

                void StartConnection();
                void StartDiscovery();
                void SetStatusLed(components::Led *statusLed);
                void AddClientRegisteredEventHandler(Callback<void()> *cb);

            private:
                NetworkInterface *networkInterface;
                components::Led *statusLed;

                Thread connectionThread;
				Task<Callback<void()>>* connectionTask;

                string ssid;
                string pswd;
                Mutex credMutex;

                void connect();
            };
        } // namespace net
    }     // namespace device
} // namespace arc

extern arc::device::net::ConnectionManager Connection;