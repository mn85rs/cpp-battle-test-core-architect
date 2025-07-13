#include "EventDispatcher.hpp"

namespace sw::sc
{
	IEventsDispatcherPtr createEventDispatcher()
	{
		return std::make_unique<ed::EventsDispatcher<
			io::MapCreated,
			io::UnitSpawned,
			io::UnitDied,
			io::UnitMoved,
			io::UnitAttacked,
			io::MarchStarted,
			io::MarchEnded>>();
	}
}
