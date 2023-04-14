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
	namespace CanTxVersion
	{
		template<reporter Reporter>
		class CanPillarbox final
		{
			Reporter reporter;
			std::weak_ptr<rclcpp::Publisher<can_plugins2::msg::Frame>> pub;
			u32 id;

			public:
			CanPillarbox(Reporter&& reporter, std::weak_ptr<rclcpp::Publisher<can_plugins2::msg::Frame>>&& pub, const u32 id):
				reporter{std::move(reporter)},
				pub{std::move(pub)},
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

		class CanClusterLetterbox final
		{
			
		};
	}

	using CanTxVersion::CanPillarbox;
}