#include "Map.hpp"

#include <Scenario/Utils/SimplePathFinding.hpp>

namespace sw::sc
{
	IMapPtr createMap(Width<Cells> width, Height<Cells> height, IEventsDispatcher& eventsDispatcher)
	{
		return std::make_unique<Map<SimplePathFinding>>(width, height, eventsDispatcher);
	}
}
