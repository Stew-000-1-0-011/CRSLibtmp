#pragma once

#include <concepts>
#include <utility>

#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/Motor.hpp>
#include <CRSLibtmp/Math.hpp>

namespace CRSLib::Mechanism
{
	namespace Implement
	{
		struct SteeringWheelMarker
		{};
	}

	template<class T>
	concept steering_wheel = std::derived_from<T, Implement::SteeringWheelMarker>;

	template<Motor::position_controlled_motor SteeringMotor, Motor::speed_controlled_motor DrivingMotor>
	struct SteeringWheel final : Implement::SteeringWheelMarker
	{
		SteeringMotor steering;
		DrivingMotor driving;
		Math::Vec2D position;
		bool last_is_negative{};

		void update(const double steering_angle, const double driving_speed)
		{
			
			steering.update_position(steering_angle);
			driving.update_speed(driving_speed);
		}
	};
}