#pragma once

#include <utility>

#include <rclcpp/rclcpp.hpp>

#include <CRSLibtmp/reporter.hpp>

namespace CRSLib::Ros2
{
	struct RosReporter final
	{
		rclcpp::Logger logger;
		rclcpp::Logger::Level level;

		RosReporter(const rclcpp::Logger& logger, const rclcpp::Logger::Level level):
			logger{logger},
			level{level}
		{}

		void operator()(auto&& ... args) noexcept
		{
			switch(level)
			{
				case rclcpp::Logger::Level::Debug:
				RCLCPP_DEBUG(logger, std::forward<decltype(args)>(args) ...);
				break;

				case rclcpp::Logger::Level::Info:
				RCLCPP_INFO(logger, std::forward<decltype(args)>(args) ...);
				break;

				case rclcpp::Logger::Level::Warn:
				RCLCPP_WARN(logger, std::forward<decltype(args)>(args) ...);
				break;

				case rclcpp::Logger::Level::Error:
				RCLCPP_ERROR(logger, std::forward<decltype(args)>(args) ...);
				break;

				case rclcpp::Logger::Level::Fatal:
				RCLCPP_FATAL(logger, std::forward<decltype(args)>(args) ...);

				case rclcpp::Logger::Level::Unset:
				RCLCPP_FATAL(logger, "No! Log level of this reporter is Unset.");
			}
		}
	};

	static_assert(reporter<RosReporter>);
}