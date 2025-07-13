#include <IO/Events/EventDispatcher.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>

#include <memory>

namespace sw::sc
{
	using IEventsDispatcher = ed::IEventsDispatcher<
		io::MapCreated,
		io::UnitSpawned,
		io::UnitDied,
		io::UnitMoved,
		io::UnitAttacked,
		io::MarchStarted,
		io::MarchEnded>;

	using IEventsDispatcherPtr = std::unique_ptr<IEventsDispatcher>;

	IEventsDispatcherPtr createEventDispatcher();
}
