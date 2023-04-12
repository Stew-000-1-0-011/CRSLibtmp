#pragma once

#include <utility>
#include <tuple>
#include <numbers>

#include "steering_wheel.hpp"

namespace CRSLib::Mechanism
{
	template<steering_wheel ... SteeringWheels>
	class IndependentSteeringN final
	{
		std::tuple<SteeringWheels ...> wheels;

		IndependentSteeringN(cvref_same<SteeringWheels> auto&& ... wheels):
			wheels{std::forward<decltype(wheels)>(wheels) ...}
		{}

		void spinning_rotate(const double body_angle_speed)
		{
			[this]<size_t ... indices>(const double body_angle_speed, std::index_sequence<indices ...>)
			{
				[this](auto&& wheel, const double body_angle_speed)
				{
					wheel.update(Math::get_angle(wheel.position) + std::numbers::pi_v / 2, body_angle_speed);
				}(std::get<indices>(wheels), body_angle_speed)...;
			}(body_angle_speed, std::make_index_sequence<sizeof...(SteeringWheels)>{});
		}

		void crab_transit(const Math::Pose2D& body_linear_speed)
		{
			[this]<size_t ... indices>(const Math::Pose2D& body_linear_speed, std::index_sequence<indices ...>)
			{
				[this](auto&& wheel, const Math::Pose2D& body_linear_speed)
				{
					wheel.update(body_linear_speed.theta, body_linear_speed.point);
				}(std::get<indices>(wheels), body_linear_speed)...;
			}
		}
	};
}