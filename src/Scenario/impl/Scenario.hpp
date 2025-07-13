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
			std::function<IMapPtr(Width<Cells>, Height<Cells>, sc::IEventsDispatcher&)> createMap;
			std::function<ICombatSystemPtr(const IMap&, sc::IEventsDispatcher&)> createCombatSystem;
			std::function<IMarchSystemPtr(IMap&, sc::IEventsDispatcher&)> createMarchSystem;
		};

	public:
		Scenario(Factories factories, sc::IEventsDispatcher& eventsDispatcher);

		void createMap(Width<Cells> width, Height<Cells> height) override;
		void spawnUnit(const Coord<Cells>& cell, UnitFaсtory unitFactory) override;
		void marchUnit(EntityId unitId, const Coord<Cells>& cell) override;

		void run() override;

		uint64_t getTick() const override { return _tick; }

	private:
		ICombatSystem& getCombatSystem() override;
		IMarchSystem& getMarchSystem() override;
		IEventsDispatcher& getEventsDispatcher() override;

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

		sc::IEventsDispatcher& _eventsDispatcher;
	};
}
