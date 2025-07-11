#include "Map.hpp"

#include <Scenario/Utils/SimplePathFinding.hpp>

namespace sw::sc
{
	IMapPtr createMap(Width<Cells> width, Height<Cells> height)
	{
		return std::make_unique<Map<SimplePathFinding>>(width, height);
	}
}
