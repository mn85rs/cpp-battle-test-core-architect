#pragma once

#include "IUnit.hpp"
#include "Utils/Common.hpp"
#include "Utils/Coord.hpp"
#include "Utils/Event.hpp"
#include "Utils/EventDispatcher.hpp"

#include <cstdint>
#include <memory>

namespace sw::sc
{
	using UnitFaсtory = std::function<IUnitPtr(IUnitCreationContext&)>;

	struct IScenario
	{
		virtual void createMap(Width<Cells> width, Height<Cells> height) = 0;
		virtual void spawnUnit(const Coord<Cells>& cell, UnitFaсtory unitFactory) = 0;
		virtual void marchUnit(EntityId unitId, const Coord<Cells>& cell) = 0;

		virtual void run() = 0;

		virtual uint64_t getTick() const = 0;

		virtual ~IScenario() = default;
	};

	using IScenarioPtr = std::unique_ptr<IScenario>;

	IScenarioPtr createScenario(IEventsDispatcher& eventDispather);
}
