#pragma once

#include <IO/Events/UnitMoved.hpp>
#include <Scenario/Utils/Common.hpp>
#include <Scenario/Utils/Coord.hpp>
#include <Scenario/Utils/Event.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_set>

namespace sw::sc
{
	struct IMap
	{
		using EventHandler = std::function<void(const Event<io::UnitMoved>&)>;

		virtual Coord<Cells> getUnitCell(EntityId unitId) const = 0;
		virtual std::unordered_set<EntityId> getUnitsInArea(const Coord<Cells>& center, Cells range) const = 0;
		virtual std::optional<EntityId> getUnitInCell(const Coord<Cells>& cell) const = 0;
		virtual Width<Cells> width() const = 0;
		virtual Height<Cells> height() const = 0;
		virtual Coord<Cells> moveUnitToDestinationCell(
			EntityId unitId, const Coord<Cells>& destinationCell, Cells totalCellsToMove) = 0;
		virtual void addUnitOnMap(EntityId unitId, const Coord<Cells>& cell) = 0;
		virtual void removeUnitFromMap(EntityId unitId) = 0;

		virtual void subscribeEvents(EventHandler handler) = 0;

		virtual ~IMap() = default;
	};

	using IMapPtr = std::unique_ptr<IMap>;

	IMapPtr createMap(Width<Cells> width, Height<Cells> height);
}
