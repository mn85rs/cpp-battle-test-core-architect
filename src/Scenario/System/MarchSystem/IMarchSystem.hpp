#pragma once

#include <Scenario/Utils/Common.hpp>
#include <Scenario/Utils/EventDispatcher.hpp>
#include <Scenario/Utils/Coord.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/MarchEnded.hpp>

#include <functional>
#include <memory>

namespace sw::sc
{
	enum class MarchStatus
	{
		OnPlace,
		Marching
	};

	struct IMarchSystem
	{
		virtual MarchStatus marchUnit(EntityId unitId, Speed<Cells> speed) = 0;
		virtual void setMarchDestinationForUnit(EntityId unitId, const Coord<Cells>& destination) = 0;

		virtual ~IMarchSystem() = default;
	};

	using IMarchSystemPtr = std::unique_ptr<IMarchSystem>;

	IMarchSystemPtr createMarchSystem(struct IMap& map, IEventsDispatcher& eventsDispatcher);
}
