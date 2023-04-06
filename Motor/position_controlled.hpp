#pragma once

#include <concepts>

#include "normal_state.hpp"
#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	struct PositionControlled : Normal
	{};

	template<class T>
	concept position_controlled_motor = normal_motor<T> && requires(T mut, const T imut, float x)
	{
		{mut.update_position(x)};
		{imut.get_position()} -> std::convertible_to<float>;
	};
}