#pragma once

#include <utility>
#include <numbers>

#include <CRSLibtmp/std_type.hpp>
#include <CRSLibtmp/Can/MainPC/pillarbox.hpp>
#include "basic_motor.hpp"

namespace CRSLib::Motor
{
	template<Can::MainPC::pillarbox Pillarbox>
	class Shirasu final : MotorMarker
	{
		enum class State : u8
		{
			disable,
			duty,
			current,
			velocity,
			position,
			homing
		};

		Pillarbox command;
		Pillarbox target;
		State state{State::disable};

		public:
		Shirasu(Pillarbox&& command, Pillarbox&& target):
			command{std::move(command)},
			target{std::move(target)}
		{}

		void stop()
		{
			change_state(State::velocity);
			update_speed(0);
		}

		void turn_off()
		{
			change_state(State::disable);
		}

		void update_current(const float x)
		{
			change_state(State::current);
			send_target(x);
		}

		void update_speed(const float x)
		{
			change_state(State::velocity);
			send_target(x * std::numbers::pi_v<float> * 2 / 60);
		}

		void update_position(const float x)
		{
			change_state(State::position);
			send_target(x / 360 * 2 * std::numbers::pi_v<float>);
		} 

		private:
		void change_state(const State s)
		{
			if(state == s)
			{
				return;
			}
			else
			{
				command.post(.buffer={to_underlying(s)}, .dlc=1);
			}
		}

		void send_target(const float x)
		{
			Can::DataField data{.dlc=4};
			std::memcpy(data.buffer, &x, 4);
			std::swap(std::buffer[0], std::buffer[3]);
			std::swap(std::buffer[1], std::buffer[2]);
			target.post(data);
		}
	};
}