#pragma once
#include <utility>

struct Point
{
	size_t x = 0;
	size_t y = 0;

	Point(size_t x, size_t y)
		: x(x)
		, y(y)
	{
	}

	Point(Point && rhs)
	{
		this->x = std::move(rhs.x);
		this->y = std::move(rhs.y);
	}

	Point(const Point & rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
	}

	Point &operator=(Point && rhs)
	{
		this->x = std::move(rhs.x);
		this->y = std::move(rhs.y);
		return *this;
	}

	Point &operator=(const Point & rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		return *this;
	}
};