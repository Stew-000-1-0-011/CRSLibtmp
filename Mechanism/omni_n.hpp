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
	template<omni_wheel ... OmniWheels>
	struct OmniN final
	{
		std::tuple<OmniWheels ...> wheels;

		OmniN(cvref_same<OmniWheels> auto&& ... wheels):
			wheels{std::forward<decltype(wheels)>(wheels)...}
		{}

		void turn_off()
		{
			
		}

		void update(const Math::Pose2D& body_speed)
		{
			[this]<size_t ... indices>(const Math::Pose2D& body_speed, std::index_sequence<indices...>)
			{
				[](auto&& wheel, const Math::Pose2D& body_speed)
				{
					wheel.update(body_speed);
				}(std::get<indices>(wheels), body_speed) ...;
			}(body_speed, std::make_index_sequence<sizeof...(Motors)>{});
		}
	};
}