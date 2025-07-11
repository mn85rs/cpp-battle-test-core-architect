#pragma once

#include <Scenario/IUnit.hpp>

namespace sw::sc
{
	class Unit : public IUnit
	{
	public:
		Unit(EntityId id);

		virtual ~Unit() = default;

		EntityId id() const override;
		void subscribeEvents(EventHandler eventHandler) override;

	protected:
		void destroy();

	private:
		EventHandler _eventHandler;
		EntityId _id;
	};
}
