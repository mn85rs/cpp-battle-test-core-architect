#pragma once

#include "Utils/Common.hpp"
#include "Utils/Event.hpp"

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
		virtual struct ICombatSystem& GetCombatSystem() = 0;
		virtual struct IMarchSystem& GetMarchSystem() = 0;
	};

	enum class TurnStatus
	{
		NoActions,
		Acted
	};

	struct IUnit
	{
		using EventHandler = std::function<void(const Event<io::UnitDied>&)>;

		virtual EntityId id() const = 0;
		virtual const char* type() const = 0;
		virtual bool isAlive() const = 0;
		virtual TurnStatus takeTurn() = 0;

		virtual void subscribeEvents(EventHandler handler) = 0;

		virtual ~IUnit() = default;
	};

	using IUnitPtr = std::unique_ptr<IUnit>;

	IUnitPtr createUnit(const sw::io::SpawnSwordsman& command, IUnitCreationContext& context);
	IUnitPtr createUnit(const sw::io::SpawnHunter& command, IUnitCreationContext& context);
}
