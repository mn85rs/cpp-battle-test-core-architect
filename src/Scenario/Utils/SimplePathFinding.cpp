#include "SimplePathFinding.hpp"

#include <Scenario/System/Map/IMap.hpp>
#include <ranges>

namespace sw::sc
{
	namespace
	{
		int32_t getOffset(Cells sourceCell, Cells destCell)
		{
			if (sourceCell < destCell)
			{
				return 1;
			}

			if (sourceCell > destCell)
			{
				return -1;
			}

			return 0;
		}

		Coord<Cells> offsetCoord(const Coord<Cells>& cell, int32_t offsetX, int32_t offsetY)
		{
			return {cell.x() + offsetX, cell.y() + offsetY};
		}
	}

	Coord<Cells> SimplePathFinding::findNextCell(
		const struct IMap& map,
		const Coord<Cells>& currentCell,
		const Coord<Cells>& destinationCell,
		Speed<Cells> speed)
	{
		if (currentCell == destinationCell || speed == 0)
		{
			return currentCell;
		}

		Coord<Cells> cell = currentCell;

		for (const auto _ : std::views::iota(0u, speed))
		{
			const auto nextCell
				= offsetCoord(cell, getOffset(cell.x(), destinationCell.x()), getOffset(cell.y(), destinationCell.y()));

			if (map.getUnitInCell(nextCell))
			{
				break;
			}

			cell = nextCell;

			if (cell == destinationCell)
			{
				break;
			}
		}

		return cell;
	}
}
