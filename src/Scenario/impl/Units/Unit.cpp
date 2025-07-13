#include "Unit.hpp"

namespace sw::sc
{
	Unit::Unit(EntityId id, IEventsDispatcher& eventDispatcher) :
			_id(id),
			_eventDispatcher(eventDispatcher)
	{}

	EntityId Unit::id() const
	{
		return _id;
	}

	void Unit::destroy()
	{
		_eventDispatcher.publish(io::UnitDied{id()});
	}
}
