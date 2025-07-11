#pragma once

#include "Common.hpp"
#include "Coord.hpp"

namespace sw::sc
{
	struct SimplePathFinding
	{
		static Coord<Cells> findNextCell(
			const Coord<Cells>& currentCell, const Coord<Cells>& destinationCell, Speed<Cells> speed);
	};
}
