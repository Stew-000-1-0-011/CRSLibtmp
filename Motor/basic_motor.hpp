#pragma once

#include "marker.hpp"
#include "normal_state.hpp"
#include "basic_state.hpp"
#include "emergency_state.hpp"

namespace CRSLib::Motor
{
	template<class T>
	concept normal_motor = motor<T> && normal<typename T::State> && requires(T mut)
	{
		{change<Stop>(mut)} -> motor;
		{change<TurnOff>(mut)} -> motor;
		{panic<EmergencyStop>(mut)} -> motor;
		{panic<EmergencyTurnOff>(mut)} -> motor;
	};

	template<normal_motor Self>
	struct NormalMotor
	{};


	template<class T>
	concept emergency_motor = motor<T> && emergency<typename T::State>;

	template<emergency_motor Self>
	struct EmergencyMotor
	{};
}