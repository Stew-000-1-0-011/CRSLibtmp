// add_*系の制御がほぼできない。いろいろconceptの意味がない。
// 現在の出力が得られるようになった or add_*系にシラスが対応したら書き換えること。

#pragma once

#include <utility>
#include <numbers>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/reporter.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>
#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	enum class ShirasuState : u8
	{
		disable,
		duty,
		current,
		velocity,
		position,
		homing
	};

	template<Can::MainPC::pillarbox Pillarbox, reporter Reporter>
	class Shirasu final : public MotorMarker
	{
		Pillarbox command;
		Pillarbox target;
		Reporter reporter;
		ShirasuState state{ShirasuState::disable};
		float current_current{0};
		float current_speed{0};
		float current_position{0};

		public:
		Shirasu(Pillarbox&& command, Pillarbox&& target, Reporter&& reporter):
			command{std::move(command)},
			target{std::move(target)},
			reporter{std::move(reporter)}
		{}

		void stop()
		{
			change_state(ShirasuState::velocity);
			update_speed(0);
		}

		void turn_off()
		{
			change_state(ShirasuState::disable);
		}

		void update_current(const float x)
		{
			change_state(ShirasuState::current);
			send_target(x);
			current_current = x;
		}

		void add_current(const float x)
		{
			change_state(ShirasuState::current);
			current_current += x;
			send_target(current_current);
		}

		void update_speed(const float x)
		{
			change_state(ShirasuState::velocity);
			send_target(x * std::numbers::pi_v<float> * 2 / 60);
			current_speed = x;
		}

		void add_speed(const float x)
		{
			change_state(ShirasuState::velocity);
			current_speed += x;
			send_target(current_speed * std::numbers::pi_v<float> * 2 / 60);
		}

		void update_position(const float x)
		{
			change_state(ShirasuState::position);
			send_target(x / 360 * 2 * std::numbers::pi_v<float>);
			current_position = x;
		}

		void add_position(const float x)
		{
			change_state(ShirasuState::position);
			current_position += x;
			send_target(current_position / 360 * 2 * std::numbers::pi_v<float>);
		}

		void change_state(const ShirasuState s)
		{
			if(state == s)
			{
				return;
			}
			else
			{
				if(s == ShirasuState::disable || s == ShirasuState::homing)
				{
					current_current = 0;
					current_speed = 0;
					if(s == ShirasuState::homing)
					{
						current_position = 0;
					}
				}
				else if(state != ShirasuState::disable) reporter("This version of Motor::Shirasu may behave unexpectedly in this state transition.");

				state = s;				
				command.post(Can::DataField{.buffer=static_cast<byte>(to_underlying(s)), .dlc=1});
			}
		}

		private:
		void send_target(const float x)
		{
			Can::DataField data{.dlc=4};
			std::memcpy(data.buffer, &x, 4);
			std::swap(data.buffer[0], data.buffer[3]);
			std::swap(data.buffer[1], data.buffer[2]);
			target.post(data);
		}
	};
}