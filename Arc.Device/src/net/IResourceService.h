#pragma once

namespace arc
{
	namespace device
	{
		namespace core
		{
			class IResourceService
			{
			public:
				virtual ~IResourceService() {}

				virtual void RegisterResource() = 0;
				virtual void RegisterFunction() = 0;
			};
		}
	}
}