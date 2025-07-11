#include "Trooper.hpp"

namespace sw::sc
{
	Trooper::Trooper(EntityId id, HitPoints maxHP, ICombatSystem& combatSystem, IMarchSystem& marchSystem) :
			Unit(id),
			_health(maxHP),
			_combatSystem(combatSystem),
			_marchSystem(marchSystem)
	{
		_combatSystem.registerUnitAsTarget(
			id,
			{AttackType::CloseAttack, AttackType::RangeAttack},
			[this](auto&&... args) { takeDamage(std::forward<decltype(args)>(args)...); });
	}

	bool Trooper::isAlive() const
	{
		return _health.HP() > 0;
	}

	void Trooper::takeDamage(
		AttackType attackType, Damage<HitPoints> damage, ICombatSystem::TargetStatusHandler targetStatusHandler)
	{
		_health.decrease(damage);

		targetStatusHandler(_health.HP());

		if (0 == _health.HP())
		{
			destroy();
		}
	}

	void Trooper::destroy()
	{
		Unit::destroy();
		_combatSystem.unRegisterUnitAsTarget(id(), {AttackType::CloseAttack, AttackType::RangeAttack});
	}

	AttackResult Trooper::attack(const Attack& attack)
	{
		return _combatSystem.attackByUnit(id(), attack);
	}

	MarchStatus Trooper::march()
	{
		return _marchSystem.marchUnit(id(), DEFAULT_SPEED);
	}
}