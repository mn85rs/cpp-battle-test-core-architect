#pragma once

#include "details/PrintFieldVisitor.hpp"
#include <iostream>
#include <typeindex>
#include <unordered_map>

namespace sw
{
	class EventLog
	{
	public:
		template <class TEvent>
		void log(uint64_t tick, TEvent&& event)
		{
			std::cout << "[" << tick << "] " << std::decay_t<TEvent>::Name << " ";
			PrintFieldVisitor visitor(std::cout);
			event.visit(visitor);
			std::cout << std::endl;
		}
	};
}
