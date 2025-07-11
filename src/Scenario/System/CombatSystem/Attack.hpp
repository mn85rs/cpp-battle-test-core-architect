#pragma once

#include <Scenario/Utils/Common.hpp>

namespace sw::sc
{
	enum class AttackResult
	{
		SuccessfulAttack,
		MissedAttack
	};

	template <typename T>
	using AttackRange = T;

	template <typename T>
	using Damage = T;

	enum class TargetCoverage
	{
		RandomTarget,
		AllTargets
	};

	enum class AttackType
	{
		CloseAttack,
		RangeAttack
	};

	struct Attack
	{
		AttackType type;
		Damage<HitPoints> damage;
		TargetCoverage targetCoverage;
		AttackRange<Cells> minRange;
		AttackRange<Cells> maxRange;
	};
}
