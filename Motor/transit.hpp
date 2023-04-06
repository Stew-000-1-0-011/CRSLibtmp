#pragma once

#include "marker.hpp"
#include "normal_state.hpp"
#include "emergency_state.hpp"

namespace CRSLib::Motor
{
	// 平常時から平常時に遷移。少なくともStopとTurnOffに移せる
	template<normal NextState, motor Motor, normal CurrentState = typename Motor::State>
	motor auto change(Motor&&);

	// 平常時から緊急時に遷移。少なくともEmergencyStopとEmergencyTurnOffに移せる
	template<emergency NextState, motor Motor>
	motor auto panic(Motor&&);

	// 緊急時から緊急時に遷移。
	template<normal NextState, motor Motor, emergency CurrentState = typename Motor::State>
	motor auto recover(Motor&&);
}