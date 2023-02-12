#pragma once

namespace Mango
{
	class RigidbodyComponent
	{
	public:
		RigidbodyComponent() {}

		inline bool IsDynamic() { return _isDynamic; }

		void SetDynamic(bool isDynamic) { _isDynamic = isDynamic; }

	private:
		bool _isDynamic = false;
	};
}
