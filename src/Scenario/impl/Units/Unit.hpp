#pragma once

#include <Scenario/IUnit.hpp>

namespace sw::sc
{
	class Unit : public IUnit
	{
	public:
		Unit(EntityId id, IEventsDispatcher& eventDispatcher);

		virtual ~Unit() = default;

		EntityId id() const override;

	protected:
		void destroy();

	private:
		IEventsDispatcher& _eventDispatcher;
		EntityId _id;
	};
}
