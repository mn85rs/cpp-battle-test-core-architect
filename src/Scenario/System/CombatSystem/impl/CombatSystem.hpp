#pragma once

#include <Scenario/System/CombatSystem/ICombatSystem.hpp>
#include <Scenario/Utils/Coord.hpp>
#include <unordered_set>

namespace sw::sc
{
	struct IMap;

	class CombatSystem : public ICombatSystem
	{
	public:
		using Units = std::unordered_set<EntityId>;

	public:
		CombatSystem(const IMap& map);

		AttackResult attackByUnit(EntityId attackerUnitId, const Attack& attack) override;
		void registerUnitAsTarget(
			EntityId targetUnitId, AttackTypes attackTypes, TakeDamageHandler takeDamageHandler) override;
		void unRegisterUnitAsTarget(EntityId targetUnitId, AttackTypes attackTypes) override;

		void subscribeEvents(EventHandler eventHandler) override;

	private:
		using TargetDamageHandlers = std::unordered_map<EntityId, TakeDamageHandler>;
		using AttackTypeToTargets = std::unordered_map<AttackType, Units>;
		using AttackArea = std::unordered_set<Coord<Cells>>;

	private:
		void attackTarget(
			EntityId attackerUnitId,
			EntityId targetUnitId,
			const TakeDamageHandler& takeDamageHandlers,
			AttackType attackType,
			Damage<HitPoints> damage) const;
		Units getUnitsInAttackArea(
			EntityId attackerUnitId, AttackRange<Cells> minRange, AttackRange<Cells> maxRange) const;

	private:
		const IMap& _map;
		AttackTypeToTargets _targets;
		TargetDamageHandlers _takeDamageHandlers;
		EventHandler _eventHandler;
	};
}
