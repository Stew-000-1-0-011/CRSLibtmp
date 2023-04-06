#pragma once

#include <concepts>

#include "normal_state.hpp"
#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	struct SpeedControlled : Normal
	{};

	template<class T>
	concept speed_controlled_motor = normal_motor<T> && requires(T mut, const T imut, float x)
	{
		{mut.update_speed(x)};
		{imut.get_speed()} -> std::convertible_to<float>;
	};
}