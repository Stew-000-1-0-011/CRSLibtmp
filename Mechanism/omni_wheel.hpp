#pragma once

#include <concepts>
#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/Motor.hpp>
#include <CRSLibtmp/Math.hpp>

namespace CRSLib::Mechanism
{
	namespace Implement
	{
		struct OmniWheelMarker
		{};
	}

	template<class T>
	concept omni_wheel = std::derived_from<T, Implement::OmniWheelMarker>;

	template<Motor::speed_controlled_motor M>
	struct OmniWheel final : Implement::OmniWheelMarker
	{
		M motor;
		Math::Pose2D pose;
		const double coefficient;

		OmniWheel(cvref_same<M> auto&& motor, cvref_same<Math::Pose2D> auto&& pose, const double revolution_ratio, const double wheel_raius):
			motor{std::forward<decltype(motor)>(motor)},
			pose{std::forward<decltype(pose)>(pose)},
			coefficient{revolution_ratio / wheel_raius}
		{}

		void update(const Math::Pose2D& body_speed)
		{
			const auto linear_component = blaze::trans(body_speed.point) * Math::unit_vec(pose.theta);
			const auto angular_component = body_speed.theta * blaze::norm(pose.point);
			motor.update_speed((linear_component + angular_component) * coefficient);
		}
	};
}