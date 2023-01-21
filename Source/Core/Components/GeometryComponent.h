#pragma once

#include "../GeometryType.h"

namespace Mango
{
	class GeometryComponent
	{
	public:
		GeometryComponent(Mango::GeometryType geometry) { _geometry = geometry; }
		inline Mango::GeometryType GetGeometry() { return _geometry; }
		
	private:
		Mango::GeometryType _geometry;
	};
}
