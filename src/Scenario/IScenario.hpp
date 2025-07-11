#pragma once

#include "IUnit.hpp"
#include "Utils/Common.hpp"
#include "Utils/Coord.hpp"
#include "Utils/Event.hpp"

#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <cstdint>
#include <memory>

namespace sw::sc
{
	using UnitFaсtory = std::function<IUnitPtr(IUnitCreationContext&)>;

	struct IScenario
	{
		using Event = sw::sc::Event<
			io::MapCreated,
			io::UnitSpawned,
			io::UnitDied,
			io::UnitMoved,
			io::UnitAttacked,
			io::MarchStarted,
			io::MarchEnded>;

		using EventHandler = std::function<void(uint64_t tick, const Event& event)>;

		virtual void subscribeEvents(EventHandler handler) = 0;

		virtual void createMap(Width<Cells> width, Height<Cells> height) = 0;
		virtual void spawnUnit(const Coord<Cells>& cell, UnitFaсtory unitFactory) = 0;
		virtual void marchUnit(EntityId unitId, const Coord<Cells>& cell) = 0;

		virtual void run() = 0;

		virtual ~IScenario() = default;
	};

	using IScenarioPtr = std::unique_ptr<IScenario>;

	IScenarioPtr createScenario();
}
