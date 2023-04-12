#pragma once

#include <cstring>
#include <memory>
#include <concepts>

#include <rclcpp/rclcpp.hpp>
#include <can_plugins2/msg/frame.hpp>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/reporter.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>
#include <CRSLibtmp/Can/utility.hpp>

namespace CRSLib::Ros2
{
	template<reporter Reporter>
	class CanPillarbox final
	{
		Reporter reporter;
		std::weak_ptr<rclcpp::Publisher<can_plugins2::msg::Frame>> pub;
		u32 id;

		public:
		CanPillarbox(Reporter&& reporter, const u32 id, const auto self)
		requires std::derived_from<std::remove_cvref_t<decltype(*self)>, rclcpp::Node>
		:
			reporter{std::move(reporter)},
			pub{self->create_publisher<can_plugins2::msg::Frame>("can_tx", 1)},
			id{id}
		{}

		void post(const Can::DataField& data)
		{
			can_plugins2::msg::Frame frame{};
			frame.id = id;
			frame.dlc = data.dlc;
			std::memcpy(frame.data.data(), data.buffer, data.dlc);

			if(const auto sp = pub.lock(); sp)
			{
				sp->publish(frame);
			}
			else
			{
				reporter("Expired CAN publisher was tried to use.");
			}
		}
	};
}