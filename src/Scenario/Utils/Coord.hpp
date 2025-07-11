#pragma once

namespace sw::sc
{
	template <typename T>
	class Coord
	{
	public:
		Coord(T x = 0, T y = 0) :
				_x(x),
				_y(y)
		{}

		T x() const
		{
			return _x;
		}

		T y() const
		{
			return _y;
		}

		bool operator==(const Coord& other) const
		{
			return _x == other._x && _y == other._y;
		}

	private:
		T _x;
		T _y;
	};
}
