#pragma once

#include <Scenario/impl/Units/Trooper.hpp>

namespace sw::sc
{
	class Hunter : public Trooper
	{
	public:
		Hunter(
			EntityId id,
			HitPoints maxHP,
			Strength strenght,
			Agility agility,
			Range range,
			ICombatSystem& combatSystem,
			IMarchSystem& movementSystem,
			IEventsDispatcher& eventDispatcher);

		const char* type() const override;

	private:
		TurnStatus takeTurn() override;

	private:
		Attack getCloseAttack() const;
		Attack getRangeAttack() const;

		static constexpr AttackRange<Cells> DEFAULT_CLOSE_ATTACK_RANGE = 1;
		static constexpr AttackRange<Cells> DEFAULT_RANGE_ATTACK_MIN_RANGE = 2;

	private:
		Strength _strenght;
		Agility _agility;
		Range _range;
	};

}
