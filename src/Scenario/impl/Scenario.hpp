#pragma once

#include <Scenario/IScenario.hpp>
#include <Scenario/System/CombatSystem/ICombatSystem.hpp>
#include <Scenario/System/Map/IMap.hpp>
#include <Scenario/System/MarchSystem/IMarchSystem.hpp>

namespace sw::sc
{
	class Scenario : public IScenario, private IUnitCreationContext
	{
	public:
		struct Factories
		{
			std::function<IMapPtr(Width<Cells>, Height<Cells>)> createMap;
			std::function<ICombatSystemPtr(const IMap&)> createCombatSystem;
			std::function<IMarchSystemPtr(IMap&)> createMarchSystem;
		};

	public:
		Scenario(Factories factories);

		void subscribeEvents(EventHandler eventHandler) override;

		void createMap(Width<Cells> width, Height<Cells> height) override;
		void spawnUnit(const Coord<Cells>& cell, UnitFaсtory unitFactory) override;
		void marchUnit(EntityId unitId, const Coord<Cells>& cell) override;

		void run() override;

	private:
		ICombatSystem& GetCombatSystem() override;
		IMarchSystem& GetMarchSystem() override;

		template <typename... EventTypes>
		void publishEvent(const sw::sc::Event<EventTypes...>& event);

	private:
		Factories _factories;

		IMapPtr _map;
		ICombatSystemPtr _combatSystem;
		IMarchSystemPtr _marchSystem;

		using Units = std::vector<EntityId>;
		using UnitIdToUnit = std::unordered_map<EntityId, IUnitPtr>;

		Units _units;
		UnitIdToUnit _unitIdToUnit;

		uint64_t _tick;

		EventHandler _eventHandler;
	};
}
