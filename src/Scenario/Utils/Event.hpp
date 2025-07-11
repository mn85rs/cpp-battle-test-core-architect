#pragma once

#include <variant>

namespace sw::sc
{
	template <typename... Ts>
	using Event = std::variant<Ts...>;
}
