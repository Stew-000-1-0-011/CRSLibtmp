#pragma once

#include <blaze/Math.h>

namespace CRSLib::Math
{
	using Vec2D = blaze::StaticVector<double, 2>;

	constexpr Vec2D unit_vec(const double theta)
	{
		return {blaze::cos(theta), blaze::sin(theta)};
	}

	constexpr double get_angle(const Vec2D& vec)
	{
		return blaze::atan2(vec[1], vec[0]);
	}
}