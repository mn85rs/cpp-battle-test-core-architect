#pragma once

#include <Scenario/System/Map/IMap.hpp>
#include <Scenario/System/MarchSystem/IMarchSystem.hpp>
#include <unordered_map>

namespace sw::sc
{
	class MarchSystem : public IMarchSystem
	{
	public:
		MarchSystem(IMap& _map);

		MarchStatus marchUnit(EntityId unitId, Speed<Cells> speed) override;

		void setMarchDestinationForUnit(EntityId unitId, const Coord<Cells>& destination) override;

		void subscribeEvents(EventHandler eventHandler) override;

	private:
		using MarchingUnits = std::unordered_map<EntityId, Coord<Cells>>;

	private:
		IMap& _map;
		MarchingUnits _marchingUnits;
		EventHandler _eventHandler;
	};
}
