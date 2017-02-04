#pragma once

namespace arc
{
	namespace device
	{
		namespace core
		{
			class IPanelInterface
			{
			public:
				virtual ~IPanelInterface() {}

				virtual void Start() = 0;
				virtual void Stop() = 0;
			};
		}
	}
}