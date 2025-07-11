#include "Unit.hpp"

namespace sw::sc
{
	Unit::Unit(EntityId id) :
			_id(id)
	{}

	EntityId Unit::id() const
	{
		return _id;
	}

	void Unit::subscribeEvents(EventHandler eventHandler)
	{
		_eventHandler = eventHandler;
	}

	void Unit::destroy()
	{
		if (_eventHandler)
		{
			_eventHandler(io::UnitDied{id()});
		}
	}
}
