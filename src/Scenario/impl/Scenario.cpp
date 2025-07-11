#include "Scenario.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>

namespace sw::sc
{
	Scenario::Scenario(Factories factories) :
			_factories(std::move(factories)),
			_tick(1)
	{}

	void Scenario::subscribeEvents(EventHandler eventHandler)
	{
		_eventHandler = eventHandler;
	}

	void Scenario::createMap(Width<Cells> width, Height<Cells> height)
	{
		_map = _factories.createMap(width, height);
		_combatSystem = _factories.createCombatSystem(*_map);
		_marchSystem = _factories.createMarchSystem(*_map);

		_map->subscribeEvents([this](const auto& event) { publishEvent(event); });
		_combatSystem->subscribeEvents([this](const auto& event) { publishEvent(event); });
		_marchSystem->subscribeEvents([this](const auto& event) { publishEvent(event); });

		publishEvent(Event(io::MapCreated{width, height}));
	}

	void Scenario::spawnUnit(const Coord<Cells>& cell, UnitFaсtory unitFactory)
	{
		if (!_map)
		{
			throw std::runtime_error("Error: Can't spawn a unit. The map is not created yet");
		}

		if (cell.x() > _map->width() || cell.y() > _map->height())
		{
			throw std::runtime_error("Error: Unit can't be spawned outside the map");
		}

		IUnitPtr unit = unitFactory(*this);

		if (_unitIdToUnit.contains(unit->id()))
		{
			throw std::runtime_error(std::format("Error: Unit with id = {} is already spawned", unit->id()));
		}

		unit->subscribeEvents(
			[this](const auto& event)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(event)>, io::UnitDied>)
				{
					_map->removeUnitFromMap(event.unitId);
				}

				publishEvent(event);
			});

		const auto unitId = unit->id();
		const std::string unitType = unit->type();

		_map->addUnitOnMap(unitId, cell);

		_units.emplace_back(unitId);
		_unitIdToUnit.emplace(unitId, std::move(unit));

		publishEvent(Event(io::UnitSpawned{unitId, unitType, cell.x(), cell.y()}));
	}

	void Scenario::marchUnit(EntityId unitId, const Coord<Cells>& cell)
	{
		if (!_marchSystem)
		{
			throw std::runtime_error("Error: Can't set march destination without the map");
		}

		if (cell.x() > _map->width() || cell.y() > _map->height())
		{
			throw std::runtime_error("Error: Unit can't march outside the map");
		}

		if (!_unitIdToUnit.contains(unitId))
		{
			throw std::runtime_error(
				std::format("Error: Can't set march destination for non spawned unit with id = {}", unitId));
		}

		_marchSystem->setMarchDestinationForUnit(unitId, cell);
	}

	void Scenario::run()
	{
		while (true)
		{
			++_tick;

			bool anyUnitActed = false;

			for (const auto unitId : _units)
			{
				const IUnitPtr& unit = _unitIdToUnit.at(unitId);

				if (unit->isAlive())
				{
					if (TurnStatus::Acted == unit->takeTurn())
					{
						anyUnitActed = true;
					}
				}
			}

			if (!anyUnitActed)
			{
				return;
			}

			const auto totalAliveUnits = std::count_if(
				_unitIdToUnit.cbegin(), _unitIdToUnit.cend(), [](const auto& p) { return p.second->isAlive(); });

			if (totalAliveUnits <= 1)
			{
				return;
			}
		}
	}

	ICombatSystem& Scenario::GetCombatSystem()
	{
		return *_combatSystem;
	}

	IMarchSystem& Scenario::GetMarchSystem()
	{
		return *_marchSystem;
	}

	template <typename... EventTypes>
	void Scenario::publishEvent(const sw::sc::Event<EventTypes...>& event)
	{
		std::visit(
			[this](const auto& concreteEvent)
			{
				if (_eventHandler)
				{
					_eventHandler(_tick, concreteEvent);
				}
			},
			event);
	}

	IScenarioPtr createScenario()
	{
		return std::make_unique<Scenario>(Scenario::Factories{createMap, createCombatSystem, createMarchSystem});
	}
}
