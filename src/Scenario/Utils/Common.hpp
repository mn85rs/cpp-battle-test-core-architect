#pragma once

#include <cstdint>

namespace sw::sc
{
	using EntityId = uint32_t;

	using Cells = uint32_t;

	template <typename T>
	using Width = T;

	template <typename T>
	using Height = T;

	using HitPoints = uint32_t;

	template <typename T>
	using Speed = T;
}
