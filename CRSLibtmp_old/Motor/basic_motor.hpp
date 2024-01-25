#pragma once

namespace CRSLib::Motor
{
	struct MotorMarker
	{};

	template<class T>
	concept basic_motor = std::derived_from<T, MotorMarker> && std::move_constructible<T> && requires(T mut)
	{
		mut.stop();
		mut.turn_off();
	};
}
