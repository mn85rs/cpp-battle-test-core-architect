#include "Swordsman.hpp"

#include <IO/Commands/SpawnSwordsman.hpp>

namespace sw::sc
{
	Swordsman::Swordsman(
		EntityId id, HitPoints maxHP, Strength strenght, ICombatSystem& combatSystem, IMarchSystem& movementSystem) :
			Trooper(id, maxHP, combatSystem, movementSystem),
			_strenght(strenght)
	{}

	const char* Swordsman::type() const
	{
		return "Swordsman";
	}

	TurnStatus Swordsman::takeTurn()
	{
		if (AttackResult::SuccessfulAttack == attack(getCloseAttack()))
		{
			return TurnStatus::Acted;
		}

		if (MarchStatus::Marching == march())
		{
			return TurnStatus::Acted;
		}

		return TurnStatus::NoActions;
	}

	Attack Swordsman::getCloseAttack() const
	{
		return {
			AttackType::CloseAttack,
			_strenght.value(),
			TargetCoverage::RandomTarget,
			DEFAULT_CLOSE_ATTACK_RANGE,
			DEFAULT_CLOSE_ATTACK_RANGE};
	}

	IUnitPtr createUnit(const io::SpawnSwordsman& command, IUnitCreationContext& context)
	{
		return std::make_unique<Swordsman>(
			command.unitId, command.hp, command.strength, context.GetCombatSystem(), context.GetMarchSystem());
	}
}
