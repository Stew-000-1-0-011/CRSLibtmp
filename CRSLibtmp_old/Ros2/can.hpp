#pragma once

#include <cstring>
#include <memory>
#include <concepts>

#include <rclcpp/rclcpp.hpp>
#include <can_plugins2/msg/frame.hpp>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Can/utility.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>
#include <CRSLibtmp/Can/MainPC/letterbox.hpp>
#include <CRSLibtmp/Can/utility.hpp>

namespace CRSLib::Ros2
{
	namespace CanTxRxVersion
	{
		class CanPillarbox final
		{
			rclcpp::Publisher<can_plugins2::msg::Frame>::SharedPtr pub;
			u32 id;

			public:
			CanPillarbox(const rclcpp::Publisher<can_plugins2::msg::Frame>::SharedPtr& pub, const u32 id) noexcept:
				pub{pub},
				id{id}
			{}

			CanPillarbox(CanPillarbox&&) = default;

			void post(const Can::DataField& data)
			{
				can_plugins2::msg::Frame frame{};
				frame.id = id;
				frame.dlc = data.dlc;
				std::memcpy(frame.data.data(), data.buffer, data.dlc);

				pub->publish(frame);
			}
		};

		class CanLetterboxMaker final
		{
			rclcpp::Node& node;
			rclcpp::SubscriptionOptions options;
			size_t queue_size;
			u32 id;

			public:
			CanLetterboxMaker(rclcpp::Node& node, const u32 id, const rclcpp::SubscriptionOptions& options = {}) noexcept:
				node{node},
				options{options},
				id{id}
			{}

			template<Can::MainPC::callback_shared_ptr CallbackSharedPtr>
			struct Letterbox final : Can::MainPC::LetterboxMarker
			{
				typename CallbackSharedPtr::weak_type callback_wp;
				u32 id;
				rclcpp::Subscription<can_plugins2::msg::Frame>::SharedPtr can_rx_sub{};

				Letterbox(const CallbackSharedPtr& callback_sp, const u32 id, rclcpp::Node& node, const rclcpp::SubscriptionOptions& options):
					callback_wp{callback_sp},
					id{id}
				{
					can_rx_sub = node.create_subscription<can_plugins2::msg::Frame>("can_rx", 100, std::bind(&Letterbox::callback, this, std::placeholders::_1), options);
				}


				void callback(const can_plugins2::msg::Frame::ConstSharedPtr frame)
				{
					if(const auto callback_sp = callback_wp.lock(); callback_sp)
					{
						if(frame->id == id)
						{
							Can::DataField data{.dlc=frame->dlc};
							std::memcpy(data.buffer, frame->data.data(), frame->dlc);
							callback_sp->callback(data);
						}
					}
				}
			};

			template<Can::MainPC::callback_shared_ptr CallbackSharedPtr>
			auto operator()(const CallbackSharedPtr& callback_sp)
			{
				return std::make_unique<Letterbox<CallbackSharedPtr>>(callback_sp, id, node, options);
			}
		};
	}

	using CanTxRxVersion::CanPillarbox;
	using CanTxRxVersion::CanLetterboxMaker;

}