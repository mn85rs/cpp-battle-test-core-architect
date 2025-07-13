#pragma once

#include <functional>

namespace sw::ed
{
	template <typename Event>
	struct IEventDispatcher
	{
		using EventHandler = std::function<void (const Event&)>;

		virtual void subscribe(EventHandler handler) = 0;
		virtual void publish(const Event& event) = 0;
	};


	template <typename... Event>
	struct IEventsDispatcher : public virtual IEventDispatcher<Event>...
	{
		using IEventDispatcher<Event>::publish...;
		using IEventDispatcher<Event>::subscribe...;

		virtual ~IEventsDispatcher() = default;
	};
}
