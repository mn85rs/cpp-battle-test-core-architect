#pragma once

#include <Scenario/System/Map/IMap.hpp>
#include <format>
#include <map>
#include <ranges>

namespace sw::sc
{
	template <typename PathFindingPolicy>
	class Map : public IMap
	{
	public:
		Map(Width<Cells> width, Height<Cells> height);

		Coord<Cells> getUnitCell(EntityId unitId) const override;
		std::unordered_set<EntityId> getUnitsInArea(const Coord<Cells>& center, Cells range) const override;
		std::optional<EntityId> getUnitInCell(const Coord<Cells>& cell) const override;
		Width<Cells> width() const override;
		Height<Cells> height() const override;
		Coord<Cells> moveUnitToDestinationCell(
			EntityId unitId, const Coord<Cells>& destinationCell, Cells totalCellsToMove) override;
		void addUnitOnMap(EntityId unitId, const Coord<Cells>& cell) override;
		void removeUnitFromMap(EntityId unitId) override;
		void subscribeEvents(EventHandler eventHandler) override;

	private:
		void moveUnitToCell(EntityId unitId, const Coord<Cells>& cell);

	private:
		Width<Cells> _width;
		Height<Cells> _height;

		using CellToUnit = std::map<Cells, std::map<Cells, EntityId>>;
		using UnitToCell = std::unordered_map<EntityId, Coord<Cells>>;

		UnitToCell _units;
		CellToUnit _cells;

		EventHandler _eventHandler;
	};

	template <typename PathFindingPolicy>
	Map<PathFindingPolicy>::Map(Width<Cells> width, Height<Cells> height) :
			_width(width),
			_height(height)
	{}

	template <typename PathFindingPolicy>
	Coord<Cells> Map<PathFindingPolicy>::getUnitCell(EntityId unitId) const
	{
		return _units.at(unitId);
	}

	template <typename PathFindingPolicy>
	std::optional<EntityId> Map<PathFindingPolicy>::getUnitInCell(const Coord<Cells>& cell) const
	{
		if (const auto itX = _cells.find(cell.x()); itX != _cells.end())
		{
			auto& [cellX, cellYToUnit] = *itX;

			if (auto itY = cellYToUnit.find(cell.y()); itY != cellYToUnit.end())
			{
				return itY->second;
			}
		}

		return std::nullopt;
	}

	template <typename PathFindingPolicy>
	std::unordered_set<EntityId> Map<PathFindingPolicy>::getUnitsInArea(const Coord<Cells>& center, Cells range) const
	{
		const auto minCellX = range < center.x() ? center.x() - range : 0;
		const auto maxCellX = range < (_width - center.x()) ? (center.x() + range) : _width;
		const auto minCellY = range < center.y() ? center.y() - range : 0;
		const auto maxCellY = range < (_height - center.y()) ? (center.y() + range) : _height;

		std::unordered_set<EntityId> unitsInArea;

		for (const auto& [cellX, cellYToUnit] :
			 std::ranges::subrange(_cells.lower_bound(minCellX), _cells.lower_bound(maxCellX)))
		{
			for (const auto& [cellY, unitId] :
				 std::ranges::subrange(cellYToUnit.lower_bound(minCellY), cellYToUnit.lower_bound(maxCellY)))
			{
				unitsInArea.insert(unitId);
			}
		}

		return unitsInArea;
	}

	template <typename PathFindingPolicy>
	Width<Cells> Map<PathFindingPolicy>::width() const
	{
		return _width;
	}

	template <typename PathFindingPolicy>
	Height<Cells> Map<PathFindingPolicy>::height() const
	{
		return _height;
	}

	template <typename PathFindingPolicy>
	Coord<Cells> Map<PathFindingPolicy>::moveUnitToDestinationCell(
		EntityId unitId, const Coord<Cells>& destinationCell, Cells totalCellsToMove)
	{
		const auto currentUnitCell = getUnitCell(unitId);
		const auto nextUnitCell
			= PathFindingPolicy::findNextCell(*this, currentUnitCell, destinationCell, totalCellsToMove);

		moveUnitToCell(unitId, nextUnitCell);

		_eventHandler(io::UnitMoved{unitId, nextUnitCell.x(), nextUnitCell.y()});

		return nextUnitCell;
	}

	template <typename PathFindingPolicy>
	void Map<PathFindingPolicy>::addUnitOnMap(EntityId unitId, const Coord<Cells>& cell)
	{
		const auto unitInCell = getUnitInCell(cell);

		if (unitInCell)
		{
			throw std::runtime_error(std::format(
				"Error: Can't move unit id = {} to cell [{}, {}] because it's occupied by another unit with id = {}",
				unitId,
				cell.x(),
				cell.y(),
				*unitInCell));
		}

		_units.emplace(unitId, cell);
		_cells[cell.x()][cell.y()] = unitId;
	}

	template <typename PathFindingPolicy>
	void Map<PathFindingPolicy>::removeUnitFromMap(EntityId unitId)
	{
		const auto unitCell = getUnitCell(unitId);

		const auto it = _cells.find(unitCell.x());
		auto& [cellX, cellYToUnit] = *it;

		cellYToUnit.erase(unitCell.y());

		if (cellYToUnit.empty())
		{
			_cells.erase(it);
		}

		_units.erase(unitId);
	}

	template <typename PathFindingPolicy>
	void Map<PathFindingPolicy>::subscribeEvents(EventHandler eventHandler)
	{
		_eventHandler = std::move(eventHandler);
	}

	template <typename PathFindingPolicy>
	void Map<PathFindingPolicy>::moveUnitToCell(EntityId unitId, const Coord<Cells>& cell)
	{
		removeUnitFromMap(unitId);
		addUnitOnMap(unitId, cell);
	}
}
