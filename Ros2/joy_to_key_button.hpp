#pragma once

#include <cstddef>
#include <vector>
#include <array>
#include <queue>
#include <shared_mutex>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>

namespace CRSLib::Ros2
{
	template<class KeyMap>
	class JoyToKeyButton final
	{
	public:
		using Axes = typename KeyMap::Axes;
		using Buttons = typename KeyMap::Buttons;

	private:
		mutable std::shared_mutex shared_mtx{};

		std::array<float, Axes::N> axes{};
		std::array<bool, Buttons::N> buttons{};
		std::array<std::queue<rclcpp::Time>, Buttons::N> buttons_up{};
		std::array<std::queue<rclcpp::Time>, Buttons::N> buttons_down{};
		const std::size_t button_queue_size;

		rclcpp::Logger logger;
		rclcpp::Clock::SharedPtr clock;
		rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub{};

	public:
		// Subscriberの重複は一切検知しないよ。
		JoyToKeyButton(rclcpp::Node& node, const char *const joy_topic_name, const rclcpp::SubscriptionOptions& options = {}, const std::size_t button_queue_size = 1):
			button_queue_size{button_queue_size},
			logger{node.get_logger()},
			clock{node.get_clock()}
		{
			joy_sub = node.create_subscription<sensor_msgs::msg::Joy>(joy_topic_name, 1, std::bind(&JoyToKeyButton::sub_update, this, std::placeholders::_1), options);
		}

		bool is_being_pushed(const typename Buttons::Enum button) const
		{
			std::shared_lock lock{shared_mtx};

			return buttons[button];
			// 戻り値のbool型の値が構築されてからlockのデストラクタが呼ばれる(はず)
		}

		bool is_pushed_up(const typename Buttons::Enum button, const rclcpp::Duration& interval = rclcpp::Duration{0, 0})
		{
			std::shared_lock lock{shared_mtx};

			while(!buttons_up[button].empty())
			{
				if(interval == rclcpp::Duration{0, 0} || clock->now() - buttons_up[button].front() < interval)
				{
					buttons_up[button].pop();
					return true;
				}
				buttons_up[button].pop();
			}

			return false;
		}

		bool is_pushed_down(const typename Buttons::Enum button, const rclcpp::Duration& interval = rclcpp::Duration{0, 0})
		{
			std::shared_lock lock{shared_mtx};

			while(!buttons_down[button].empty())
			{
				if(interval == rclcpp::Duration{0, 0} || clock->now() - buttons_down[button].front() < interval)
				{
					buttons_down[button].pop();
					return true;
				}
				buttons_down[button].pop();
			}
			
			return false;
		}

		float get_axis(const typename Axes::Enum axe) const
		{
			std::shared_lock lock{shared_mtx};

			return axes[axe];
		}

		auto& get_axes() const
		{
			std::shared_lock lock{shared_mtx};

			return axes;
		}

	private:
		void sub_update(const sensor_msgs::msg::Joy& joy)
		{
			std::lock_guard lock{shared_mtx};

			{
				bool flag{};
				if(joy.axes.size() != Axes::N)
				{
					RCLCPP_WARN(logger, "size of axes is differ from Axes::N.");
					flag = true;
				}
				if(joy.buttons.size() != Buttons::N)
				{
					RCLCPP_WARN(logger, "size of buttons is differ from Buttons::N.");
					flag = true;
				}
				if(flag) return;
			}
			
			for(std::underlying_type_t<decltype(Buttons::N)> i = 0; i < Axes::N; ++i)
			{
				axes[i] = joy.axes[i];
			}

			for(std::underlying_type_t<decltype(Buttons::N)> i = 0; i < Buttons::N; ++i)
			{
				const bool latest_button = joy.buttons[i];

				if(!latest_button && buttons[i])
				{
					if(buttons_up[i].size() > button_queue_size) buttons_up[i].pop();
					buttons_up[i].push(joy.header.stamp);
				}
				if(latest_button && !buttons[i])
				{
					if(buttons_down[i].size() > button_queue_size) buttons_down[i].pop();
					buttons_down[i].push(joy.header.stamp);
				}
				buttons[i] = latest_button;
			}
		}
	};
}