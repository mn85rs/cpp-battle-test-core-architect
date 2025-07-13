#pragma once

#include "Utils/Common.hpp"
#include "Utils/Event.hpp"
#include "Utils/EventDispatcher.hpp"

#include <IO/Events/UnitDied.hpp>
#include <functional>
#include <memory>

namespace sw::io
{
	struct SpawnSwordsman;
	struct SpawnHunter;
}

namespace sw::sc
{
	struct IUnitCreationContext
	{
		virtual struct ICombatSystem& getCombatSystem() = 0;
		virtual struct IMarchSystem& getMarchSystem() = 0;
		virtual IEventsDispatcher& getEventsDispatcher() = 0;
	};

	enum class TurnStatus
	{
		NoActions,
		Acted
	};

	struct IUnit
	{
		virtual EntityId id() const = 0;
		virtual const char* type() const = 0;
		virtual bool isAlive() const = 0;
		virtual TurnStatus takeTurn() = 0;

		virtual ~IUnit() = default;
	};

	using IUnitPtr = std::unique_ptr<IUnit>;

	IUnitPtr createUnit(const sw::io::SpawnSwordsman& command, IUnitCreationContext& context);
	IUnitPtr createUnit(const sw::io::SpawnHunter& command, IUnitCreationContext& context);
}
