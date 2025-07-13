#pragma once

#include <Scenario/impl/Units/Trooper.hpp>

namespace sw::sc
{
	class Swordsman : public Trooper
	{
	public:
		Swordsman(
			EntityId id,
			HitPoints maxHP,
			Strength strenght,
			ICombatSystem& combatSystem,
			IMarchSystem& movementSystem,
			IEventsDispatcher& eventDispatcher);

		const char* type() const override;

	private:
		TurnStatus takeTurn() override;

	private:
		Attack getCloseAttack() const;

		static constexpr AttackRange<Cells> DEFAULT_CLOSE_ATTACK_RANGE = 1;

	private:
		Strength _strenght;
	};
}
