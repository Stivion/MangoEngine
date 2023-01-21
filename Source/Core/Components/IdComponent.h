#pragma once

#include "../GUID.h"

namespace Mango
{
	class IdComponent
	{
	public:
		inline Mango::GUID GetId(){ return _id; }
		void SetId(Mango::GUID id) { _id = id; }

	private:
		Mango::GUID _id;
	};
}
