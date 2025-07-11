#pragma once

#include "Common.hpp"
#include <cstdint>

namespace sw::sc
{
	template <typename T, typename Tag>
	class Ability
	{
	public:
		Ability(T value) :
				_value(value)
		{}

		T value() const
		{
			return _value;
		}

	private:
		T _value;
	};

	class StrengthTag;
	using Strength = Ability<uint32_t, StrengthTag>;

	class AgilityTag;
	using Agility = Ability<uint32_t, AgilityTag>;

	class RangeTag;
	using Range = Ability<Cells, RangeTag>;

	class Health
	{
	public:
		Health(HitPoints maxHP) :
				_maxHP(maxHP),
				_HP(maxHP)
		{}

		HitPoints HP() const
		{
			return _HP;
		}

		void decrease(HitPoints hp)
		{
			if (hp > _HP)
			{
				_HP = 0;
				return;
			}

			_HP -= hp;
		}

		void increase(HitPoints hp)
		{
			if (hp > (_maxHP - _HP))
			{
				_HP = _maxHP;
				return;
			}

			_HP += hp;
		}

	private:
		HitPoints _maxHP;
		HitPoints _HP;
	};
}
