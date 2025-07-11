#pragma once

#include "Attack.hpp"

#include <IO/Events/UnitAttacked.hpp>
#include <Scenario/Utils/Common.hpp>
#include <Scenario/Utils/Event.hpp>
#include <functional>
#include <memory>
#include <unordered_set>

namespace sw::sc
{
	struct ICombatSystem
	{
		using TargetStatusHandler = std::function<void(HitPoints)>;
		using TakeDamageHandler = std::function<void(AttackType, Damage<HitPoints>, TargetStatusHandler)>;
		using AttackTypes = std::unordered_set<AttackType>;

		using EventHandler = std::function<void(const Event<io::UnitAttacked>&)>;

		virtual AttackResult attackByUnit(EntityId attackerUnitId, const Attack& attack) = 0;
		virtual void registerUnitAsTarget(
			EntityId targetUnitId, AttackTypes types, TakeDamageHandler takeDamageHandler) = 0;
		virtual void unRegisterUnitAsTarget(EntityId targetUnitId, AttackTypes types) = 0;

		virtual void subscribeEvents(EventHandler handler) = 0;

		virtual ~ICombatSystem() = default;
	};

	using ICombatSystemPtr = std::unique_ptr<ICombatSystem>;

	ICombatSystemPtr createCombatSystem(const struct IMap& map);
}
