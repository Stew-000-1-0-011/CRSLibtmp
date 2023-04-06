#pragma once

#include "normal_state.hpp"
#include "emergency_state.hpp"

namespace CRSLib::Motor
{
	struct Stop : Normal
	{};
	struct TurnOff : Normal
	{};
	struct EmergencyStop : Emergency
	{};
	struct EmergencyTurnOff : Emergency
	{};
}