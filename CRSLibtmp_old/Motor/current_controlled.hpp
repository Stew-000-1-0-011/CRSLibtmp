#pragma once

#include <concepts>

#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	template<class T>
	concept current_controlled_motor = basic_motor<T> && requires(T mut, const T imut, float x)
	{
		{mut.update_current(x)};  // Ampere
		{mut.add_current(x)};
	};
}