#pragma once

#include <CRSLibtmp/Motor.hpp>
#include <CRSLibtmp/Math.hpp>

namespace CRSLib::Mechanism
{
	template<Motor::speed_controlled_motor M>
	struct OmniWheel final
	{
		M motor;
		Math::Pose2D pose;
	};
}