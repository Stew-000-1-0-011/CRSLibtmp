#pragma once

#include <string_view>

#include <rclcpp/rclcpp.hpp>
#include <CRSLibtmp/MetaType.hpp>

namespace CRSLib::Ros2
{
	namespace Parameter
	{
		namespace Implement
		{
			template<MetaType::Concept::is_primitive auto type>
			auto read_inner(const rclcpp::Parameter& parameter, const std::string_view name)
			{
				#define Stew_ros2_parameter_read_inner_ifconstexpr a
				if constexpr(std::same_as<decltype(type), PrimitiveType>)
				{
					try
					{
						
					}
				}
			}
		}

		template<MetaType::Concept::is_type auto>
		auto read(const std::string_view parameter_name)
		{
			return read_inner(parameter_name);
		}
	}
}
