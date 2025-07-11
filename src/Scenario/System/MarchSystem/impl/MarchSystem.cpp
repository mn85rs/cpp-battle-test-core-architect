#include "MarchSystem.hpp"

namespace sw::sc
{

	MarchSystem::MarchSystem(IMap& _map) :
			_map(_map)
	{}

	MarchStatus MarchSystem::marchUnit(EntityId unitId, Speed<Cells> speed)
	{
		if (!_marchingUnits.contains(unitId))
		{
			return MarchStatus::OnPlace;
		}

		const auto destinationCell = _marchingUnits.at(unitId);
		const auto newUnitCell = _map.moveUnitToDestinationCell(unitId, destinationCell, speed);

		if (newUnitCell == destinationCell)
		{
			_marchingUnits.erase(unitId);

			if (_eventHandler)
			{
				_eventHandler(io::MarchEnded{unitId, destinationCell.x(), destinationCell.y()});
			}
		}

		return MarchStatus::Marching;
	}

	void MarchSystem::setMarchDestinationForUnit(EntityId unitId, const Coord<Cells>& destination)
	{
		_marchingUnits.insert_or_assign(unitId, destination);
		const auto currentCell = _map.getUnitCell(unitId);
		_eventHandler(io::MarchStarted{unitId, currentCell.x(), currentCell.y(), destination.x(), destination.y()});
	}

	void MarchSystem::subscribeEvents(EventHandler eventHandler)
	{
		_eventHandler = std::move(eventHandler);
	}

	IMarchSystemPtr createMarchSystem(IMap& map)
	{
		return std::make_unique<MarchSystem>(map);
	}
}
