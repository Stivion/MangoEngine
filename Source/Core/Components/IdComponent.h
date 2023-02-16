#pragma once

#include "../GUID.h"

namespace Mango
{
	class IdComponent
	{
	public:
		IdComponent() = default;
		IdComponent(Mango::GUID id) { _id = id; };

		inline Mango::GUID GetId(){ return _id; }
		inline const Mango::GUID GetId() const { return _id; }
		void SetId(Mango::GUID id) { _id = id; }

	private:
		Mango::GUID _id;
	};
}
