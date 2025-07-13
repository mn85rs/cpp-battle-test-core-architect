#pragma once

#include "IEventDispatcher.hpp"
#include <vector>

namespace sw::ed
{
	//class EventsDispatcher2
	//{
	//public:
	//	template <typename Event>
	//	void subscribe(std::function<void(const Event&)> eventHandler)
	//	{
	//		_handlers[std::type_index(typeid(Event))].emplace_back(
	//			[eventHandler = std::move(eventHandler)](const void* event)
	//			{ eventHandler(*reinterpret_cast<const Event*>(event)); });
	//	}

	//	template <typename Event>
	//	void publish(const Event& event)
	//	{
	//		for (const auto& handler : _handlers.at(std::type_index(typeid(Event))))
	//		{
	//			handler(&event);
	//		}
	//	}

	//private:
	//	using EventHandlerWrapper = std::function<void(const void*)>;

	//	std::unordered_map<std::type_index, std::vector<EventHandlerWrapper>> _handlers;
	//};

	template <typename Event>
	class EventDispatcher : public virtual IEventDispatcher<Event>
	{
	public:
		void subscribe(typename IEventDispatcher<Event>::EventHandler handler) override
		{
			_handlers.emplace_back(std::move(handler));
		}

		void publish(const Event& event) override
		{
			for (const auto& handler : _handlers)
			{
				handler(event);
			}
		}

	private:
		std::vector<typename IEventDispatcher<Event>::EventHandler> _handlers;
	};

	template <typename... Event>
	struct EventsDispatcher : public IEventsDispatcher<Event...>,
							  private EventDispatcher<Event>...
	{};

	template <typename... Event, typename F>
	void setEventsHandler(ed::IEventsDispatcher<Event...>& ed, F&& f)
	{
		(static_cast<ed::IEventDispatcher<Event>&>(ed).subscribe(std::forward<F>(f)), ...);
	}
}
