#pragma once

#include <concepts>

#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	template<class T>
	concept position_controlled_motor = basic_motor<T> && requires(T mut, const T imut, float x)
	{
		{mut.update_position(x)};  // 無限回転(degree)
		{mut.add_position(x)};
	};
}