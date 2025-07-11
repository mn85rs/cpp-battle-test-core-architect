#include "CombatSystem.hpp"

#include <Scenario/System/Map/IMap.hpp>
#include <algorithm>
#include <format>
#include <ranges>

namespace sw::sc
{
	namespace
	{
		template <typename T>
		constexpr bool between(const T& value, const T& min, const T& max)
		{
			return value >= min && value <= max;
		}

		template <typename T>
		constexpr auto distance(const T& value1, const T& value2)
		{
			return value1 > value2 ? value1 - value2 : value2 - value1;
		}

		constexpr Cells cellDistance(const Coord<Cells>& cell1, const Coord<Cells>& cell2)
		{
			const auto distanceX = distance(cell1.x(), cell2.x());
			const auto distanceY = distance(cell1.y(), cell2.y());

			return std::max(distanceX, distanceY);
		}

		CombatSystem::Units getTargetsInAttackArea(
			const CombatSystem::Units& unitsInsideAttackArea, const CombatSystem::Units& targetUnits)
		{
			auto units = unitsInsideAttackArea
						 | std::views::filter([&targetUnits](const auto targetUnitId)
											  { return targetUnits.contains(targetUnitId); });

			return CombatSystem::Units(units.begin(), units.end());
		}

		EntityId selectRandomUnit(const CombatSystem::Units& units)
		{
			auto targetUnitIdIt = units.cbegin();
			std::advance(targetUnitIdIt, rand() % units.size());
			return *targetUnitIdIt;
		}

		CombatSystem::Units getTargetsCoveredByAttack(TargetCoverage targetCoverage, CombatSystem::Units&& targets)
		{
			if (TargetCoverage::AllTargets == targetCoverage)
			{
				return std::move(targets);
			}

			return {selectRandomUnit(targets)};
		}

		bool isUnitWithinAttackRange(
			const Coord<Cells>& attackerCell,
			const Coord<Cells>& unitCell,
			AttackRange<Cells> minRange,
			AttackRange<Cells> maxRange,
			Cells rangePenalty = 0)
		{
			const AttackRange<Cells> deminishedMinRange = minRange - rangePenalty;
			const AttackRange<Cells> deminishedMaxRange = maxRange - rangePenalty;

			const auto unitDistance = cellDistance(attackerCell, unitCell);

			return between(unitDistance, deminishedMinRange, deminishedMaxRange);
		}
	}

	CombatSystem::CombatSystem(const IMap& map) :
			_map(map)
	{}

	AttackResult CombatSystem::attackByUnit(EntityId attackerUnitId, const Attack& attack)
	{
		const auto& targetUnits = _targets.at(attack.type);
		const auto unitsInAttackArea = getUnitsInAttackArea(attackerUnitId, attack.minRange, attack.maxRange);
		auto targetsInAttackArea = getTargetsInAttackArea(unitsInAttackArea, targetUnits);

		if (targetsInAttackArea.empty())
		{
			return AttackResult::MissedAttack;
		}

		const auto targetsCoveredByAttack
			= getTargetsCoveredByAttack(attack.targetCoverage, std::move(targetsInAttackArea));

		for (const auto targetUnitId : targetsCoveredByAttack)
		{
			attackTarget(
				attackerUnitId, targetUnitId, _takeDamageHandlers.at(targetUnitId), attack.type, attack.damage);
		}

		return AttackResult::SuccessfulAttack;
	}

	void CombatSystem::registerUnitAsTarget(
		EntityId targetUnitId, AttackTypes attackTypes, TakeDamageHandler takeDamageHandler)
	{
		for (const auto attackType : attackTypes)
		{
			_targets[attackType].insert(targetUnitId);
		}

		_takeDamageHandlers.insert_or_assign(targetUnitId, std::move(takeDamageHandler));
	}

	void CombatSystem::unRegisterUnitAsTarget(EntityId targetUnitId, AttackTypes attackTypes)
	{
		for (const auto attackType : attackTypes)
		{
			_targets.at(attackType).erase(targetUnitId);
		}

		const bool unitIsNoLongerTarget = std::ranges::all_of(
			_targets, [targetUnitId](const auto& pair) { return !pair.second.contains(targetUnitId); });

		if (unitIsNoLongerTarget)
		{
			_takeDamageHandlers.erase(targetUnitId);
		}
	}

	void CombatSystem::subscribeEvents(EventHandler eventHandler)
	{
		_eventHandler = std::move(eventHandler);
	}

	void CombatSystem::attackTarget(
		EntityId attackerUnitId,
		EntityId targetUnitId,
		const TakeDamageHandler& takeDamageHandler,
		AttackType attackType,
		Damage<HitPoints> damage) const
	{
		takeDamageHandler(
			attackType,
			damage,
			[attackerUnitId, targetUnitId, damage, this](HitPoints targetRemainingHP)
			{
				if (_eventHandler)
				{
					_eventHandler(io::UnitAttacked{attackerUnitId, targetUnitId, damage, targetRemainingHP});
				}
			});
	}

	CombatSystem::Units CombatSystem::getUnitsInAttackArea(
		EntityId attackerUnitId, AttackRange<Cells> minRange, AttackRange<Cells> maxRange) const
	{
		if (minRange > maxRange)
		{
			throw std::runtime_error(
				std::format("Error: minimal range {} for attack is greater than maximum range {}", minRange, maxRange));
		}

		if (0 == maxRange)
		{
			throw std::runtime_error("Error: max range for attack is 0");
		}

		const auto attackerUnitCell = _map.getUnitCell(attackerUnitId);
		const auto unitsWithinMaxRange = _map.getUnitsInArea(attackerUnitCell, maxRange);
		auto unitsInsideAttackArea
			= unitsWithinMaxRange
			  | std::views::filter(
				  [&](const auto unitId)
				  {
					  return (unitId != attackerUnitId)
							 && isUnitWithinAttackRange(attackerUnitCell, _map.getUnitCell(unitId), minRange, maxRange);
				  });

		return Units(unitsInsideAttackArea.begin(), unitsInsideAttackArea.end());
	}

	ICombatSystemPtr createCombatSystem(const IMap& map)
	{
		return std::make_unique<CombatSystem>(map);
	}
}
