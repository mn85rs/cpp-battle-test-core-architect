#pragma once

#include <Scenario/Utils/Common.hpp>
#include <Scenario/Utils/Event.hpp>
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
		using EventHandler = std::function<void(const Event<io::MarchStarted, io::MarchEnded>&)>;

		virtual MarchStatus marchUnit(EntityId unitId, Speed<Cells> speed) = 0;
		virtual void setMarchDestinationForUnit(EntityId unitId, const Coord<Cells>& destination) = 0;

		virtual void subscribeEvents(EventHandler handler) = 0;

		virtual ~IMarchSystem() = default;
	};

	using IMarchSystemPtr = std::unique_ptr<IMarchSystem>;

	IMarchSystemPtr createMarchSystem(struct IMap& map);
}
