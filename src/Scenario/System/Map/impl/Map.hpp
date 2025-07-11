#pragma once

#include <Scenario/System/Map/IMap.hpp>
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

		using CellToUnit = std::map<Cells, std::map<Cells, std::unordered_set<EntityId>>>;
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
	std::unordered_set<EntityId> Map<PathFindingPolicy>::getUnitsInArea(const Coord<Cells>& center, Cells range) const
	{
		const auto minCellX = range < center.x() ? center.x() - range : 0;
		const auto maxCellX = range < (_width - center.x()) ? (center.x() + range) : _width;
		const auto minCellY = range < center.y() ? center.y() - range : 0;
		const auto maxCellY = range < (_height - center.y()) ? (center.y() + range) : _height;

		std::unordered_set<EntityId> unitsInArea;

		for (const auto& [cellX, cellYToUnits] :
			 std::ranges::subrange(_cells.lower_bound(minCellX), _cells.lower_bound(maxCellX)))
		{
			for (const auto& [cellY, unitIds] :
				 std::ranges::subrange(cellYToUnits.lower_bound(minCellY), cellYToUnits.lower_bound(maxCellY)))
			{
				std::copy(unitIds.cbegin(), unitIds.cend(), std::inserter(unitsInArea, unitsInArea.begin()));
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
		const auto nextUnitCell = PathFindingPolicy::findNextCell(currentUnitCell, destinationCell, totalCellsToMove);

		moveUnitToCell(unitId, nextUnitCell);

		_eventHandler(io::UnitMoved{unitId, nextUnitCell.x(), nextUnitCell.y()});

		return nextUnitCell;
	}

	template <typename PathFindingPolicy>
	void Map<PathFindingPolicy>::addUnitOnMap(EntityId unitId, const Coord<Cells>& cell)
	{
		_units.emplace(unitId, cell);
		_cells[cell.x()][cell.y()].insert(unitId);
	}

	template <typename PathFindingPolicy>
	void Map<PathFindingPolicy>::removeUnitFromMap(EntityId unitId)
	{
		const auto unitCell = getUnitCell(unitId);
		_cells[unitCell.x()][unitCell.y()].erase(unitId);
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
