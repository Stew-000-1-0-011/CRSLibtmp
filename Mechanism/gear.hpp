#pragma once

#include <cmath>
#include <concepts>
#include <CRSLibtmp/utility.hpp>
#include <CRSLibtmp/Motor.hpp>
#include <CRSLibtmp/Math/ranged_scalar.hpp>

namespace CRSLib::Mechanism
{
	namespace Implement
	{
		struct GearMarker
		{};
	}

	template<class T>
	concept gear = std::derived_from<T, Implement::GearMarker>;

	template<Motor::basic_motor M, is_std_ratio auto gear_ratio_reference_to_motor>
	requires (ratio_to_floating<double>(gear_ratio_reference_to_motor) != 0)
	struct Gear final
	{
		// static constexpr auto max_rotation = ratio_to_floating<double>(gear_ratio_reference_to_motor) * 360.0;

		M motor;
		double current_angle;

		Gear(M&& motor, const double initial_angle):
			motor{std::move(motor)},
			current_angle{initial_angle}
		{}

		void update_angle(const Math::Degree degree)
		// [[expected: 0 <= degree && degree <=360]]
		requires Motor::position_controlled_motor<M>
		{
			constexpr auto is_nearist_angle = [](const double x){return x * x < 180 * 180;};
			if(const auto diff = degree - current_angle; is_nearist_angle(diff))
			{
				motor.add_position(diff * ratio_to_floating<double>(gear_ratio_reference_to_motor));
			}
			else
			{
				const auto fixed_diff = static_cast<double>(std::signbit(diff)) * 360.0 + diff;
				motor.add_position(fixed_diff * ratio_to_floating<double>(gear_ratio_reference_to_motor));
			}

			current_angle = degree;
		}
	};
}