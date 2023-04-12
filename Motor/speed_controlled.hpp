#pragma once

#include <concepts>

#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	template<class T>
	concept speed_controlled_motor = basic_motor<T> && requires(T mut, const T imut, float x)
	{
		{mut.update_speed(x)};  // rpm
		{mut.add_speed(x)};
	};
}