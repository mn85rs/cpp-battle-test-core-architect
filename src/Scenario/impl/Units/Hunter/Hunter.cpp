#include "Hunter.hpp"

#include <IO/Commands/SpawnHunter.hpp>

namespace sw::sc
{
	Hunter::Hunter(
		EntityId id,
		HitPoints maxHP,
		Strength strenght,
		Agility agility,
		Range range,
		ICombatSystem& combatSystem,
		IMarchSystem& movementSystem,
		IEventsDispatcher& eventDispatcher) :
			Trooper(id, maxHP, combatSystem, movementSystem, eventDispatcher),
			_strenght(strenght),
			_agility(agility),
			_range(range)
	{}

	const char* Hunter::type() const
	{
		return "Hunter";
	}

	TurnStatus Hunter::takeTurn()
	{
		if (AttackResult::SuccessfulAttack == attack(getRangeAttack()))
		{
			return TurnStatus::Acted;
		}

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

	Attack Hunter::getCloseAttack() const
	{
		return {
			AttackType::CloseAttack,
			_strenght.value(),
			TargetCoverage::RandomTarget,
			DEFAULT_CLOSE_ATTACK_RANGE,
			DEFAULT_CLOSE_ATTACK_RANGE};
	}

	Attack Hunter::getRangeAttack() const
	{
		return {
			AttackType::RangeAttack,
			_agility.value(),
			TargetCoverage::RandomTarget,
			DEFAULT_RANGE_ATTACK_MIN_RANGE,
			_range.value()};
	}

	IUnitPtr createUnit(const io::SpawnHunter& command, IUnitCreationContext& context)
	{
		return std::make_unique<Hunter>(
			command.unitId,
			command.hp,
			command.strength,
			command.agility,
			command.range,
			context.getCombatSystem(),
			context.getMarchSystem(),
			context.getEventsDispatcher());
	}
}
