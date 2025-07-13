#pragma once

#include "Unit.hpp"

#include <Scenario/System/CombatSystem/ICombatSystem.hpp>
#include <Scenario/System/MarchSystem/IMarchSystem.hpp>
#include <Scenario/Utils/Abilities.hpp>
#include <Scenario/Utils/Common.hpp>

namespace sw::sc
{
	class Trooper : public Unit
	{
	public:
		Trooper(
			EntityId id,
			HitPoints maxHP,
			ICombatSystem& combatSystem,
			IMarchSystem& marchSystem,
			IEventsDispatcher& eventDispatcher);

		bool isAlive() const override;

	protected:
		void takeDamage(
			AttackType attackType, Damage<HitPoints> damage, ICombatSystem::TargetStatusHandler targetStatusHandler);

		void destroy();
		AttackResult attack(const Attack& attack);
		MarchStatus march();

	protected:
		static constexpr Speed<Cells> DEFAULT_SPEED = 1;

	protected:
		Health _health;
		ICombatSystem& _combatSystem;
		IMarchSystem& _marchSystem;
	};
}