#pragma once

#include <utility>
#include <tuple>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/Motor.hpp>
#include <CRSLibtmp/Math.hpp>

#include "omni_wheel.hpp"

namespace CRSLib::Mechanism
{
	template<Motor::speed_controlled_motor ... Motors>
	class OmniN final
	{
		std::tuple<OmniWheel<Motors> ...> wheels;

		public:
		OmniN(cvref_same<OmniWheel<Motors>> auto&& ... wheels):
			wheels{std::forward<decltype(wheels)>(wheels)...}
		{}

		void update(const Math::Pose2D& body_speed)
		{
			[this]<size_t ... indices>(const Math::Pose2D& body_speed, std::index_sequence<indices...>)
			{
				[](auto&& wheel, const Math::Pose2D& body_speed)
				{
					const auto linear_component = blaze::trans(body_speed.point) * Math::unit_vec(wheel.pose.theta);
					const auto angular_component = body_speed.theta * blaze::norm(wheel.pose.point);
				
					wheel.motor.update_speed(linear_component + angular_component);
				}(std::get<indices>(wheels), body_speed) ...;
			}(body_speed, std::make_index_sequence<sizeof...(Motors)>);
		}
	};
}